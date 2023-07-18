$ErrorActionPreference = "Stop"
wsl make compile
If ($?) {
  avrdude -v -p attiny10 -c usbasp -U fuse:w:0xfe:m -U flash:w:src/main.hex:i
}
