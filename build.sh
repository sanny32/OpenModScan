#!/usr/bin/env bash
set -e

echo "=================================="
echo " OpenModScan build script (Linux) "
echo "=================================="
echo ""

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
CHECK_CMD=""
INSTALL_CMD=""
SEARCH_CMD=""

case "$ID" in
    debian|ubuntu|linuxmint|astra)
        DISTRO="debian-based"
        CHECK_CMD="dpkg -s"
        INSTALL_CMD="apt install -y"
        SEARCH_CMD="apt-cache search --names-only"
        ;;
    rhel|fedora|rocky|redos)
        DISTRO="rhel-based"
        CHECK_CMD="rpm -q"
        INSTALL_CMD="dnf install -y"
        SEARCH_CMD="dnf list --available"
        ;;
    altlinux)
        DISTRO="altlinux"
        CHECK_CMD="rpm -q"
        INSTALL_CMD="apt-get install -y"
        SEARCH_CMD="apt-cache search --names-only"
        ;;
    suse|opensuse*)
        DISTRO="suse-based"
        CHECK_CMD="rpm -q"
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
    rocky)
        check_min_os_version "9.7"
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
# Check other requirements
# ==========================
case "$ID" in
    rocky)
        if ! dnf repolist enabled | grep -q "crb"; then
            echo -e "\033[31mError: CRB repository is not enabled in Rocky Linux.\033[0m"
            echo "Please enable it with root privileges:"
            echo "  dnf config-manager --set-enabled crb"
            echo ""
            exit 1
        fi
        ;;
esac

# ==========================
# Parse script arguments
# ==========================
QT_CHOICE=""
for arg in "$@"; do
    case "$arg" in
        -qt5|qt5)
            QT_CHOICE="qt5"
            shift
            ;;
        -qt6|qt6)
            QT_CHOICE="qt6"
            shift
            ;;
        *)
            ;;
    esac
done

# ==========================
# Can use sudo?
# ==========================
CAN_SUDO=0
if command -v sudo >/dev/null 2>&1; then
    if sudo -v 2>/dev/null; then
        CAN_SUDO=1
    fi
fi

# ==========================
# Get Qt5 packages
# ==========================
get_qt5_packages() {
    case "$DISTRO" in
        debian-based)
            echo "qtbase5-dev qtbase5-dev-tools qttools5-dev qttools5-dev-tools libqt5serialport5-dev libqt5serialbus5-dev"
            ;;
        rhel-based)
            echo "qt5-qtbase-devel qt5-qttools-devel qt5-qtserialport-devel qt5-qtserialbus-devel"
            ;;
        altlinux)
            echo "qt5-base-devel qt5-tools-devel qt5-serialport-devel qt5-serialbus-devel"
            ;;
        suse-based)
            echo "libqt5-qtbase-devel libqt5-qttools-devel libqt5-qtserialport-devel libqt5-qtserialbus libqt5-qtserialbus-devel"
            ;;
    esac
}

# ==========================
# Get Qt6 packages
# ==========================
get_qt6_packages() {
    case "$DISTRO" in
        debian-based)
            case "$ID-${VERSION_ID%%.*}" in
                ubuntu-22)
                    echo "qt6-base-dev qt6-base-dev-tools qt6-tools-dev qt6-tools-dev-tools qt6-l10n-tools libqt6serialport6-dev \
                            libqt6serialbus6-bin libqt6serialbus6-dev libqt6core5compat6-dev"
                ;;
                *)
                    echo "qt6-base-dev qt6-base-dev-tools qt6-tools-dev qt6-tools-dev-tools qt6-serialport-dev qt6-serialbus-dev \
                            qt6-5compat-dev"
                ;;
            esac
            ;;
        rhel-based)
            echo "qt6-qtbase-devel qt6-qttools-devel qt6-qtserialport-devel qt6-qtserialbus-devel qt6-qt5compat-devel"
            ;;
        altlinux)
            echo "qt6-base-devel qt6-tools-devel qt6-serialport-devel qt6-serialbus-devel qt6-5compat-devel"
            ;;
        suse-based)
            echo "qt6-base-devel qt6-tools-devel qt6-serialport-devel qt6-serialbus-devel qt6-qt5compat-devel qt6-linguist-devel"
            ;;
    esac
}

# ==========================
# Function to get packages
# Returns combined general + Qt package list
# ==========================
get_packages() {
    local general_packages=""
    local qt_packages=""

    case "$DISTRO" in
        debian-based)
            general_packages="build-essential cmake ninja-build libxcb-cursor-dev libgl1-mesa-dev pkg-config"
            ;;
        rhel-based)
            general_packages="gcc gcc-c++ cmake ninja-build pkgconf-pkg-config xcb-util-cursor-devel"
            ;;
        altlinux)
            general_packages="gcc gcc-c++ cmake ninja-build pkg-config libxcbutil-cursor"
            ;;
        suse-based)
            general_packages="gcc gcc-c++ cmake ninja pkg-config libxcb-cursor0"
            ;;
    esac

    case "$QT_CHOICE" in
        qt5)
            qt_packages=$(get_qt5_packages) || return 1
            ;;
        qt6)
            qt_packages=$(get_qt6_packages) || return 1
            ;;
    esac

    echo "$general_packages $qt_packages"
}

# ==========================
# Install packages
# ==========================
install_pkg() {
    local pkg_groups=("$@")
    local missing=()
  
    for group in "${pkg_groups[@]}"; do
        printf "Checking for %-30s... " "$group"
        if $CHECK_CMD "$group" >/dev/null 2>&1; then
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
                if [ "$CAN_SUDO" -eq 1 ]; then
                    if sudo $INSTALL_CMD "${missing[@]}"; then
                        :
                    else
                        status=$?
                        err=$(sudo -n true 2>&1)
                        if echo "$err" | grep -Eq "not in the sudoers file|may not run sudo"; then
                            CAN_SUDO=0
                            echo "Using su (user may not run sudo)..."
                            su -c "$INSTALL_CMD ${missing[*]}"
                        else
                            echo "sudo failed with code $status"
                            exit $status
                        fi
                    fi
                else
                    echo "Using su (user may not run sudo)..."
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
# Install prerequisites
# ==========================
install_prereqs() {
    local pkgs
    pkgs=$(get_packages) || return 1
    install_pkg $pkgs
}

# ==========================
# Always check/install prereqs first
# ==========================
echo "Checking prerequisites for $ID..."
if [ -z "$QT_CHOICE" ]; then
    if $SEARCH_CMD '^qt6-' 2>/dev/null | head -n30 | grep -q "qt6-"; then
        QT_CHOICE="qt6"
    elif $SEARCH_CMD '^libqt6' 2>/dev/null | head -n30 | grep -q "libqt6"; then
        QT_CHOICE="qt6"
    elif $SEARCH_CMD 'qt6-*' 2>/dev/null | head -n30 | grep -q "qt6-"; then
        QT_CHOICE="qt6"
    else
        QT_CHOICE="qt5"
    fi
fi
install_prereqs
echo ""

# ==========================
# Get Qt version string
# ==========================
get_qt_version() {
    local ver=""
    local probes=()

    if [ "$QT_CHOICE" = "qt6" ]; then
        probes=(qmake6 qmake-qt6 qtpaths6 qmake)
    elif [ "$QT_CHOICE" = "qt5" ]; then
        probes=(qmake-qt5 qt5-qmake qtpaths-qt5 qmake)
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
                local major="${ver%%.*}"
                case "$QT_CHOICE" in
                    qt6)
                        if [ "$major" -eq 6 ]; then
                            echo "$ver"
                            return 0
                        fi
                        ;;
                    qt5)
                        if [ "$major" -eq 5 ]; then
                            echo "$ver"
                            return 0
                        fi
                        ;;
                esac
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
    local config_file=""
    local prefix=""

    if [ "$QT_CHOICE" = "qt6" ]; then
        config_file="Qt6CoreConfig.cmake"
    elif [ "$QT_CHOICE" = "qt5" ]; then
        config_file="Qt5CoreConfig.cmake"
    fi

    if command -v pkg-config >/dev/null 2>&1; then
        case "$QT_CHOICE" in
            qt6) prefix=$(pkg-config --variable=prefix Qt6Core 2>/dev/null || true) ;;
            qt5) prefix=$(pkg-config --variable=prefix Qt5Core 2>/dev/null || true) ;;
        esac
        if [ -n "$prefix" ]; then
            echo "$prefix"
            return 0
        fi
    fi

    local probes=()
    if [ "$QT_CHOICE" = "qt6" ]; then
        probes=(qmake6 qmake-qt6 qtpaths6 qmake)
    elif [ "$QT_CHOICE" = "qt5" ]; then
        probes=(qmake-qt5 qt5-qmake qtpaths-qt5 qmake)
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
# Detect Qt version and cmake prefix
# ==========================
QT_VERSION=$(get_qt_version)
CMAKE_PREFIX=$(get_cmake_prefix)

# ==========================
# Check minimal Qt version
# ==========================
MIN_QT_VERSION="5.15.0"
if verlt "$QT_VERSION" "$MIN_QT_VERSION"; then
    echo "Error: Qt >= $MIN_QT_VERSION is required, but found $QT_VERSION" >&2
    exit 1
else
    echo "Using Qt $QT_VERSION (Qt5) from: $CMAKE_PREFIX"
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
        if [ "$CAN_SUDO" -eq 1 ]; then
            echo -e "    cd $BUILD_DIR && sudo ninja install"
        else
            echo -e "    cd $BUILD_DIR && su -c 'ninja install'"
        fi
    fi
    echo ""
fi