#!/bin/bash
set -e

# config
REPO_BASE="https://raw.githubusercontent.com/Lukeaalbert/bjork/main/bjork-tools"
INSTALL_DIR="$HOME/.local/bin"
INTERNAL_DIR="$HOME/.local/share/bjork"

echo "Installing Bjork CLI to: $INSTALL_DIR"
mkdir -p "$INSTALL_DIR"
mkdir -p "$INTERNAL_DIR"
TMP_DIR=$(mktemp -d)

# download files
echo "Downloading source files..."
curl -fsSL "$REPO_BASE/bjork.cpp" -o "$TMP_DIR/bjork.cpp"
curl -fsSL "$REPO_BASE/spinner.cpp" -o "$TMP_DIR/spinner.cpp"
curl -fsSL "$REPO_BASE/spinner.h" -o "$TMP_DIR/spinner.h"
curl -fsSL "$REPO_BASE/system.cpp" -o "$TMP_DIR/system.cpp"
curl -fsSL "$REPO_BASE/system.h" -o "$TMP_DIR/system.h"
curl -fsSL "$REPO_BASE/CMakeLists.txt" -o "$TMP_DIR/CMakeLists.txt"
curl -fsSL "$REPO_BASE/bjork-listen" -o "$TMP_DIR/bjork-listen"

# setup bjork-listen (internal-only)
chmod +x "$TMP_DIR/bjork-listen"
mv "$TMP_DIR/bjork-listen" "$INTERNAL_DIR/"

# setup bjork-explanation-complexity (internal-only)
touch "$TMP_DIR/bjork-explanation-complexity"
echo "5" > "$TMP_DIR/bjork-explanation-complexity" # default complexity of 5
chmod +x "$TMP_DIR/bjork-explanation-complexity"
mv "$TMP_DIR/bjork-listen" "$INTERNAL_DIR/"

# build bjork binary
echo "Building bjork binary with CMake..."
mkdir "$TMP_DIR/build"
cd "$TMP_DIR/build"
cmake .. > /dev/null
cmake --build . > /dev/null

echo "Installing bjork binary..."
mv bjork "$INSTALL_DIR/"

# cleanup
cd ~
rm -rf "$TMP_DIR"

# path setup
NEEDS_PATH_ADDITION=false
if ! echo "$PATH" | grep -q "$INSTALL_DIR"; then
  NEEDS_PATH_ADDITION=true
fi

if $NEEDS_PATH_ADDITION; then
  SHELL_CONFIG=""
  if [[ $SHELL == */bash ]]; then
    SHELL_CONFIG="$HOME/.bashrc"
  elif [[ $SHELL == */zsh ]]; then
    SHELL_CONFIG="$HOME/.zshrc"
  fi

  if [[ -n "$SHELL_CONFIG" ]]; then
    echo 'export PATH="$HOME/.local/bin:$PATH"' >> "$SHELL_CONFIG"
    echo "Added $INSTALL_DIR to PATH in $SHELL_CONFIG"
    echo "Run: source $SHELL_CONFIG"
  else
    echo "Couldn't detect shell config. Add this to your shell profile manually:"
    echo 'export PATH="$HOME/.local/bin:$PATH"'
  fi
else
  echo "$INSTALL_DIR already in PATH"
fi

# verify
if command -v bjork > /dev/null; then
  echo "Bjork installed successfully!"
  echo "Try running:"
  echo "   bjork --listen g++ badcode.cpp"
  echo "   bjork --explain"
else
  echo "Installation complete, but 'bjork' not found in PATH yet."
  echo "   You may need to restart your terminal or run:"
  echo "   source ~/.bashrc   or   source ~/.zshrc"
fi
