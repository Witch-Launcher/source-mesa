#!/bin/bash
# Build KosmicKrisp Vulkan driver for iOS
# Requires: Mesa mainline source, LLVM 20+, Xcode CLI tools
#
# Usage:
#   1. Clone Mesa: git clone https://gitlab.freedesktop.org/mesa/mesa.git mesa-kosmickrisp
#   2. Run this script from the Angel-Aura-Amethyst-iOS directory

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"
SOURCE_DIR="$PROJECT_DIR/source"
MESA_SRC="$SOURCE_DIR/mesa-kosmickrisp"
MESA_BUILD="$SOURCE_DIR/mesa-kk-build"
PREFIX="$PROJECT_DIR/Natives/resources/Frameworks"
CROSS_FILE="$SCRIPT_DIR/cross-ios-arm64-kkosmic.txt"

echo "=== KosmicKrisp iOS Build ==="
echo "Mesa source: $MESA_SRC"
echo "Build dir:   $MESA_BUILD"
echo "Prefix:      $PREFIX"

# Check Mesa source exists
if [ ! -d "$MESA_SRC/src/kosmickrisp" ]; then
    echo "ERROR: Mesa source not found at $MESA_SRC"
    echo "Please clone Mesa mainline:"
    echo "  git clone https://gitlab.freedesktop.org/mesa/mesa.git $MESA_SRC"
    exit 1
fi

# Clean and create build directory
rm -rf "$MESA_BUILD"
mkdir -p "$MESA_BUILD"

# Setup Meson for KosmicKrisp on iOS
cd "$MESA_SRC"
meson setup "$MESA_BUILD" . \
    --cross-file "$CROSS_FILE" \
    -Dplatforms= \
    -Dvulkan-drivers=kosmickrisp \
    -Dgallium-drivers= \
    -Dopengl=false \
    -Dgles1=disabled \
    -Dgles2=disabled \
    -Dglx=disabled \
    -Dglx-direct=false \
    -Dgbm=disabled \
    -Dllvm=disabled \
    -Dvalgrind=disabled \
    -Dlibunwind=disabled \
    -Dzstd=disabled \
    -Dshader-cache=disabled \
    -Dexpat=disabled \
    -Dbuildtype=release \
    -Dprefix="$PREFIX" \
    -Dlibdir=lib \
    -Dbindir=bin

# Build
echo "Building KosmicKrisp..."
ninja -C "$MESA_BUILD" -j$(sysctl -n hw.ncpu)

# Copy output
echo "Copying KosmicKrisp driver..."
cp "$MESA_BUILD/src/vulkan/libvulkan_kosmickrisp.dylib" "$PREFIX/" 2>/dev/null || \
cp "$MESA_BUILD/src/vulkan/libvulkan_kosmickrisp.dylib" "$PREFIX/" || \
echo "WARNING: libvulkan_kosmickrisp.dylib not found, check build output"

echo "=== KosmicKrisp iOS Build Complete ==="
echo "Output: $PREFIX/libvulkan_kosmickrisp.dylib"
