#!/bin/bash
# Script to install the build dependencies for the Chompzilla project

# Define the colors
SETCOLOR_FAILURE="\\033[1;31m"
SETCOLOR_SUCCESS="\\033[1;32m"
SETCOLOR_WARNING="\\033[1;33m"
SETCOLOR_NORMAL="\\033[0;39m"

# Define the functions to print the messages
success() {
    echo -e "${SETCOLOR_SUCCESS}$*${SETCOLOR_NORMAL}"
}
error() {
    echo -e "${SETCOLOR_FAILURE}$*${SETCOLOR_NORMAL}"
    exit 1
}
warn() {
    echo -e "${SETCOLOR_WARNING}$*${SETCOLOR_NORMAL}"
}

# Check if the sudo command is available
if ! command -v sudo &> /dev/null; then
    # Check if the user is root
    if [[ $EUID -ne 0 ]]; then
        error "Please run the script as root or install sudo"
    else
        SUDO=""
    fi
else
    SUDO="sudo"
fi

# Function to check if the script is run on Linux
function check_linux {
    if [[ $(uname) != "Linux" ]]; then
        echo "This script is only supported on Linux"
        exit 1
    fi
}

# Function to check the package manager
function check_package_manager {
    if command -v apt-get &> /dev/null; then
        PACKAGE_MANAGER="apt-get"
    elif command -v dnf &> /dev/null; then
        PACKAGE_MANAGER="dnf"
    else
        echo "Unsupported package manager"
        exit 1
    fi
}

# Function to install the build dependencies using apt-get
function install_apt_deps {
    $SUDO apt-get update
    # Ask the user if he wants to upgrade the packages
    read -p "Do you want to upgrade the packages? [y/n]: " UPGRADE
    if [[ $UPGRADE == "y" ]]; then
        $SUDO apt-get upgrade -y
    fi
    $SUDO apt-get install -y subversion build-essential libgtk-4-1 libgtk-4-dev lcov graphviz doxygen doxygen-gui clang libcriterion-dev
}

# Function to install the build dependencies using dnf
function install_dnf_deps {
    $SUDO dnf check-update
    # Ask the user if he wants to upgrade the packages
    read -p "Do you want to upgrade the packages? [y/n]: " UPGRADE
    if [[ $UPGRADE == "y" ]]; then
        $SUDO dnf upgrade -y
    fi
    $SUDO dnf install -y subversion make gcc gtk4 gtk4-devel lcov graphviz doxygen doxygen-doxywizard clang
}

# Main function
function main {
    success "Welcome to the Chompzilla project!"
    success "This script will install the build dependencies for the project"
    # Ask the user if he wants to continue
    read -p "Do you want to continue? [y/n]: " CONTINUE
    if [[ $CONTINUE != "y" ]]; then
        exit 0
    fi

    warn "Checking the system..."
    check_linux
    success "You are running Linux!"
    warn "Checking the package manager..."
    check_package_manager
    if [[ $PACKAGE_MANAGER == "apt-get" ]]; then
        success "The package manager is apt-get"
        warn "Installing the build dependencies using apt-get..."
        install_apt_deps
        success "The build dependencies have been installed successfully!"
    elif [[ $PACKAGE_MANAGER == "dnf" ]]; then
        success "The package manager is dnf"
        warn "Installing the build dependencies using dnf..."
        install_dnf_deps
        success "The build dependencies have been installed successfully!"
    fi
    success "You are all set to build the Chompzilla project!"
}

# Call the main function
main