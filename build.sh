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
        GENERAL_PACKAGES=(build-essential cmake ninja-build)
        for pkg in "${GENERAL_PACKAGES[@]}"; do
            if ! dpkg -s "$pkg" >/dev/null 2>&1; then
                echo "Installing missing package: $pkg"
                $INSTALL_CMD "$pkg"
            fi
        done

        # pkg-config
        if ! command -v pkg-config >/dev/null 2>&1; then
            echo "Installing missing package: pkg-config"
            $INSTALL_CMD pkg-config
        fi

        # Qt
        if apt-cache show qt6-base-dev >/dev/null 2>&1; then
            QT_PACKAGES=(qt6-base-dev qt6-base-dev-tools qt6-tools-dev qt6-tools-dev-tools qt6-serialport-dev qt6-serialbus-dev qt6-connectivity-dev qt6-core5compat-dev)
            QTVERSION="Qt6"
        else
            QT_PACKAGES=(qtbase5-dev qtbase5-dev-tools qttools5-dev qttools5-dev-tools libqt5serialport5-dev libqt5serialbus5-dev)
            QTVERSION="Qt5"
        fi

        MISSING_QT=()
        for pkg in "${QT_PACKAGES[@]}"; do
            if ! dpkg -s "$pkg" >/dev/null 2>&1; then
                MISSING_QT+=("$pkg")
            fi
        done

        if [ ${#MISSING_QT[@]} -gt 0 ]; then
            echo "Installing missing $QTVERSION packages: ${MISSING_QT[*]}"
            $INSTALL_CMD "${MISSING_QT[@]}"
        else
            echo "$QTVERSION packages already installed."
        fi

    elif [ "$PM" = "dnf" ]; then
        GENERAL_PACKAGES=(gcc gcc-c++ cmake ninja-build)
        for pkg in "${GENERAL_PACKAGES[@]}"; do
            if ! rpm -q "$pkg" >/dev/null 2>&1; then
                echo "Installing missing package: $pkg"
                $INSTALL_CMD "$pkg"
            fi
        done

        # pkg-config
        if ! command -v pkg-config >/dev/null 2>&1; then
            echo "Installing missing package: pkg-config"
            $INSTALL_CMD pkgconf-pkg-config
        fi


        # Qt6
        QT6_PACKAGES=(qt6-qtbase-devel qt6-qttools-devel qt6-qtserialport-devel qt6-qtserialbus-devel qt6-qtconnectivity-devel qt6-qt5compat-devel)
        MISSING_QT6=()
        for pkg in "${QT6_PACKAGES[@]}"; do
            if ! rpm -q "$pkg" >/dev/null 2>&1; then
                MISSING_QT6+=("$pkg")
            fi
        done

        if [ ${#MISSING_QT6[@]} -gt 0 ]; then
            echo "Installing missing Qt6 packages: ${MISSING_QT6[*]}"
            $INSTALL_CMD "${MISSING_QT6[@]}" || true
        else
            echo "Qt6 packages already installed."
        fi

        # Qt5
        if [ ${#MISSING_QT6[@]} -eq ${#QT6_PACKAGES[@]} ]; then
            echo "Falling back to Qt5..."
            QT5_PACKAGES=(qt5-qtbase-devel qt5-qttools-devel qt5-qtserialport-devel qt5-qtserialbus-devel)
            MISSING_QT5=()
            for pkg in "${QT5_PACKAGES[@]}"; do
                if ! rpm -q "$pkg" >/dev/null 2>&1; then
                    MISSING_QT5+=("$pkg")
                fi
            done
            if [ ${#MISSING_QT5[@]} -gt 0 ]; then
                echo "Installing missing Qt5 packages: ${MISSING_QT5[*]}"
                $INSTALL_CMD "${MISSING_QT5[@]}"
            else
                echo "Qt5 packages already installed."
            fi
        fi

    elif [ "$PM" = "pacman" ]; then
        GENERAL_PACKAGES=(base-devel cmake ninja)
        for pkg in "${GENERAL_PACKAGES[@]}"; do
            if ! pacman -Qi "$pkg" >/dev/null 2>&1; then
                echo "Installing missing package: $pkg"
                $INSTALL_CMD "$pkg"
            fi
        done

        # pkg-config
        if ! command -v pkg-config >/dev/null 2>&1; then
            echo "Installing missing package: pkg-config"
            $INSTALL_CMD pkgconf
        fi

        # Qt6
        QT6_PACKAGES=(qt6-base qt6-tools qt6-serialport qt6-serialbus qt6-connectivity qt6-5compat)
        MISSING_QT6=()
        for pkg in "${QT6_PACKAGES[@]}"; do
            if ! pacman -Qi "$pkg" >/dev/null 2>&1; then
                MISSING_QT6+=("$pkg")
            fi
        done

        if [ ${#MISSING_QT6[@]} -gt 0 ]; then
            echo "Installing missing Qt6 packages: ${MISSING_QT6[*]}"
            $INSTALL_CMD "${MISSING_QT6[@]}"
        else
            echo "Qt6 packages already installed."
        fi

        # Qt5
        if [ ${#MISSING_QT6[@]} -eq ${#QT6_PACKAGES[@]} ]; then
            echo "Falling back to Qt5..."
            QT5_PACKAGES=(qt5-base qt5-tools qt5-serialport qt5-serialbus)
            MISSING_QT5=()
            for pkg in "${QT5_PACKAGES[@]}"; do
                if ! pacman -Qi "$pkg" >/dev/null 2>&1; then
                    MISSING_QT5+=("$pkg")
                fi
            done
            if [ ${#MISSING_QT5[@]} -gt 0 ]; then
                echo "Installing missing Qt5 packages: ${MISSING_QT5[*]}"
                $INSTALL_CMD "${MISSING_QT5[@]}"
            else
                echo "Qt5 packages already installed."
            fi
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
            prefix=$("$q" -query QT_INSTALL_HEADERS 2>/dev/null)
            if [ -n "$prefix" ] && [ -d "$prefix" ]; then
                echo "$(dirname "$prefix")"
                return
            fi
            
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
    local QT_VER=$1
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

    echo "Error: $QT_VER installation not found or invalid. Please install $QT_VER development packages." >&2
    exit 1
}

get_cmake_prefix() {
    local QT_VER="$1"   # "qt5" or "qt6"
    local PREFIX=""

    if [ "$QT_VER" = "qt6" ]; then
        if command -v qmake6 >/dev/null 2>&1; then
            PREFIX=$(qmake6 -query QT_INSTALL_PREFIX 2>/dev/null)
        elif command -v qmake-qt6 >/dev/null 2>&1; then
            PREFIX=$(qmake-qt6 -query QT_INSTALL_PREFIX 2>/dev/null)
        elif command -v qtpaths6 >/dev/null 2>&1; then
            PREFIX=$(qtpaths6 --install-prefix 2>/dev/null || qtpaths6 --qt-install-dir 2>/dev/null)
        fi
    else
        if command -v qmake >/dev/null 2>&1; then
            PREFIX=$(qmake -query QT_INSTALL_PREFIX 2>/dev/null)
        elif command -v qtpaths >/dev/null 2>&1; then
            PREFIX=$(qtpaths --install-prefix 2>/dev/null || qtpaths --qt-install-dir 2>/dev/null)
        fi
    fi

    if [ -n "$PREFIX" ]; then
        if [ -f "$PREFIX/lib/cmake/Qt${QT_VER: -1}Core/Qt${QT_VER: -1}CoreConfig.cmake" ] || \
           [ -f "$PREFIX/cmake/Qt${QT_VER: -1}Core/Qt${QT_VER: -1}CoreConfig.cmake" ]; then
            echo "$PREFIX"
            return
        fi
    fi

    # Fedora use /usr/lib64/qt6 or /usr/lib64/qt5
    if [ -f "/usr/lib64/cmake/Qt${QT_VER: -1}Core/Qt${QT_VER: -1}CoreConfig.cmake" ]; then
        echo "/usr/lib64"
        return
    fi
    if [ -f "/usr/lib/cmake/Qt${QT_VER: -1}Core/Qt${QT_VER: -1}CoreConfig.cmake" ]; then
        echo "/usr/lib"
        return
    fi

    echo "Error: $QT_VER installation not found or invalid. Please install $QT_VER development packages." >&2
    exit 1
}


# ==========================
# Detect Qt version and prefix
# ==========================
CMAKE_PREFIX=""
QT_VERSION="Unknown"

if command -v qmake6 >/dev/null 2>&1 || command -v qmake-qt6 >/dev/null 2>&1 || command -v qtpaths6 >/dev/null 2>&1; then
    QT_VERSION=$(get_qt_version "qt6")
    CMAKE_PREFIX=$(get_cmake_prefix "qt6")
    echo "Using Qt $QT_VERSION (Qt6) from: $CMAKE_PREFIX"
elif command -v qmake >/dev/null 2>&1 || command -v qtpaths >/dev/null 2>&1; then
    QT_VERSION=$(get_qt_version "qt5")
    CMAKE_PREFIX=$(get_cmake_prefix "qt5")
    echo "Using Qt $QT_VERSION (Qt5) from: $CMAKE_PREFIX"
else
    echo "Error: Qt installation not found even after installing prerequisites" >&2
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
cmake ../omodscan -GNinja -DCMAKE_PREFIX_PATH="${CMAKE_PREFIX}" -DCMAKE_BUILD_TYPE=${BUILD_TYPE}
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
