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
elif command -v dnf >/dev/null 2>&1; then
    PM="dnf"
    INSTALL_CMD="sudo dnf install -y"
elif command -v pacman >/dev/null 2>&1; then
    PM="pacman"
    INSTALL_CMD="sudo pacman -S --noconfirm"
else
    echo "Error: No supported package manager found (apt, dnf, pacman)."
    exit 1
fi

# ==========================
# Install prerequisites
# ==========================
install_prereqs() {
    echo "Checking prerequisites..."

    if [ "$PM" = "apt-get" ]; then
        for pkg in build-essential cmake ninja-build pkg-config; do
            if ! dpkg -s "$pkg" >/dev/null 2>&1; then
                echo "Installing missing package: $pkg"
                $INSTALL_CMD "$pkg"
            fi
        done

        if apt-cache show qt6-base-dev >/dev/null 2>&1; then
            echo "Qt6 found in repo, checking packages..."
            for pkg in qt6-base-dev qt6-base-dev-tools qt6-tools-dev qt6-tools-dev-tools qt6-serialport-dev qt6-connectivity-dev qt6-core5compat-dev; do
                if ! dpkg -s "$pkg" >/dev/null 2>&1; then
                    echo "Installing missing package: $pkg"
                    $INSTALL_CMD "$pkg"
                fi
            done
        else
            echo "Qt6 not available, falling back to Qt5..."
            for pkg in qtbase5-dev qtbase5-dev-tools qttools5-dev qttools5-dev-tools libqt5serialport5-dev libqt5serialbus5-dev; do
                if ! dpkg -s "$pkg" >/dev/null 2>&1; then
                    echo "Installing missing package: $pkg"
                    $INSTALL_CMD "$pkg"
                fi
            done
        fi

    elif [ "$PM" = "dnf" ]; then
        for pkg in gcc gcc-c++ cmake ninja-build pkg-config; do
            if ! rpm -q "$pkg" >/dev/null 2>&1; then
                echo "Installing missing package: $pkg"
                $INSTALL_CMD "$pkg"
            fi
        done

        if $INSTALL_CMD qt6-qtbase-devel qt6-qttools-devel qt6-qtserialport-devel qt6-qtconnectivity-devel qt6-qt5compat-devel; then
            echo "Qt6 packages installed/verified."
        else
            echo "Falling back to Qt5..."
            $INSTALL_CMD qt5-qtbase-devel qt5-qttools-devel qt5-qtserialport-devel qt5-qtserialbus-devel
        fi

    elif [ "$PM" = "pacman" ]; then
        for pkg in base-devel cmake ninja pkgconf; do
            if ! pacman -Qi "$pkg" >/dev/null 2>&1; then
                echo "Installing missing package: $pkg"
                $INSTALL_CMD "$pkg"
            fi
        done

        if $INSTALL_CMD qt6-base qt6-tools qt6-serialport qt6-connectivity qt6-5compat; then
            echo "Qt6 packages installed/verified."
        else
            echo "Falling back to Qt5..."
            $INSTALL_CMD qt5-base qt5-tools qt5-serialport qt5-serialbus
        fi
    fi
}

# ==========================
# Always check/install prereqs first
# ==========================
install_prereqs

# ==========================
# Detect Qt installation path
# ==========================
get_qt_prefix() {
    for q in qmake6 qmake-qt6 qmake; do
        if command -v "$q" >/dev/null 2>&1; then
            # Сначала пробуем путь к include (он всегда в qt6/)
            prefix=$("$q" -query QT_INSTALL_HEADERS 2>/dev/null)
            if [ -n "$prefix" ] && [ -d "$prefix" ]; then
                echo "$(dirname "$prefix")"
                return
            fi

            # Или путь к libs
            prefix=$("$q" -query QT_INSTALL_LIBS 2>/dev/null)
            if [ -n "$prefix" ] && [ -d "$prefix" ]; then
                echo "$prefix"
                return
            fi
        fi
    done

    if command -v qtpaths6 >/dev/null 2>&1; then
        prefix=$(qtpaths6 --qt-install-headers 2>/dev/null)
        if [ -n "$prefix" ] && [ -d "$prefix" ]; then
            echo "$(dirname "$prefix")"
            return
        fi
    fi

    if [ -d "/usr/lib64/qt6" ]; then echo "/usr/lib64/qt6"; return; fi
    if [ -d "/usr/lib/qt6" ]; then echo "/usr/lib/qt6"; return; fi
    if [ -d "/usr/lib64/qt5" ]; then echo "/usr/lib64/qt5"; return; fi
    if [ -d "/usr/lib/qt5" ]; then echo "/usr/lib/qt5"; return; fi
    echo "/usr"
}


# ==========================
# Get Qt version string
# ==========================
get_qt_version() {
    local qt_type=$1
    if command -v qmake6 >/dev/null 2>&1; then
        qmake6 -query QT_VERSION 2>/dev/null && return
    fi
    if command -v qmake-qt6 >/dev/null 2>&1; then
        qmake-qt6 -query QT_VERSION 2>/dev/null && return
    fi
    if command -v qmake >/dev/null 2>&1; then
        qmake -query QT_VERSION 2>/dev/null && return
    fi
    if command -v qtpaths6 >/dev/null 2>&1; then
        qtpaths6 --version 2>/dev/null | grep -oP 'Qt version \K[0-9.]+' && return
    fi
    if command -v qtpaths >/dev/null 2>&1; then
        qtpaths --version 2>/dev/null | grep -oP 'Qt version \K[0-9.]+' && return
    fi
    if [ "$qt_type" = "qt6" ]; then
        echo "6.0.0"
    else
        echo "5.0.0"
    fi
}

# ==========================
# Detect Qt version and prefix
# ==========================
QT_PREFIX=""
QT_VERSION="Unknown"

if command -v qmake6 >/dev/null 2>&1 || command -v qmake-qt6 >/dev/null 2>&1 || command -v qtpaths6 >/dev/null 2>&1; then
    QT_PREFIX=$(get_qt_prefix)
    QT_VERSION=$(get_qt_version "qt6")
    echo "Using Qt $QT_VERSION from: $QT_PREFIX"
elif command -v qmake >/dev/null 2>&1 || command -v qtpaths >/dev/null 2>&1; then
    QT_PREFIX=$(get_qt_prefix)
    QT_VERSION=$(get_qt_version "qt5")
    echo "Using Qt $QT_VERSION from: $QT_PREFIX"
else
    echo "Error: Qt installation not found even after installing prerequisites"
    exit 1
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
SANITIZED_QT_VERSION=$(echo "$QT_VERSION" | tr '.' '_' | tr ' ' '_')
BUILD_DIR="build-Qt_${SANITIZED_QT_VERSION}_${COMPILER}_${ARCH}-${BUILD_TYPE}"
echo "Starting build in: $BUILD_DIR"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"
cmake ../omodscan -GNinja -DCMAKE_PREFIX_PATH="$QT_PREFIX" -DCMAKE_BUILD_TYPE=${BUILD_TYPE}
ninja
echo "Build finished successfully in $BUILD_DIR."

# ==========================
# Optional: Install
# ==========================
read -p "Do you want to install the application? [y/N] " -n 1 -r
echo
if [[ $REPLY =~ ^[Yy]$ ]]; then
    sudo ninja install
    echo "Application installed successfully."
fi
