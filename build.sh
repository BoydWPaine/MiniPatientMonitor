#!/usr/bin/env bash
# MiniPatientMonitor one-click build script.
# Usage:
#   ./build.sh            # Debug (default)
#   ./build.sh debug      # Debug
#   ./build.sh release    # Release
#   ./build.sh clean      # Remove build directory
#   ./build.sh help       # Show usage

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${ROOT_DIR}/build"
CMAKE_MIN_VERSION="3.20"
JOBS="${JOBS:-$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)}"

usage() {
    cat <<EOF
MiniPatientMonitor build script

Usage:
  $(basename "$0") [debug|release|clean|help]

Commands:
  debug     Configure and build Debug (default)
  release   Configure and build Release
  clean     Delete ${BUILD_DIR}
  help      Show this message

Environment:
  JOBS      Parallel build jobs (default: ${JOBS})

Outputs:
  ${BUILD_DIR}/device/mini_device
  ${BUILD_DIR}/host/mini_host
EOF
}

require_cmake() {
    if ! command -v cmake >/dev/null 2>&1; then
        echo "Error: cmake not found. Run ./tools_install.sh first." >&2
        exit 1
    fi

    local cmake_version
    cmake_version="$(cmake --version | head -n1 | awk '{print $3}')"
    if ! printf '%s\n%s\n' "${CMAKE_MIN_VERSION}" "${cmake_version}" \
        | sort -V -C 2>/dev/null; then
        echo "Error: cmake >= ${CMAKE_MIN_VERSION} required, found ${cmake_version}" >&2
        exit 1
    fi
}

clean_build() {
    if [[ -d "${BUILD_DIR}" ]]; then
        rm -rf "${BUILD_DIR}"
        echo "Cleaned ${BUILD_DIR}"
    else
        echo "Nothing to clean (${BUILD_DIR} does not exist)"
    fi
}

build_project() {
    local build_type="$1"

    require_cmake

    echo "==> Configuring ${build_type} build in ${BUILD_DIR}"
    cmake -B "${BUILD_DIR}" \
        -DCMAKE_BUILD_TYPE="${build_type}" \
        -DMPM_BUILD_DEVICE=ON \
        -DMPM_BUILD_HOST=ON

    echo "==> Building with ${JOBS} job(s)"
    cmake --build "${BUILD_DIR}" -j "${JOBS}"

    echo
    echo "Build succeeded (${build_type})"
    echo "  Device: ${BUILD_DIR}/device/mini_device"
    echo "  Host:   ${BUILD_DIR}/host/mini_host"
}

main() {
    local action="${1:-debug}"

    case "${action}" in
        debug)
            build_project "Debug"
            ;;
        release)
            build_project "Release"
            ;;
        clean)
            clean_build
            ;;
        help|-h|--help)
            usage
            ;;
        *)
            echo "Error: unknown command '${action}'" >&2
            usage >&2
            exit 1
            ;;
    esac
}

main "$@"