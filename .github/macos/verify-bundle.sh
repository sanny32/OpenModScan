#!/bin/bash
set -euo pipefail

bundle=$1
expected_version=$2
architecture=${3:-arm64}
plist="$bundle/Contents/Info.plist"
executable=$(/usr/libexec/PlistBuddy -c 'Print :CFBundleExecutable' "$plist")
icon=$(/usr/libexec/PlistBuddy -c 'Print :CFBundleIconFile' "$plist")
binary="$bundle/Contents/MacOS/$executable"

test -x "$binary"
test -f "$bundle/Contents/Resources/$icon.icns"
test "$(/usr/libexec/PlistBuddy -c 'Print :CFBundleIconName' "$plist")" = "$icon"
test -f "$bundle/Contents/Resources/Assets.car"
test -d "$bundle/Contents/Frameworks/QtCore.framework"
test -f "$bundle/Contents/PlugIns/platforms/libqcocoa.dylib"
test -f "$bundle/Contents/PlugIns/platforms/libqoffscreen.dylib"
test -f "$bundle/Contents/Resources/translations/qt_ru.qm"
test -f "$bundle/Contents/Resources/translations/qtbase_ru.qm"
/usr/bin/plutil -lint "$plist"
/usr/bin/lipo -verify_arch "$architecture" "$binary"
/usr/bin/codesign --verify --deep --strict "$bundle"

find "$bundle/Contents" -type f -print0 | while IFS= read -r -d '' path; do
    if /usr/bin/file -b "$path" | grep -q 'Mach-O'; then
        dependencies=$(/usr/bin/otool -L "$path" | awk '/^[[:space:]]/ {print $1}')
        while IFS= read -r dependency; do
            case "$dependency" in
                /System/*|/usr/lib/*|@*) ;;
                /*) echo "External dependency in $path: $dependency" >&2; exit 1 ;;
            esac
        done <<< "$dependencies"
    fi
done

actual_version=$(env -u DYLD_LIBRARY_PATH -u DYLD_FRAMEWORK_PATH \
    QT_QPA_PLATFORM=offscreen "$binary" --version)
if [ "$actual_version" != "$expected_version" ]; then
    echo "Expected version $expected_version, got $actual_version" >&2
    exit 1
fi
