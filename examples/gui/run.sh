MODE="both"

if [ "$1" = "-r" ]; then
  MODE="renderer"
elif [ "$1" = "-f" ]; then
  MODE="firmware"
fi

export PATH="$PATH:$HOME/.platformio/penv/bin"

if [ "$MODE" = "firmware" ] || [ "$MODE" = "both" ]; then
  rm -rf .pio
  pio run -t upload
fi

if [ "$MODE" = "renderer" ] || [ "$MODE" = "both" ]; then
  cmake -S ./renderer/ -B ./renderer/build
  cmake --build ./renderer/build
  ./renderer/build/tinyrtos_gui_renderer
fi
