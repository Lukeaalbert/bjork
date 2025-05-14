#!/bin/bash
set -e

# config
REPO_BASE="https://raw.githubusercontent.com/Lukeaalbert/bjork/main/bjork-tools"
INSTALL_DIR="$HOME/.local/bin"
BJORK_LISTEN_URL="$REPO_BASE/bjork-listen"
BJORK_CPP_URL="$REPO_BASE/bjork.cpp"
CMAKE_LISTS_URL="$REPO_BASE/CMakeLists.txt"

# setup install directories
mkdir -p "$INSTALL_DIR"
TMP_DIR=$(mktemp -d)
BUILD_DIR="$TMP_DIR/build"

echo "Downloading files to temp directory..."
curl -fsSL "$BJORK_LISTEN_URL" -o "$TMP_DIR/bjork-listen"
curl -fsSL "$BJORK_CPP_URL" -o "$TMP_DIR/bjork.cpp"
curl -fsSL "$CMAKE_LISTS_URL" -o "$TMP_DIR/CMakeLists.txt"

# make bjork-listen executable and install
chmod +x "$TMP_DIR/bjork-listen"
mv "$TMP_DIR/bjork-listen" "$INSTALL_DIR/"
echo "Installed bjork-listen to $INSTALL_DIR"

# build bjork binary
echo "⚙️  Building bjork binary..."
mkdir "$BUILD_DIR"
cd "$BUILD_DIR"
cmake ..
cmake --build .

# move bjork binary
mv bjork "$INSTALL_DIR/"
echo "Installed bjork to $INSTALL_DIR"

# add INSTALL_DIR to PATH if not already
if [[ ":$PATH:" != *":$INSTALL_DIR:"* ]]; then
  SHELL_CONFIG=""
  if [[ $SHELL == */bash ]]; then
    SHELL_CONFIG="$HOME/.bashrc"
  elif [[ $SHELL == */zsh ]]; then
    SHELL_CONFIG="$HOME/.zshrc"
  fi

  echo "Adding $INSTALL_DIR to PATH..."
  if [[ -n "$SHELL_CONFIG" ]]; then
    echo 'export PATH="$HOME/.local/bin:$PATH"' >> "$SHELL_CONFIG"
    echo "Added to $SHELL_CONFIG"
    echo "Run: source $SHELL_CONFIG"
  else
    echo "Could not detect your shell config. Add this manually to your shell profile:"
    echo 'export PATH="$HOME/.local/bin:$PATH"'
  fi
fi

# final checks
echo "Verifying install..."
if command -v bjork &>/dev/null && command -v bjork-listen &>/dev/null; then
  echo "Install complete! You can now run:"
  echo "   bjork-listen g++ badcode.cpp"
  echo "   bjork --explain"
else
  echo "bjork or bjork-listen not found in PATH."
  echo "Try running 'source ~/.bashrc' or 'source ~/.zshrc', or manually add:"
  echo '   export PATH="$HOME/.local/bin:$PATH"'
fi

# Cleanup
rm -rf "$TMP_DIR"
