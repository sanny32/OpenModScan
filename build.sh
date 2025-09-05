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
SEARCH_CMD=""

case "$ID" in
    debian|ubuntu|linuxmint|astra)
        DISTRO="debian-based"
        INSTALL_CMD="apt install -y"
        SEARCH_CMD="apt-cache show"
        ;;
    rhel|fedora|redos)
        DISTRO="rhel-based"
        INSTALL_CMD="dnf install -y"
        SEARCH_CMD="dnf list --available"
        ;;
    altlinux)
        DISTRO="altlinux"
        INSTALL_CMD="apt-get install -y"
        SEARCH_CMD="apt-cache show"
        ;;
    suse|opensuse*)
        DISTRO="suse-based"
        INSTALL_CMD="zypper install -y"
        SEARCH_CMD="zypper search"
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
        echo "Error: Cannot detect OS version."
        exit 1
    fi

    local OS_VER="${VERSION_ID//\"/}"

    if verlt "$OS_VER" "$MIN_VERSION"; then
        echo "Error: $NAME $OS_VER is too old. Minimum required version is $MIN_VERSION." >&2
        exit 1
    fi
}

case "$ID" in
    ubuntu)
        check_min_os_version "22"
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
    suse|opensuse*)
        check_min_os_version "15.5"
        ;;
esac

# ==========================
# Parse script arguments
# ==========================
QT_CHOICE="auto"
for arg in "$@"; do
    case "$arg" in
        -qt5)
            QT_CHOICE="qt5"
            shift
            ;;
        -qt6)
            QT_CHOICE="qt6"
            shift
            ;;
        *)
            ;;
    esac
done

if [ "$QT_CHOICE" = "auto" ]; then
    if $SEARCH_CMD qt6-* 2>/dev/null | grep -q "qt6"; then
        QT_CHOICE="qt6"
        echo "Auto-detected: Qt6 available, using Qt6"
    else
        QT_CHOICE="qt5"
        echo "Auto-detected: Qt6 not available, using Qt5"
    fi
fi

# ==========================
# Can use sudo?
# ==========================
can_sudo() {
    command -v sudo >/dev/null 2>&1 && \
    ! (sudo -n -l 2>&1 | grep -q "not in the sudoers file")
}

# ==========================
# Install packages
# ==========================
install_pkg() {
    local pkg_groups=("$@")
    local missing=()
    local check_cmd

    case "$DISTRO" in
        debian-based)
            check_cmd="dpkg -s"
            ;;
        rhel-based)
            check_cmd="rpm -q"
            ;;
        altlinux)
            check_cmd="rpm -q"
            ;;
        suse-based)
            check_cmd="rpm -q"
            ;;
    esac

    for group in "${pkg_groups[@]}"; do
        printf "Checking for %-30s... " "$group"
        if $check_cmd "$group" >/dev/null 2>&1; then
            echo "yes"
        else
            echo "no"
            missing+=("$group")
        fi
    done

    if [ ${#missing[@]} -gt 0 ]; then
        echo "Installing missing packages: ${missing[*]}"
        if [ "$EUID" -eq 0 ]; then
            $INSTALL_CMD "${missing[@]}"
        else
            if command -v sudo >/dev/null 2>&1; then
               trap 'echo "Installation cancelled by user."; exit 1' INT
                if can_sudo; then
                    sudo $INSTALL_CMD "${missing[@]}"
                else
                    echo "Using su (user not in sudoers)..."
                    su -c "$INSTALL_CMD ${missing[*]}"
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
# Function to get packages for a distro
# Returns combined general + Qt package list
# ==========================
get_packages_for_distro() {
    local distro="$1"
    local version="$2"
    local qt_choice="$3"

    local general_packages=""
    local qt_packages=""

    case "$distro" in
        debian)
            general_packages="build-essential cmake ninja-build libxcb-cursor-dev pkg-config"
            ;;
        rhel)
            general_packages="gcc gcc-c++ cmake ninja-build pkgconf-pkg-config xcb-util-cursor-devel"
            ;;
        altlinux)
            general_packages="gcc gcc-c++ cmake ninja-build pkg-config libxcbutil-cursor"
            ;;
        suse)
            general_packages="gcc gcc-c++ cmake ninja pkg-config libxcb-cursor0"
            ;;
    esac

    case "$qt_choice" in
        qt6)
            case "$distro" in
                debian)
                    if [ "$ID" = "ubuntu" ] && [ "${version%%.*}" = "22" ]; then
                        qt_packages="qt6-base-dev qt6-base-dev-tools qt6-tools-dev qt6-tools-dev-tools libqt6serialport6-dev libqt6serialbus6-bin libqt6serialbus6-dev libqt6core5compat6-dev"
                    else
                        qt_packages="qt6-base-dev qt6-base-dev-tools qt6-tools-dev qt6-tools-dev-tools qt6-serialport-dev qt6-serialbus-dev qt6-5compat-dev"
                    fi
                    ;;
                rhel)
                    qt_packages="qt6-qtbase-devel qt6-qttools-devel qt6-qtserialport-devel qt6-qtserialbus-devel qt6-qt5compat-devel"
                    ;;
                altlinux)
                    qt_packages="qt6-base-devel qt6-tools-devel qt6-serialport-devel qt6-serialbus-devel qt6-5compat-devel"
                    ;;
                suse)
                    qt_packages="qt6-base-devel qt6-tools-devel qt6-serialport-devel qt6-serialbus-devel qt6-qt5compat-devel qt6-linguist-devel"
                    ;;
            esac
            ;;
        qt5)
            case "$distro" in
                debian)
                    qt_packages="qtbase5-dev qtbase5-dev-tools qttools5-dev qttools5-dev-tools libqt5serialport5-dev libqt5serialbus5-dev"
                    ;;
                rhel)
                    qt_packages="qt5-qtbase-devel qt5-qttools-devel qt5-qtserialport-devel qt5-qtserialbus-devel"
                    ;;
                altlinux)
                    qt_packages="qt5-base-devel qt5-tools-devel qt5-serialport-devel qt5-serialbus-devel"
                    ;;
                suse)
                    qt_packages="libqt5-qtbase-devel libqt5-qttools-devel libqt5-qtserialport-devel libqt5-qtserialbus libqt5-qtserialbus-devel"
                    ;;
            esac
            ;;
    esac

    echo "$general_packages $qt_packages"
}

# ==========================
# Install prerequisites
# ==========================
install_prereqs() {
    local pkgs
    pkgs=$(get_packages_for_distro "$DISTRO" "$VERSION_ID" "$QT_CHOICE") || return 1
    install_pkg $pkgs
}

# ==========================
# Always check/install prereqs first
# ==========================
echo "Checking prerequisites for $ID..."
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
    local REQ="$1"
    local ver=""
    local probes=()

    if [ "$REQ" = "qt6" ]; then
        probes=(qmake qmake6 qmake-qt6 qtpaths6)
    elif [ "$REQ" = "qt5" ]; then
        probes=(qmake qmake-qt5 qt5-qmake qtpaths-qt5)
    fi

    for p in "${probes[@]}"; do
        if command -v "$p" >/dev/null 2>&1; then
            case "$p" in
                qtpaths* )
                    ver=$("$p" --version 2>/dev/null | grep -oE '[0-9]+(\.[0-9]+)+' || true)
                    ;;
                *)
                    ver=$("$p" -query QT_VERSION 2>/dev/null || true)
                    ;;
            esac

            if [ -n "$ver" ]; then
                echo "$ver"
                return 0
            fi
        fi
    done

    echo "Error: Can't detect installed Qt version." >&2
    exit 1
}

# ==========================
# Get cmake prefix for Qt
# ==========================
get_cmake_prefix() {
    local REQ="$1"
    local config_file=""
    local prefix=""

    if [ "$REQ" = "qt6" ]; then
        config_file="Qt6CoreConfig.cmake"
    elif [ "$REQ" = "qt5" ]; then
        config_file="Qt5CoreConfig.cmake"
    fi

    if command -v pkg-config >/dev/null 2>&1; then
        case "$REQ" in
            qt6) prefix=$(pkg-config --variable=prefix Qt6Core 2>/dev/null || true) ;;
            qt5) prefix=$(pkg-config --variable=prefix Qt5Core 2>/dev/null || true) ;;
        esac
        if [ -n "$prefix" ]; then
            echo "$prefix"
            return 0
        fi
    fi

    local probes=()
    if [ "$REQ" = "qt6" ]; then
        probes=(qmake6 qmake-qt6 qtpaths6)
    elif [ "$REQ" = "qt5" ]; then
        probes=(qmake-qt5 qt5-qmake qtpaths-qt5)
    fi

    for q in "${probes[@]}"; do
        if command -v "$q" >/dev/null 2>&1; then
            case "$q" in
                qtpaths* )
                    prefix=$("$q" --install-prefix 2>/dev/null || true)
                    ;;
                *)
                    prefix=$("$q" -query QT_INSTALL_PREFIX 2>/dev/null || true)
                    ;;
            esac
            if [ -n "$prefix" ]; then
                if [ -n "$config_file" ]; then
                    if [ -f "$prefix/lib/cmake/$config_file" ] || [ -f "$prefix/lib64/cmake/$config_file" ]; then
                        echo "$prefix"
                        return 0
                    fi
                else
                    echo "$prefix"
                    return 0
                fi
            fi
        fi
    done

    local found=""
    if [ -n "$config_file" ]; then
        found=$(find /usr /usr/local -type f -name "$config_file" 2>/dev/null | head -n1)
    else
        found=$(find /usr /usr/local -type f \( -name "Qt6CoreConfig.cmake" -o -name "Qt5CoreConfig.cmake" \) 2>/dev/null | head -n1)
    fi

    if [ -n "$found" ]; then
        echo "$(dirname "$found")/.."
        return 0
    fi

    echo "Error: Can't detect cmake prefix path for $REQ." >&2
    exit 1
}

# ==========================
# Detect Qt version and prefix
# ==========================
CMAKE_PREFIX=""
QT_VERSION=""

if [ "$QT_CHOICE" = "qt6" ]; then
    if command -v qmake6 >/dev/null 2>&1 || command -v qmake-qt6 >/dev/null 2>&1 || command -v qtpaths6 >/dev/null 2>&1; then
        QT_VERSION=$(get_qt_version "qt6")
        CMAKE_PREFIX=$(get_cmake_prefix "qt6")
        echo "Using Qt $QT_VERSION (Qt6) from: $CMAKE_PREFIX"
    else
        echo "Error: Qt6 was explicitly requested but not found." >&2
        exit 1
    fi
elif [ "$QT_CHOICE" = "qt5" ]; then
    if command -v qmake-qt5 >/dev/null 2>&1 || command -v qt5-qmake >/dev/null 2>&1 || command -v qtpaths-qt5 >/dev/null 2>&1 || command -v qmake >/dev/null 2>&1; then
        QT_VERSION=$(get_qt_version "qt5")
        CMAKE_PREFIX=$(get_cmake_prefix "qt5")
        echo "Using Qt $QT_VERSION (Qt5) from: $CMAKE_PREFIX"
    else
        echo "Error: Qt5 was explicitly requested but not found." >&2
        exit 1
    fi
else
    # auto-detect qt version
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
select_max_gpp() {
    local candidates max_ver="" max_bin=""
    
    candidates=$(compgen -c g++ | sort -u)

    for bin in $candidates; do
        if command -v "$bin" >/dev/null 2>&1; then
            ver=$("$bin" -dumpfullversion -dumpversion 2>/dev/null | head -n1)
            if [[ -n "$ver" ]]; then
                if [[ -z "$max_ver" ]] || verlt "$max_ver" "$ver"; then
                    max_ver=$ver
                    max_bin=$bin
                fi
            fi
        fi
    done

    if [[ -z "$max_bin" ]]; then
        echo "Error: Can't find g++ compiller" >&2
        exit 1
    fi

    echo "$max_bin"
}
CXX_COMPILER=$(select_max_gpp)
echo "Found compiler: $CXX_COMPILER"

# ==========================
# Build type
# ==========================
BUILD_TYPE=Release

# ==========================
# Build project
# ==========================
SANITIZED_QT_VERSION=$(echo "$QT_VERSION" | tr '.' '_' | tr ' ' '_')
BUILD_DIR="build-omodscan-Qt_${SANITIZED_QT_VERSION}_g++_${ARCH}-${BUILD_TYPE}"
echo "Starting build in: $BUILD_DIR"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"
cmake ../omodscan -GNinja -DCMAKE_PREFIX_PATH="${CMAKE_PREFIX}" -DCMAKE_CXX_COMPILER=${CXX_COMPILER} -DCMAKE_BUILD_TYPE=${BUILD_TYPE}
ninja
echo "Build finished successfully in $BUILD_DIR."
echo ""

NINJA_VER=$(ninja --version)
if verlt "1.11.0" "$NINJA_VER"; then
    echo "To install Open ModScan, run:"
    echo ""
    if [ "$EUID" -eq 0 ]; then
        echo -e "    cd $BUILD_DIR && ninja install"
    else
        if can_sudo; then
            echo -e "    cd $BUILD_DIR && sudo ninja install"
        else
            echo -e "    cd $BUILD_DIR && su -c 'ninja install'"
        fi
    fi
    echo ""
fi
