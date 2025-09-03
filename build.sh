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
    INSTALL_CMD="sudo apt-get install -y"
    UPDATE_CMD="sudo apt-get update"
elif command -v dnf >/dev/null 2>&1; then
    PM="dnf"
    INSTALL_CMD="sudo dnf install -y"
    UPDATE_CMD="sudo dnf update -y"
elif command -v pacman >/dev/null 2>&1; then
    PM="pacman"
    INSTALL_CMD="sudo pacman -S --noconfirm"
    UPDATE_CMD="sudo pacman -Sy --noconfirm"
else
    echo "Error: No supported package manager found (apt, dnf, pacman)."
    exit 1
fi

# ==========================
# Install prerequisites
# ==========================
install_prereqs() {
    echo "Installing prerequisites..."

    # Update package lists
    $UPDATE_CMD

    # Install basic build tools
    $INSTALL_CMD build-essential cmake ninja-build pkg-config

    if [ "$PM" = "apt-get" ]; then
        if apt-cache show qt6-base-dev >/dev/null 2>&1; then
            echo "Qt6 found in repo, installing..."
            $INSTALL_CMD \
                qt6-base-dev \
                qt6-base-dev-tools \
                qt6-tools-dev \
                qt6-tools-dev-tools \
                qt6-serialport-dev \
                qt6-connectivity-dev \
                qt6-core5compat-dev
        else
            echo "Qt6 not available, falling back to Qt5..."
            $INSTALL_CMD \
                qtbase5-dev \
                qtbase5-dev-tools \
                qttools5-dev \
                qttools5-dev-tools \
                libqt5serialport5-dev \
                libqt5serialbus5-dev
        fi

    elif [ "$PM" = "dnf" ]; then
        $INSTALL_CMD gcc gcc-c++ cmake ninja-build pkg-config
        $INSTALL_CMD qt6-qtbase-devel qt6-qttools-devel qt6-qtserialport-devel qt6-qtconnectivity-devel qt6-qt5compat-devel || \
            $INSTALL_CMD qt5-qtbase-devel qt5-qttools-devel qt5-qtserialport-devel qt5-qtserialbus-devel

    elif [ "$PM" = "pacman" ]; then
        $INSTALL_CMD base-devel cmake ninja pkgconf
        $INSTALL_CMD qt6-base qt6-tools qt6-serialport qt6-connectivity qt6-5compat || \
            $INSTALL_CMD qt5-base qt5-tools qt5-serialport qt5-serialbus
    fi
}

# ==========================
# Check and install required tools
# ==========================
check_and_install() {
    local tool=$1
    if ! command -v "$tool" >/dev/null 2>&1; then
        echo "$tool not found. Installing..."
        if [ "$PM" = "apt-get" ]; then
            sudo apt-get install -y "$tool"
        elif [ "$PM" = "dnf" ]; then
            sudo dnf install -y "$tool"
        elif [ "$PM" = "pacman" ]; then
            sudo pacman -S --noconfirm "$tool"
        fi
    fi
}

# Проверяем и устанавливаем необходимые инструменты
check_and_install cmake
check_and_install ninja
check_and_install g++
check_and_install gcc

# ==========================
# Detect Qt installation path
# ==========================
get_qt_prefix() {
    # Try multiple methods to get Qt installation prefix
    
    # Method 1: Use qmake (most reliable)
    if command -v qmake6 >/dev/null 2>&1; then
        qmake6 -query QT_INSTALL_PREFIX 2>/dev/null && return
    fi
    
    if command -v qmake-qt6 >/dev/null 2>&1; then
        qmake-qt6 -query QT_INSTALL_PREFIX 2>/dev/null && return
    fi
    
    if command -v qmake >/dev/null 2>&1; then
        qmake -query QT_INSTALL_PREFIX 2>/dev/null && return
    fi
    
    # Method 2: Use qtpaths with different options
    if command -v qtpaths6 >/dev/null 2>&1; then
        # Try different options that might work
        qtpaths6 --install-prefix 2>/dev/null && return
        qtpaths6 --qt-install-dir 2>/dev/null && return
    fi
    
    # Method 3: Default paths
    if [ -d "/usr/lib64/qt6" ]; then
        echo "/usr/lib64/qt6"
        return
    fi
    
    if [ -d "/usr/lib/qt6" ]; then
        echo "/usr/lib/qt6"
        return
    fi
    
    if [ -d "/usr/lib64/qt5" ]; then
        echo "/usr/lib64/qt5"
        return
    fi
    
    if [ -d "/usr/lib/qt5" ]; then
        echo "/usr/lib/qt5"
        return
    fi
    
    # Fallback to /usr
    echo "/usr"
}

# ==========================
# Detect Qt version
# ==========================
QT_PREFIX=""
QT_VERSION="Unknown"

# First try to detect Qt6
if command -v qmake6 >/dev/null 2>&1 || command -v qmake-qt6 >/dev/null 2>&1 || command -v qtpaths6 >/dev/null 2>&1; then
    QT_PREFIX=$(get_qt_prefix)
    QT_VERSION="qt6"
    echo "Using Qt6 from: $QT_PREFIX"
# Then try Qt5
elif command -v qmake >/dev/null 2>&1 || command -v qtpaths >/dev/null 2>&1; then
    QT_PREFIX=$(get_qt_prefix)
    QT_VERSION="qt5"
    echo "Using Qt5 from: $QT_PREFIX"
else
    echo "Qt not found. Installing prerequisites..."
    install_prereqs
    # Try again after installation
    if command -v qmake6 >/dev/null 2>&1 || command -v qmake-qt6 >/dev/null 2>&1; then
        QT_PREFIX=$(get_qt_prefix)
        QT_VERSION="qt6"
    elif command -v qmake >/dev/null 2>&1; then
        QT_PREFIX=$(get_qt_prefix)
        QT_VERSION="qt5"
    else
        echo "Error: Qt installation not found even after installing prerequisites"
        exit 1
    fi
fi

# ==========================
# Detect architecture
# ==========================
ARCH=$(uname -m)

# ==========================
# Detect compiler
# ==========================
COMPILER="Unknown"
if command -v g++ >/dev/null 2>&1; then
    COMPILER="GCC"
elif command -v clang++ >/dev/null 2>&1; then
    COMPILER="Clang"
fi

# ==========================
# Build type
# ==========================
BUILD_TYPE=Release

# ==========================
# Build project
# ==========================
BUILD_DIR="build-Qt_${QT_VERSION}_${COMPILER}_${ARCH}-${BUILD_TYPE}"
echo "Starting build in: $BUILD_DIR"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"
cmake ../omodscan -GNinja -DCMAKE_PREFIX_PATH="$QT_PREFIX" -DCMAKE_BUILD_TYPE=${BUILD_TYPE}
ninja
echo "Build finished successfully in $BUILD_DIR."