# Security Key

Development scripts for Raspberry Pi Pico security key project.

## Scripts

- `setup.sh` - Install dependencies and clone required repositories (pico-sdk, pico-examples, picotool)
- `build-picotool.sh` - Build the picotool utility
- `build.sh` - Build the firmware project
- `upload.sh` - Upload firmware to Pico in BOOTSEL mode and reboot
- `monitor.sh` - Monitor serial output from the device

## Usage

1. Run setup first: `./scripts/setup.sh`
2. Build picotool: `./scripts/build-picotool.sh`
3. Build firmware: `./scripts/build.sh`
4. Upload to device: `./scripts/upload.sh`
5. Monitor output: `./scripts/monitor.sh`

## Author

HaoVA.

## License

MIT.
