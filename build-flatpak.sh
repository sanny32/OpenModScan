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

# Build project
echo -e "\033[32mBuilding flatpak project...\033[0m"
if [ -z "${BRANCH_NAME:-}" ]; then
    BRANCH_NAME="$(git rev-parse --abbrev-ref HEAD)"
fi
sed -i "s|branch: main|branch: ${BRANCH_NAME}|" io.github.sanny32.omodscan.yaml

$FB_APP --repo=repo --force-clean --verbose build io.github.sanny32.omodscan.yaml
echo -e "Done\n"

# Create flatpak bundle
echo -e "\033[32mCreating flatpak bundle...\033[0m"
flatpak build-bundle repo io.github.sanny32.omodscan.flatpak io.github.sanny32.omodscan stable
echo -e "Done\n"

# Cleanup
echo -e "\033[32mCleanup...\033[0m"
rm -rf build repo .flatpak-builder
echo "Done"

# Move bundle to script directory
mv io.github.sanny32.omodscan.flatpak "$SCRIPT_DIR"/ || exit 1
echo "To install Open ModScan, run:"
echo -e "    flatpak install --user io.github.sanny32.omodscan.flatpak"
