#!/bin/bash

verlt() {
    # $1 < $2 ?
    [ "$1" != "$2" ] && \
    [ "$1" = "$(printf '%s\n%s' "$1" "$2" | sort -V | head -n1)" ]
}

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"

# Check if flatpak is installed
if ! command -v flatpak &> /dev/null; then
    echo -e "\033[31mError: flatpak is not installed. Please install it and try again.\033[0m"
    exit 1
fi

# Add flathub remote if not exists
flatpak --user remote-add --if-not-exists flathub https://flathub.org/repo/flathub.flatpakrepo

# Check if flatpak-builder is installed
if ! command -v flatpak-builder &> /dev/null; then
    echo -e "\033[31mError: flatpak-builder is not installed. Please install it and try again.\033[0m"
    exit 1
fi

FB_APP="flatpak-builder"
FB_VERSION=$($FB_APP --version 2>/dev/null | grep -o '[0-9]\+\.[0-9]\+' | head -1)
if verlt "$FB_VERSION" "1.4"; then
    echo -e "\033[33mWarning: flatpak-builder version must be 1.4 or higher.\033[0m"

    if flatpak info --user org.flatpak.Builder > /dev/null 2>&1; then
        flatpak --user update -y org.flatpak.Builder || exit 1
    else
        flatpak --user install -y flathub org.flatpak.Builder || exit 1
    fi
    FB_APP="flatpak run org.flatpak.Builder"
fi

cd .github/linux/flatpak || exit 1

# Install required runtimes and tools
if flatpak info --user org.kde.Sdk//6.9 > /dev/null 2>&1; then
    flatpak --user update -y org.kde.Sdk//6.9 || exit 1
else
    flatpak --user install -y flathub org.kde.Sdk//6.9 || exit 1
fi

echo ""

# Detect Git branch name 
if [ -z "${REF_NAME:-}" ]; then
    REF_NAME="$(git rev-parse --abbrev-ref HEAD)"
    if [ -z "$REF_NAME" ]; then
       echo -e "\033[33mWarning: failed to detect Git branch name. Using 'main' as fallback.\033[0m"
       REF_NAME="main"
    else
       echo "Detected Git branch: $REF_NAME"
    fi   
fi

# Create desktop file
sed -e "s|@APP_EXEC@|/app/bin/omodscan|g" \
    -e "s|@APP_ICON@|io.github.sanny32.omodscan|g" \
    ../usr/share/applications/omodscan.desktop.in > ../usr/share/applications/omodscan.desktop
    
# Create mainfo file
sed -e "s|@REF_NAME@|$REF_NAME|g" \
    -e "s|@COMPONENT_ID@|io.github.sanny32.omodscan|g" \
    -e "s|@APP_ICON@|io.github.sanny32.omodscan|g" \
    ../usr/share/metainfo/omodscan.metainfo.xml.in > ../usr/share/metainfo/omodscan.metainfo.xml

# Build project
echo -e "\033[32mBuilding flatpak project...\033[0m"
sed -i "s|branch: main|branch: ${REF_NAME}|" io.github.sanny32.omodscan.yaml

$FB_APP --repo=repo --force-clean --verbose build io.github.sanny32.omodscan.yaml
echo -e "Done\n"

# Create flatpak bundle
echo -e "\033[32mCreating flatpak bundle...\033[0m"
flatpak build-bundle repo io.github.sanny32.omodscan.flatpak io.github.sanny32.omodscan stable || exit 1
echo -e "Done\n"

# Cleanup
echo -e "\033[32mCleanup...\033[0m"
rm -rf build repo .flatpak-builder
rm -rf ../usr/share/applications/omodscan.desktop
rm -rf ../usr/share/metainfo/omodscan.metainfo.xml
echo "Done"

# Move bundle to script directory
mv io.github.sanny32.omodscan.flatpak "$SCRIPT_DIR"/ || exit 1
echo -e "\nTo install Open ModScan, run:"
echo -e "    flatpak install --user io.github.sanny32.omodscan.flatpak\n"