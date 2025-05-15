#!/bin/bash
set -e

# config
REPO_BASE="https://raw.githubusercontent.com/Lukeaalbert/bjork/main/bjork-tools"
INSTALL_DIR="$HOME/.local/bin"
BJORK_LISTEN_URL="$REPO_BASE/bjork-listen"
BJORK_CPP_URL="$REPO_BASE/bjork.cpp"
CMAKE_LISTS_URL="$REPO_BASE/CMakeLists.txt"

echo "Installing Bjork CLI to: $INSTALL_DIR"
mkdir -p "$INSTALL_DIR"
TMP_DIR=$(mktemp -d)

# download files
echo "Downloading source files..."
curl -fsSL "$BJORK_CPP_URL" -o "$TMP_DIR/bjork.cpp"
curl -fsSL "$BJORK_LISTEN_URL" -o "$TMP_DIR/spinner.cpp"
curl -fsSL "$CMAKE_LISTS_URL" -o "$TMP_DIR/CMakeLists.txt"
curl -fsSL "$BJORK_LISTEN_URL" -o "$TMP_DIR/bjork-listen"

# setup bjork-listen
chmod +x "$TMP_DIR/bjork-listen"
mv "$TMP_DIR/bjork-listen" "$INSTALL_DIR/"

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
if command -v bjork > /dev/null && command -v bjork-listen > /dev/null; then
  echo "Bjork installed successfully!"
  echo "Try running:"
  echo "   bjork-listen g++ badcode.cpp"
  echo "   bjork --explain"
else
  echo "Installation complete, but 'bjork' or 'bjork-listen' not found in PATH yet."
  echo "   You may need to restart your terminal or run:"
  echo "   source ~/.bashrc   or   source ~/.zshrc"
fi
