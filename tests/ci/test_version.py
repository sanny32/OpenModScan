import os
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile
import textwrap
import unittest
from unittest.mock import patch


ROOT = Path(__file__).resolve().parents[2]


def bash_executable():
    if sys.platform != 'win32':
        return 'bash'

    # A bare bash.exe can resolve to the Windows WSL launcher. Use the
    # shell shipped with Git for Windows, which CI already installs.
    git = shutil.which('git')
    if git:
        # Git can be exposed through cmd/, bin/, or mingw64/bin/ (mingw32
        # on 32-bit installs). Check each possible installation root.
        for git_root in list(Path(git).resolve().parents)[:3]:
            for relative_path in ('bin/bash.exe', 'usr/bin/bash.exe'):
                bash = git_root / relative_path
                if bash.is_file():
                    return str(bash)
    raise RuntimeError(
        f'Git Bash was not found alongside git ({git!r}). '
        'Install Git for Windows; WSL is not required.'
    )


class BashSelectionTests(unittest.TestCase):
    def test_windows_uses_git_bash(self):
        for git_path in ('cmd/git.exe', 'bin/git.exe',
                         'mingw64/bin/git.exe', 'mingw32/bin/git.exe'):
            for bash_path in ('bin/bash.exe', 'usr/bin/bash.exe'):
                with self.subTest(git=git_path, bash=bash_path), tempfile.TemporaryDirectory() as directory:
                    git_root = Path(directory) / 'Git'
                    git = git_root / git_path
                    git.parent.mkdir(parents=True)
                    git.touch()
                    bash = git_root / bash_path
                    bash.parent.mkdir(parents=True, exist_ok=True)
                    bash.touch()
                    with patch.object(sys, 'platform', 'win32'), patch.object(
                        shutil, 'which', return_value=str(git)
                    ):
                        self.assertEqual(bash_executable(), str(bash.resolve()))

    def test_windows_does_not_fall_back_to_wsl(self):
        with patch.object(sys, 'platform', 'win32'), patch.object(
            shutil, 'which', return_value=None
        ):
            with self.assertRaisesRegex(RuntimeError, 'Git Bash was not found'):
                bash_executable()


class PackageVersionTests(unittest.TestCase):
    def test_release_and_snapshot_versions(self):
        action = ROOT / '.github/actions/extract-version/action.yml'
        script = textwrap.dedent(action.read_text().split('      run: |\n', 1)[1])
        bash = bash_executable()
        sha = subprocess.check_output(
            ['git', 'rev-parse', '--short', 'HEAD'], cwd=ROOT, text=True
        ).strip()
        cases = [
            ('main', '', '', 'branch', 'main', 'tilde', '2.0.0', 'false'),
            ('dev', '', '', 'branch', 'dev', 'tilde', f'2.0~dev.g{sha}', 'false'),
            ('main', 'dev', '', 'branch', 'main', 'dash', f'2.0-dev.g{sha}', 'false'),
            ('dev', '', 'v2.0.0', 'branch', 'dev', 'tilde', '2.0.0', 'false'),
            ('dev', 'dev', 'v2.0.0-rc1', 'branch', 'dev', 'tilde', '2.0.0~rc1', 'true'),
            ('v2.0.0-beta2', '', '', 'tag', 'v2.0.0-beta2', 'dash', '2.0.0-beta2', 'true'),
            ('main', 'release/2.0', '', 'branch', 'main', 'tilde', f'2.0~dev.g{sha}', 'false'),
            ('dev', 'main', '', 'branch', 'dev', 'tilde', '2.0.0', 'false'),
        ]
        for branch, selected_branch, tag, ref_type, ref, style, version, prerelease in cases:
            with self.subTest(branch=branch, selected_branch=selected_branch, tag=tag, style=style):
                with tempfile.TemporaryDirectory() as directory:
                    env_file = Path(directory) / 'env'
                    output_file = Path(directory) / 'output'
                    env = dict(os.environ, PACKAGE_PREFIX='qt6-omodscan',
                               PACKAGE_NAME_ENV='PACKAGE_NAME', DEV_SUFFIX_STYLE=style,
                               PRERELEASE_SUFFIX_STYLE='same', RELEASE_BRANCHES_ARE_DEV='true',
                               EVENT_TAG=tag, EVENT_BRANCH=selected_branch, REF_TYPE=ref_type,
                               REF_NAME=ref, GITHUB_REF_NAME=branch,
                               GITHUB_ENV=env_file.as_posix(), GITHUB_OUTPUT=output_file.as_posix())
                    # Keep shell quoting out of the Windows command line and
                    # pass the script unchanged, including its embedded quotes.
                    # Binary input also prevents Windows CRLF translation.
                    result = subprocess.run(
                        [bash, '-eu', '-s'], input=script.encode('utf-8'),
                        cwd=ROOT, env=env, capture_output=True,
                    )
                    self.assertEqual(
                        result.returncode, 0,
                        'Version extraction failed:\n'
                        + result.stdout.decode('utf-8', errors='replace')
                        + '\n' + result.stderr.decode('utf-8', errors='replace'),
                    )
                    values = dict(line.split('=', 1) for line in env_file.read_text().splitlines())
                    self.assertEqual(values['APP_VERSION'], version)
                    self.assertEqual(values['BASE_VERSION'], '2.0.0')
                    self.assertEqual(values['PACKAGE_NAME'], 'qt6-omodscan2')
                    self.assertEqual(values['PRERELEASE_ENABLED'], prerelease)
                    expected_ref = f'refs/tags/{tag or ref}' if tag or ref_type == 'tag' else f'refs/heads/{selected_branch or ref}'
                    self.assertEqual(values['OMODSCAN_SOURCE_REF'], expected_ref)

    def test_cmake_uses_selected_ref(self):
        for source_ref, suffix, expected in [
            ('refs/tags/v2.0.0', '', '2.0.0'),
            ('refs/tags/v2.0.0-rc1', 'rc1', '2.0.0-rc1'),
            ('refs/heads/main', '', '2.0.0'),
        ]:
            with self.subTest(source_ref=source_ref), tempfile.TemporaryDirectory() as directory:
                script = Path(directory) / 'version.cmake'
                script.write_text(f'''set(PROJECT_VERSION 2.0.0)
set(PROJECT_SOURCE_DIR "{(ROOT / 'src').as_posix()}")
include("{(ROOT / 'src/cmake/project_version.cmake').as_posix()}")
set(PRERELEASE_SUFFIX "{suffix}")
omodscan_apply_project_version_suffix()
if(NOT PROJECT_VERSION STREQUAL "{expected}")
    message(FATAL_ERROR "Unexpected version: ${{PROJECT_VERSION}}")
endif()
''')
                env = dict(os.environ, GITHUB_REF='refs/heads/dev', OMODSCAN_SOURCE_REF=source_ref)
                subprocess.run(['cmake', '-P', str(script)], env=env, check=True,
                               capture_output=True, text=True)


if __name__ == '__main__':
    unittest.main()
