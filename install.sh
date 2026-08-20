#!/usr/bin/env bash

set -Eeuo pipefail

readonly RESET='\033[0m'
readonly GREEN='\033[1;32m'
readonly RED='\033[1;31m'
readonly YELLOW='\033[1;33m'

readonly INSTALL_DIR="/usr/local/bin"
readonly CONFIG_DIR="${HOME}/.config/pwnit"
readonly BUILD_DIR="build"

log() {
    echo -e "${GREEN}==>${RESET} $*"
}

warn() {
    echo -e "${YELLOW}Warning:${RESET} $*" >&2
}

error() {
    echo -e "${RED}Error:${RESET} $*" >&2
}

die() {
    error "$*"
    exit 1
}

detect_distro() {
    if [[ ! -r /etc/os-release ]]; then
        echo "unknown"
        return
    fi

    source /etc/os-release

    case "${ID,,}" in
        arch|manjaro)
            echo "arch"
            ;;
        debian|ubuntu|linuxmint|pop|elementary)
            echo "debian"
            ;;
        fedora)
            echo "fedora"
            ;;
        rhel|centos|rocky|almalinux)
            echo "rhel"
            ;;
        *)
            echo "${ID,,}"
            ;;
    esac
}

require_sudo() {
    if [[ $EUID -eq 0 ]]; then
        return 0
    fi

    command -v sudo >/dev/null 2>&1 \
        || die "sudo is required to install system packages."

    sudo -v
}

install_dependencies() {
    local distro
    distro="$(detect_distro)"

    log "Detected distro: ${distro}"

    case "$distro" in
        arch)
            require_sudo
            sudo pacman -Syu --needed --noconfirm \
                cmake \
                gcc \
                make \
                tar \
                patchelf \
                elfutils
            ;;

        debian)
            require_sudo
            sudo apt-get update
            sudo apt-get install -y \
                cmake \
                build-essential \
                tar \
                patchelf \
                elfutils
            ;;

        fedora)
            require_sudo
            sudo dnf install -y \
                cmake \
                gcc \
                gcc-c++ \
                make \
                tar \
                patchelf \
                elfutils
            ;;

        rhel)
            require_sudo

            if command -v dnf >/dev/null 2>&1; then
                sudo dnf install -y \
                    cmake \
                    gcc \
                    gcc-c++ \
                    make \
                    tar \
                    patchelf \
                    elfutils
            else
                sudo yum install -y \
                    cmake \
                    gcc \
                    gcc-c++ \
                    make \
                    tar \
                    patchelf \
                    elfutils
            fi
            ;;

        *)
            die "Unsupported distribution: ${distro}"
            ;;
    esac

    local required_commands=(
        cmake
        make
        tar
        patchelf
        eu-unstrip
    )

    local cmd

    for cmd in "${required_commands[@]}"; do
        command -v "$cmd" >/dev/null 2>&1 \
            || die "Required command not found: ${cmd}"
    done
}

compile_pwnit() {
    log "Compiling pwnit..."

    [[ -f CMakeLists.txt ]] \
        || die "CMakeLists.txt not found. Run this script from the project root."

    cmake \
        -S . \
        -B "$BUILD_DIR" \
        -DCMAKE_BUILD_TYPE=Release

    cmake \
        --build "$BUILD_DIR" \
        --parallel
}

install_configs() {
    log "Installing configuration files..."

    [[ -f config/pwnit.toml ]] \
        || die "Missing configuration file: config/pwnit.toml"

    [[ -f config/default.py ]] \
        || die "Missing template file: config/default.py"

    install -d "${CONFIG_DIR}/templates"

    install \
        -m 644 \
        config/pwnit.toml \
        "${CONFIG_DIR}/pwnit.toml"

    install \
        -m 644 \
        config/default.py \
        "${CONFIG_DIR}/templates/default.py"
}

install_binary() {
    log "Installing pwnit globally..."

    local binary

    if [[ -f "${BUILD_DIR}/pwnit" ]]; then
        binary="${BUILD_DIR}/pwnit"
    elif [[ -f pwnit ]]; then
        binary="pwnit"
    else
        die "Binary 'pwnit' not found."
    fi

    require_sudo

    sudo install \
        -Dm755 \
        "$binary" \
        "${INSTALL_DIR}/pwnit"
}

main() {
    install_dependencies
    compile_pwnit
    install_configs
    install_binary

    echo
    echo -e "${GREEN}pwnit installed successfully!${RESET}"
    echo
    echo "Binary: ${INSTALL_DIR}/pwnit"
    echo "Config: ${CONFIG_DIR}"
}

main "$@"
