#!/bin/bash

# OpenModScan Installation Script for Debian/Ubuntu-based systems
# This script will install necessary dependencies, clone the repository,
# build the application using CMake, and place the executable in /usr/local/bin.

# --- Configuration ---
INSTALL_DIR="$HOME/OpenModScan_src" # Directory to clone the source code
BIN_PATH="/usr/local/bin"          # Path to install the executable
APP_NAME="omodscan"                # Name of the executable from CMakeLists.txt
PROJECT_SUBDIR="omodscan"          # Subdirectory where the CMakeLists.txt and source code are located
BUILD_DIR="build"                  # Directory for out-of-source CMake build

# --- Functions ---

# Function to display error messages and exit
error_exit() {
    echo ""
    echo "ERROR: $1" >&2
    echo "Installation failed. Please review the output above."
    exit 1
}

# Function to check if a command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

echo "--- Starting OpenModScan Installation ---"
echo "This script requires root privileges for installing packages."

# --- Initial Checks ---

echo ""
echo "0. Performing initial system checks..."

# Check for sudo
if ! command_exists sudo; then
    error_exit "The 'sudo' command is not found. This script requires sudo to install packages."
fi

# Check for git
if ! command_exists git; then
    error_exit "The 'git' command is not found. Please install git first (e.g., sudo apt install git)."
fi

# Check for cmake (it will be installed, but a pre-check is good)
if ! command_exists cmake; then
    echo "  'cmake' command not found, but will be installed with dependencies."
fi

# Check for make (it will be installed, but a pre-check is good)
if ! command_exists make; then
    echo "  'make' command not found, but will be installed with dependencies."
fi

# Check for Debian/Ubuntu distribution
if [ -f /etc/os-release ]; then
    . /etc/os-release
    if [[ "$ID" != "ubuntu" && "$ID" != "debian" ]]; then
        echo "WARNING: This script is primarily tested and designed for Debian or Ubuntu-based systems."
        echo "         Your distribution is '$ID'. Installation might fail if package names differ."
        read -p "Do you wish to continue anyway? (y/N): " choice
        case "$choice" in
            y|Y ) echo "Continuing with installation...";;
            * ) error_exit "Installation aborted by user due to unsupported distribution.";;
        esac
    else
        echo "  Detected operating system: $PRETTY_NAME"
    fi
else
    echo "WARNING: Could not determine operating system. Assuming Debian/Ubuntu-like."
    read -p "Do you wish to continue anyway? (y/N): " choice
    case "$choice" in
        y|Y ) echo "Continuing with installation...";;
        * ) error_exit "Installation aborted by user due to unknown distribution.";;
    esac
fi
echo "Initial checks completed."

# 1. Update package list and install build dependencies
echo ""
echo "1. Updating package list and installing build dependencies..."
sudo apt update || error_exit "Failed to update apt package list. Check your internet connection or apt sources."
sudo apt install -y build-essential git cmake \
    qtbase5-dev libqt5serialport5-dev libqt5serialbus5-dev \
    libmodbus-dev || error_exit "Failed to install required packages. Please check your internet connection and apt sources."
echo "Dependencies installed successfully."

# 2. Clone the OpenModScan repository
echo ""
echo "2. Cloning OpenModScan repository into $INSTALL_DIR..."
if [ -d "$INSTALL_DIR" ]; then
    echo "Directory $INSTALL_DIR already exists. Removing it to get a fresh clone..."
    rm -rf "$INSTALL_DIR" || error_exit "Failed to remove existing directory $INSTALL_DIR."
fi
git clone https://github.com/sanny32/OpenModScan.git "$INSTALL_DIR" || error_exit "Failed to clone OpenModScan repository. Check the repository URL or your network connection."
echo "Repository cloned successfully."

# --- DEBUGGING STEP: List contents after cloning ---
echo ""
echo "--- DEBUG: Listing contents of $INSTALL_DIR after cloning ---"
ls -F "$INSTALL_DIR" || echo "WARNING: Could not list contents of $INSTALL_DIR."
if [ -d "$INSTALL_DIR/$PROJECT_SUBDIR" ]; then
    echo "--- DEBUG: Listing contents of $INSTALL_DIR/$PROJECT_SUBDIR ---"
    ls -F "$INSTALL_DIR/$PROJECT_SUBDIR" || echo "WARNING: Could not list contents of $INSTALL_DIR/$PROJECT_SUBDIR."
fi
echo "----------------------------------------------------"
# --- END DEBUGGING STEP ---


# 3. Navigate into the source directory where the CMakeLists.txt file is located
echo ""
echo "3. Changing directory to $INSTALL_DIR/$PROJECT_SUBDIR..."
cd "$INSTALL_DIR/$PROJECT_SUBDIR" || error_exit "Failed to change directory to $INSTALL_DIR/$PROJECT_SUBDIR. Ensure the '$PROJECT_SUBDIR' directory exists and was cloned correctly."
echo "Currently in: $(pwd)"

# 4. Prepare for CMake build
echo ""
echo "4. Creating build directory and preparing for CMake..."
if [ -d "$BUILD_DIR" ]; then
    echo "Removing existing build directory: $BUILD_DIR"
    rm -rf "$BUILD_DIR" || error_exit "Failed to remove existing build directory."
fi
mkdir "$BUILD_DIR" || error_exit "Failed to create build directory."
cd "$BUILD_DIR" || error_exit "Failed to change into build directory."
echo "Currently in: $(pwd)"

# 5. Run CMake to configure the project
echo ""
echo "5. Running CMake to configure the project..."
# '..' refers to the parent directory where CMakeLists.txt is located
cmake .. || error_exit "CMake configuration failed. This could indicate missing Qt components or other build issues. Review the output for errors."
echo "CMake configuration completed successfully."

# 6. Build the application with make
echo ""
echo "6. Building OpenModScan with make..."
make -j$(nproc) || error_exit "Make failed. Compilation errors occurred. Review the build output for details."
echo "OpenModScan built successfully."

# 7. Install the executable
echo ""
echo "7. Installing $APP_NAME executable to $BIN_PATH..."
# The executable will be generated in the current build directory
if [ -f "./$APP_NAME" ]; then
    sudo mv "./$APP_NAME" "$BIN_PATH/$APP_NAME" || error_exit "Failed to move executable to $BIN_PATH. Check directory permissions."
    echo "Executable installed: $BIN_PATH/$APP_NAME"
else
    error_exit "OpenModScan executable not found after build. Please ensure the build process completed successfully. Expected executable at: $(pwd)/$APP_NAME"
fi

# 8. Clean up build artifacts (optional but good practice)
# Navigate back to the main source directory to clean up the build directory
echo ""
echo "8. Cleaning up build artifacts..."
cd .. # Go back to $INSTALL_DIR/$PROJECT_SUBDIR
rm -rf "$BUILD_DIR" || echo "WARNING: Failed to remove build directory ($BUILD_DIR)."
echo "Build artifacts cleaned."

echo ""
echo "--- OpenModScan Installation Complete! ---"
echo "You can now run OpenModScan from your terminal by typing: $APP_NAME"
echo ""
echo "Source code (and original build directory) is located at: $INSTALL_DIR/$PROJECT_SUBDIR"
echo ""
echo "If you encounter further issues, please provide the complete output, including any 'DEBUG' or 'WARNING' messages."
