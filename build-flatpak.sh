#!/bin/bash

# Check if flatpak is installed
if ! command -v flatpak &> /dev/null; then
    echo "Error: flatpak is not installed. Please install it and try again."
    exit 1
fi

# Check if flatpak-builder is installed
if ! command -v flatpak-builder &> /dev/null; then
    echo "Error: flatpak-builder is not installed. Please install it and try again."
    exit 1
fi

cd .flatpak || exit 1

# Add flathub remote if not exists
flatpak --user remote-add --if-not-exists flathub https://flathub.org/repo/flathub.flatpakrepo

# Install required runtimes and tools
if flatpak list --user | grep -q "org.kde.Sdk.*6.9"; then
    flatpak --user update -y org.kde.Sdk//6.9
else
    flatpak --user install -y flathub org.kde.Sdk//6.9
fi

# Build project
flatpak-builder --repo=repo --force-clean --verbose build io.github.sanny32.omodscan.yaml

# Create flatpak bundle
flatpak build-bundle repo io.github.sanny32.omodscan.flatpak io.github.sanny32.omodscan stable

# Cleanup
rm -rf build repo .flatpak-builder
