import os
from pathlib import Path
import subprocess
import tempfile
import textwrap
import unittest


ROOT = Path(__file__).resolve().parents[2]


class PackageVersionTests(unittest.TestCase):
    def test_release_and_snapshot_versions(self):
        action = ROOT / '.github/actions/extract-version/action.yml'
        script = textwrap.dedent(action.read_text().split('      run: |\n', 1)[1])
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
                        ['bash', '-eu', '-s'], input=script.encode('utf-8'),
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
