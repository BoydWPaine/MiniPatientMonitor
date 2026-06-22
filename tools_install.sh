#!/usr/bin/env bash
# MiniPatientMonitor dependency installer (Phase 1 / M1).
# Installs build tools required by CMake, FetchContent (git), and protobuf runtime.
#
# Usage:
#   ./tools_install.sh
#   sudo ./tools_install.sh

set -euo pipefail

CMAKE_MIN_VERSION="3.20"
PROTOBUF_MIN_VERSION="3.21"

usage() {
    cat <<EOF
MiniPatientMonitor tools installer

Installs:
  - build-essential (gcc/g++, make)
  - cmake (>= ${CMAKE_MIN_VERSION})
  - git
  - pkg-config
  - zlib development headers
  - protobuf-compiler + libprotobuf-dev (>= ${PROTOBUF_MIN_VERSION})
  - qt6-base-dev + libgl1-mesa-dev (Host Qt UI, M2)
  - libsdl2-dev (Device LVGL UI, M2 — or use CMake FetchContent)

Supported:
  - Debian / Ubuntu (apt)

Optional (not installed by this script — see docs/DevSetup.md):
  - Qt6 (Widgets, Network) for Host UI (M2+)
  - LVGL + SDL2 for Device config UI (M2+)
  - Google Test for unit tests (M5+)
EOF
}

have_sudo() {
    command -v sudo >/dev/null 2>&1
}

run_as_root() {
    if [[ "${EUID}" -eq 0 ]]; then
        "$@"
    elif have_sudo; then
        sudo "$@"
    else
        echo "Error: root privileges required. Re-run with sudo." >&2
        exit 1
    fi
}

detect_distro() {
    if [[ -f /etc/os-release ]]; then
        # shellcheck disable=SC1091
        . /etc/os-release
        echo "${ID:-unknown}"
        return
    fi
    echo "unknown"
}

version_ge() {
    local required="$1"
    local actual="$2"
    printf '%s\n%s\n' "${required}" "${actual}" | sort -V -C
}

install_debian_packages() {
    echo "==> Updating apt package index"
    run_as_root apt-get update

    echo "==> Installing build dependencies"
    echo "==> Installing M1 build dependencies"
    run_as_root apt-get install -y \
        build-essential \
        cmake \
        git \
        pkg-config \
        zlib1g-dev \
        protobuf-compiler \
        libprotobuf-dev

    echo "==> Installing M2 dependencies (Qt6 Host + SDL2 Device UI)"
    run_as_root apt-get install -y \
        qt6-base-dev \
        libgl1-mesa-dev \
        libsdl2-dev
}

verify_tools() {
    local missing=0

    echo
    echo "==> Verifying installed tools"

    for tool in cmake git protoc g++ make pkg-config; do
        if command -v "${tool}" >/dev/null 2>&1; then
            echo "  OK  ${tool}: $(command -v "${tool}")"
        else
            echo "  MISSING  ${tool}"
            missing=1
        fi
    done

    if command -v cmake >/dev/null 2>&1; then
        local cmake_version
        cmake_version="$(cmake --version | head -n1 | awk '{print $3}')"
        if version_ge "${CMAKE_MIN_VERSION}" "${cmake_version}"; then
            echo "  OK  cmake version ${cmake_version}"
        else
            echo "  WARN cmake ${cmake_version} < ${CMAKE_MIN_VERSION}"
            missing=1
        fi
    fi

    if command -v protoc >/dev/null 2>&1; then
        local protoc_version
        protoc_version="$(protoc --version | awk '{print $2}')"
        if version_ge "${PROTOBUF_MIN_VERSION}" "${protoc_version}"; then
            echo "  OK  protoc version ${protoc_version}"
        else
            echo "  WARN protoc ${protoc_version} < ${PROTOBUF_MIN_VERSION}"
            echo "       Build still works: CMake FetchContent builds protobuf v21.12."
        fi
    fi

    if command -v g++ >/dev/null 2>&1; then
        echo "  OK  g++ version $(g++ --version | head -n1)"
    fi

    if [[ "${missing}" -ne 0 ]]; then
        echo
        echo "Some required tools are missing. Please fix and re-run." >&2
        exit 1
    fi

    echo
    echo "All required tools are available."
    echo "Next step: ./build.sh debug"
    echo "For Qt6 / LVGL / GTest (M2/M5), see docs/DevSetup.md"
}

main() {
    if [[ "${1:-}" == "help" || "${1:-}" == "-h" || "${1:-}" == "--help" ]]; then
        usage
        exit 0
    fi

    local distro
    distro="$(detect_distro)"
    echo "Detected distro: ${distro}"

    case "${distro}" in
        ubuntu|debian|linuxmint|pop)
            install_debian_packages
            ;;
        *)
            echo "Error: unsupported distro '${distro}'." >&2
            echo "Install manually: cmake>=${CMAKE_MIN_VERSION}, g++ (C++17), git, zlib dev, protobuf dev." >&2
            exit 1
            ;;
    esac

    verify_tools
}

main "$@"