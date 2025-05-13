#!/bin/bash
set -e

# config
REPO_BASE="https://raw.githubusercontent.com/Lukeaalbert/bjork/main/bjork-tools"
INSTALL_DIR="$HOME/.local/bin"
BJORK_LISTEN_URL="$REPO_BASE/bjork-listen"
BJORK_CPP_URL="$REPO_BASE/bjork.cpp"
CMAKE_LISTS_URL="$REPO_BASE/CMakeLists.txt"

# setup
mkdir -p "$INSTALL_DIR"
TMP_DIR=$(mktemp -d)

echo "Downloading 'bjork-listen' script..."
curl -fsSL "$BJORK_LISTEN_URL" -o "$TMP_DIR/bjork-listen"
chmod +x "$TMP_DIR/bjork-listen"
mv "$TMP_DIR/bjork-listen" "$INSTALL_DIR/"

echo "Downloading bjork.cpp source and CMakeLists.txt..."
curl -fsSL "$BJORK_CPP_URL" -o "$TMP_DIR/bjork.cpp"
curl -fsSL "$CMAKE_LISTS_URL" -o "$TMP_DIR/CMakeLists.txt"

echo "Setting up CMake build (with libcurl)..."
mkdir "$TMP_DIR/build"
cd "$TMP_DIR/build"
cmake ..
cmake --build .

echo "Installing bjork binary..."
mv bjork "$INSTALL_DIR/"

# add to PATH if needed
if [[ ":$PATH:" != *":$INSTALL_DIR:"* ]]; then
  SHELL_CONFIG=""
  if [[ $SHELL == */bash ]]; then
    SHELL_CONFIG="$HOME/.bashrc"
  elif [[ $SHELL == */zsh ]]; then
    SHELL_CONFIG="$HOME/.zshrc"
  fi

  echo "Adding $INSTALL_DIR to your PATH..."
  if [[ -n "$SHELL_CONFIG" ]]; then
    echo 'export PATH="$HOME/.local/bin:$PATH"' >> "$SHELL_CONFIG"
    echo "Added to $SHELL_CONFIG. Run: source $SHELL_CONFIG"
  else
    echo "Couldn't auto-detect shell config. Add this manually:"
    echo 'export PATH="$HOME/.local/bin:$PATH"'
  fi
fi

# done
echo "Installation complete!"
echo "You can now run:"
echo "   bjork-listen g++ badcode.cpp"
echo "   bjork --explain"

# cleanup
rm -rf "$TMP_DIR"
