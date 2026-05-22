export PATH=$PATH:~/.platformio/penv/bin/
rm -rf .pio
# compile and upload firmware to target
pio run -t upload
# compile and run receiver application
# you want to install sdl2 and libserialport via Homebrew on mac. use choco on Windows.
cmake -S ./renderer/ -B ./renderer/build
cmake --build ./renderer/build
./renderer/build/tinyrtos_gui_renderer
