#!/usr/bin/env bash

set -e

DEFAULT="\e[0m"
GREEN="\e[1;32m"
RED="\e[1;31m"

detect_distro() {
    if [[ -f /etc/os-release ]]; then
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
        return 0
    fi

    if [[ -f /etc/debian_version ]]; then
        echo "debian"
        return 0
    fi

    if [[ -f /etc/redhat-release ]]; then
        echo "rhel"
        return 0
    fi

    echo "unknown"
}

install_dependencies() {
    local distro
    distro="$(detect_distro)"

    echo -e "${GREEN}Detected distro: ${distro}${DEFAULT}"

    case "$distro" in
        arch)
            sudo pacman -Sy --needed --noconfirm tar patchelf
            ;;

        debian)
            sudo apt-get update
            sudo apt-get install -y tar patchelf
            ;;

        fedora)
            sudo dnf install -y tar patchelf
            ;;

        rhel)
            if command -v dnf >/dev/null 2>&1; then
                sudo dnf install -y tar patchelf
            else
                sudo yum install -y tar patchelf
            fi
            ;;

        *)
            echo -e "${RED}Distro not supported: ${distro}${DEFAULT}"
            exit 1
            ;;
    esac
}

compile_pwnit() {
    echo -e "${GREEN}Compiling pwnit...${DEFAULT}"

    mkdir -p build
    cmake -S . -B build
    cmake --build build
}

install_configs() {
    echo -e "${GREEN}Installing configuration files...${DEFAULT}"

    mkdir -p "$HOME/.config/pwnit/templates"

    cp config/pwnit.toml "$HOME/.config/pwnit/"
    cp config/default.py "$HOME/.config/pwnit/templates/"
}

install_binary() {
    echo -e "${GREEN}Installing pwnit globally...${DEFAULT}"

    if [[ ! -f pwnit && ! -f build/pwnit ]]; then
        echo -e "${RED}Binary 'pwnit' not found.${DEFAULT}"
        exit 1
    fi

    if [[ -f build/pwnit ]]; then
        sudo install -m 755 build/pwnit /usr/local/bin/pwnit
    else
        sudo install -m 755 pwnit /usr/local/bin/pwnit
    fi
}

main() {
    install_dependencies
    compile_pwnit
    install_configs
    install_binary

    echo -e "${GREEN}pwnit installed successfully!${DEFAULT}"
}

main
