export PATH=$PATH:~/.platformio/penv/bin/
rm -rf .pio
pio run -t upload && pio device monitor