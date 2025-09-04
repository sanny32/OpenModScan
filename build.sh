#!/usr/bin/env bash
set -e

echo "=================================="
echo " OpenModScan build script (Linux) "
echo "=================================="

# ==========================
# Detect package manager and distro
# ==========================
if [ -f /etc/os-release ]; then
    . /etc/os-release
else
    echo "Cannot detect Linux distribution"
    exit 1
fi

DISTRO=""
INSTALL_CMD=""

case "$ID" in
    debian|ubuntu|linuxmint|astra)
        DISTRO="debian-based"
        INSTALL_CMD="apt install -y"
        ;;
    rhel|fedora|redos)
        DISTRO="rhel-based"
        INSTALL_CMD="dnf install -y"
        ;;
    altlinux)
        DISTRO="altlinux"
        INSTALL_CMD="apt-get install -y"
        ;;
    suse|opensuse)
        DISTRO="suse-based"
        INSTALL_CMD="zypper install -y"
        ;;
    *)
        echo "Unsupported Linux distribution: $ID"
        exit 1
        ;;
esac

# ==========================
# Check minimum OS version
# ==========================
verlte() {
    # $1 <= $2 ?
    [ "$1" = "$(printf '%s\n%s' "$1" "$2" | sort -V | head -n1)" ]
}
verlt() {
    # $1 < $2 ?
    [ "$1" != "$2" ] && verlte "$1" "$2"
}

check_min_os_version() {
    local MIN_VERSION="$1"
    if [ -z "$VERSION_ID" ]; then
        echo "Warning: Cannot detect OS version, skipping version check."
        return
    fi

    local OS_VER="${VERSION_ID//\"/}"

    if verlt "$OS_VER" "$MIN_VERSION"; then
        echo "Error: $NAME $OS_VER is too old. Minimum required version is $MIN_VERSION." >&2
        exit 1
    fi
}

case "$ID" in
    ubuntu)
        check_min_os_version "24.04"
        ;;
    linuxmint)
        check_min_os_version "22"
        ;;
    debian)
        check_min_os_version "11"
        ;;
    fedora)
        check_min_os_version "42"
        ;;
    rhel)
        check_min_os_version "8"
        ;;
    redos)
        check_min_os_version "8"
        ;;
    astra)
        check_min_os_version "1.7.3"
        ;;
    altlinux)
        check_min_os_version "11.0"
        ;;
    suse|opensuse)
        check_min_os_version "15.0"
        ;;
esac

# ==========================
# Install packages
# ==========================
install_pkg() {
    local pkg_groups=("$@")
    local missing=()
    local check_cmd search_cmd

    case "$DISTRO" in
        debian-based)
            check_cmd="dpkg -s"
            search_cmd="apt-cache show"
            ;;
        rhel-based)
            check_cmd="rpm -q"
            search_cmd="dnf list --available"
            ;;
        altlinux)
            check_cmd="rpm -q"
            search_cmd="apt-cache show"
            ;;
        suse-based)
            check_cmd="rpm -q"
            search_cmd="zypper search -i"
            ;;
    esac

    for group in "${pkg_groups[@]}"; do
        IFS=',' read -ra aliases <<< "$group"
        installed=false
        for pkg in "${aliases[@]}"; do
            printf "Checking for %-30s... " "$pkg"
            if $check_cmd "$pkg" >/dev/null 2>&1; then
                installed=true
                echo "yes"
                break
            else
                echo "no"
            fi
        done

        if [ "$installed" = false ]; then
            for pkg in "${aliases[@]}"; do
                if $search_cmd "$pkg" >/dev/null 2>&1; then
                    missing+=("$pkg")
                    break
                fi
            done
        fi
    done

    if [ ${#missing[@]} -gt 0 ]; then
        echo "Installing missing packages: ${missing[*]}"
        if [ "$EUID" -eq 0 ]; then
            $INSTALL_CMD "${missing[@]}"
        else
            if command -v sudo >/dev/null 2>&1; then
               trap 'echo "Installation cancelled by user."; exit 1' INT
                if sudo -n true 2>/dev/null; then
                    sudo $INSTALL_CMD "${missing[@]}"
                else
                    if sudo -l >/dev/null 2>&1; then
                        sudo $INSTALL_CMD "${missing[@]}"
                    else
                        echo "Using su (user not in sudoers)..."
                        su -c "$INSTALL_CMD ${missing[*]}"
                    fi
                fi
                trap - INT
            else
                echo "Using su (sudo not installed)..."
                su -c "$INSTALL_CMD ${missing[*]}"
            fi
        fi
    fi
}

# ==========================
# Install prerequisites
# ==========================
install_prereqs() {
    echo "Checking prerequisites for $ID..."

    case "$DISTRO" in
        debian-based)
            GENERAL_PACKAGES=(build-essential cmake ninja-build libxcb-cursor-dev pkg-config)
            
            # Qt6/Qt5 selection
            if apt-cache show qt6-base-dev >/dev/null 2>&1; then
                QT_PACKAGES=(
                    qt6-base-dev 
                    qt6-base-dev-tools 
                    qt6-tools-dev 
                    qt6-tools-dev-tools 
                    qt6-serialport-dev,libqt6serialport6-dev
                    qt6-serialbus-dev,libqt6serialbus6-dev
                    qt6-5compat-dev,libqt6core5compat6-dev
                )
            else
                QT_PACKAGES=(
                    qtbase5-dev 
                    qtbase5-dev-tools 
                    qttools5-dev 
                    qttools5-dev-tools 
                    libqt5serialport5-dev 
                    libqt5serialbus5-dev
                )
            fi
            ;;

        rhel-based)
            GENERAL_PACKAGES=(gcc gcc-c++ cmake ninja-build pkgconf-pkg-config xcb-util-cursor-devel)         
     
            if dnf list available qt6-qtbase-devel >/dev/null 2>&1; then
                QT_PACKAGES=(
                    qt6-qtbase-devel 
                    qt6-qttools-devel 
                    qt6-qtserialport-devel 
                    qt6-qtserialbus-devel 
                    qt6-qt5compat-devel
                )
            else
                QT_PACKAGES=(
                    qt5-qtbase-devel 
                    qt5-qttools-devel 
                    qt5-qtserialport-devel 
                    qt5-qtserialbus-devel
                )
            fi
            ;;

        altlinux)
            GENERAL_PACKAGES=(gcc gcc-c++ cmake ninja-build pkg-config libxcbutil-cursor)         
    
            # Qt6/Qt5 selection
            if apt-cache show qt6-base-devel >/dev/null 2>&1; then
                QT_PACKAGES=(
                    qt6-base-devel 
                    qt6-tools-devel 
                    qt6-serialport-devel 
                    qt6-serialbus-devel 
                    qt6-5compat-devel
                )
            else
                QT_PACKAGES=(
                    qt5-base-devel 
                    qt5-tools-devel 
                    qt5-serialport-devel 
                    qt5-serialbus-devel
                )
            fi
            ;;
        
        suse-based)
            GENERAL_PACKAGES=(gcc gcc-c++ cmake ninja pkg-config libxcb-cursor0)
    
            # Qt6/Qt5 selection
            if zypper search -i qt6-base-devel >/dev/null 2>&1; then
                QT_PACKAGES=(
                    qt6-base-devel
                    qt6-tools-devel
                    qt6-serialport-devel
                    qt6-serialbus-devel
                    qt6-core5compat-devel
                )
            else
                QT_PACKAGES=(
                    libqt5-qtbase-devel
                    libqt5-qttools-devel
                    libqt5-qtserialport-devel
                    libqt5-qtserialbus-devel
                )
            fi
           ;;
    esac

    install_pkg "${GENERAL_PACKAGES[@]}" "${QT_PACKAGES[@]}"
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

    echo "Error: Can't detect installed Qt version." >&2
    exit 1
}

get_cmake_prefix() {
    local QT_VER="$1"
    local CONFIG_FILE="Qt${QT_VER: -1}CoreConfig.cmake"
    local PREFIX=""

     if command -v pkg-config >/dev/null 2>&1; then
        PREFIX=$(pkg-config --variable=prefix Qt${QT_VER: -1}Core 2>/dev/null || true)
        if [ -n "$PREFIX" ]; then
            echo "$PREFIX"
            return
        fi
    fi

     for q in qmake6 qmake-qt6 qmake qtpaths6 qtpaths; do
        if command -v "$q" >/dev/null 2>&1; then
            PREFIX=$("$q" -query QT_INSTALL_PREFIX 2>/dev/null || "$q" --install-prefix 2>/dev/null || true)
            if [ -n "$PREFIX" ] && [ -f "$PREFIX/lib/cmake/$CONFIG_FILE" ]; then
                echo "$PREFIX"
                return
            fi
        fi
    done

    PREFIX=$(find /usr /usr/local -type f -name "$CONFIG_FILE" 2>/dev/null | head -n1)
    if [ -n "$PREFIX" ]; then
        echo "$(dirname "$PREFIX")"
        return
    fi

    echo "Error: Can't detect cmake prefix path." >&2
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
# Check minimal Qt version
# ==========================
MIN_QT_VERSION="5.15.0"
if verlt "$QT_VERSION" "$MIN_QT_VERSION"; then
    echo "Error: Qt >= $MIN_QT_VERSION is required, but found $QT_VERSION" >&2
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
    COMPILER="g++"
elif command -v clang++ >/dev/null 2>&1; then
    COMPILER="clang++"
fi

# ==========================
# Build type
# ==========================
BUILD_TYPE=Release

# ==========================
# Build project
# ==========================
SANITIZED_QT_VERSION=$(echo "$QT_VERSION" | tr '.' '_' | tr ' ' '_')
BUILD_DIR="build-omodscan-Qt_${SANITIZED_QT_VERSION}_${COMPILER}_${ARCH}-${BUILD_TYPE}"
echo "Starting build in: $BUILD_DIR"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"
cmake ../omodscan -GNinja -DCMAKE_PREFIX_PATH="${CMAKE_PREFIX}" -DCMAKE_BUILD_TYPE=${BUILD_TYPE}
ninja
echo "Build finished successfully in $BUILD_DIR."
echo ""
if ninja --version | awk '{if ($1 >= 1.10) exit 0; else exit 1}'; then
    echo "To install Open ModScan, run:"
    echo ""
    if command -v sudo >/dev/null 2>&1 && sudo -n true 2>/dev/null; then
        echo -e "    cd $BUILD_DIR && sudo ninja install"
    else
        echo -e "    cd $BUILD_DIR && su -c 'ninja install'"
    fi
    echo ""
fi
