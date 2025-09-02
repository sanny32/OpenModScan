#!/usr/bin/env bash
set -e

echo "=================================="
echo " OpenModScan build script (Linux) "
echo "=================================="

# ==========================
# Detect package manager
# ==========================
if command -v apt-get >/dev/null 2>&1; then
    PM="apt-get"
elif command -v dnf >/dev/null 2>&1; then
    PM="dnf"
elif command -v pacman >/dev/null 2>&1; then
    PM="pacman"
else
    echo "Error: No supported package manager found (apt, dnf, pacman)."
    exit 1
fi

# ==========================
# Install prerequisites
# ==========================
install_prereqs() {
    echo "Installing prerequisites..."

    if [ "$PM" = "apt-get" ]; then
        sudo apt-get update
        sudo apt-get install -y build-essential cmake ninja-build pkg-config

        if apt-cache show qt6-base-dev >/dev/null 2>&1; then
            echo "Qt6 found in repo, installing..."
            sudo apt-get install -y \
                qt6-base-dev \
                qt6-base-dev-tools \
                qt6-tools-dev \
                qt6-tools-dev-tools \
                qt6-serialport-dev \
                qt6-connectivity-dev \
                qt6-core5compat-dev
        else
            echo "Qt6 not available, falling back to Qt5..."
            sudo apt-get install -y \
                qtbase5-dev \
                qtbase5-dev-tools \
                qttools5-dev \
                qttools5-dev-tools \
                libqt5serialport5-dev \
                libqt5serialbus5-dev
        fi

    elif [ "$PM" = "dnf" ]; then
        sudo dnf install -y gcc gcc-c++ cmake ninja-build pkg-config
        sudo dnf install -y qt6-qtbase-devel qt6-qttools-devel qt6-qtserialport-devel qt6-qtconnectivity-devel qt6-qt5compat-devel || \
            sudo dnf install -y qt5-qtbase-devel qt5-qttools-devel qt5-qtserialport-devel qt5-qtserialbus-devel

    elif [ "$PM" = "pacman" ]; then
        sudo pacman -Sy --noconfirm base-devel cmake ninja pkgconf
        sudo pacman -S --noconfirm qt6-base qt6-tools qt6-serialport qt6-connectivity qt6-5compat || \
            sudo pacman -S --noconfirm qt5-base qt5-tools qt5-serialport qt5-serialbus
    fi
}

# ==========================
# Detect Qt
# ==========================
QT_PREFIX=""
QT_VERSION="unknown"

if command -v qtpaths6 >/dev/null 2>&1; then
    QT_PREFIX=$(qtpaths6 --qt-install-prefix)
    QT_VERSION="qt6"
    echo "Using Qt6 from: $QT_PREFIX"
elif command -v qtpaths-qt5 >/dev/null 2>&1; then
    QT_PREFIX=$(qtpaths-qt5 --qt-install-prefix)
    QT_VERSION="qt5"
    echo "Using Qt5 from: $QT_PREFIX"
elif command -v qmake >/dev/null 2>&1; then
    if qmake -query QT_INSTALL_PREFIX >/dev/null 2>&1; then
        QT_PREFIX=$(qmake -query QT_INSTALL_PREFIX)
        QT_VERSION=$(qmake -query QT_VERSION | cut -d. -f1-2 | sed 's/\./_/')
        echo "Using Qt ($QT_VERSION) from qmake: $QT_PREFIX"
    else
        echo "qmake is broken. Installing prerequisites..."
        install_prereqs
    fi
else
    echo "Qt not found. Installing prerequisites..."
    install_prereqs
fi

# ==========================
# Detect architecture
# ==========================
ARCH=$(uname -m)

# ==========================
# Detect compiler
# ==========================
COMPILER="unknown"
if command -v g++ >/dev/null 2>&1; then
    COMPILER="GCC"
elif command -v clang++ >/dev/null 2>&1; then
    COMPILER="Clang"
fi

# ==========================
# Build project
# ==========================
BUILD_DIR="build-Qt_${QT_VERSION}_${COMPILER}_${ARCH}-Release"
echo "Starting build in: $BUILD_DIR"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"
cmake ../omodscan -GNinja -DCMAKE_PREFIX_PATH="$QT_PREFIX" -DCMAKE_BUILD_TYPE=Release
ninja
echo "Build finished successfully in $BUILD_DIR."
