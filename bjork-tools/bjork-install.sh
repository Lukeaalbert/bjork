#!/bin/bash
set -e

# config
REPO_BASE="https://raw.githubusercontent.com/lukeaalbert/bjork/bjork-tools"
INSTALL_DIR="$HOME/.local/bin"
BJORK_LISTEN_URL="$REPO_BASE/bjork-listen"
BJORK_CPP_URL="$REPO_BASE/bjork.cpp"

# setup
mkdir -p "$INSTALL_DIR"
TMP_DIR=$(mktemp -d)

echo "Downloading 'bjork-listen' script..."
curl -fsSL "$BJORK_LISTEN_URL" -o "$TMP_DIR/bjork-listen"
chmod +x "$TMP_DIR/bjork-listen"
mv "$TMP_DIR/bjork-listen" "$INSTALL_DIR/"

echo "Downloading and compiling bjork tools..."
curl -fsSL "$BJORK_CPP_URL" -o "$TMP_DIR/bjork.cpp"
g++ -std=c++17 "$TMP_DIR/bjork.cpp" -o "$INSTALL_DIR/bjork"

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
    echo 'Export PATH="$HOME/.local/bin:$PATH"' >> "$SHELL_CONFIG"
    echo "Added to $SHELL_CONFIG. Run: source $SHELL_CONFIG"
  else
    echo "Couldn't auto-detect shell config. Add this manually:"
    echo 'Export PATH="$HOME/.local/bin:$PATH"'
  fi
fi

# done
echo "Installation complete!"
echo "You can now run:"
echo "   bjork-listen g++ badcode.cpp"
echo "   bjork --explain"

# cleanup
rm -rf "$TMP_DIR"
