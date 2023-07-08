# N64FlashcartMenu
**This repo and its documentation is currently experimental and subject to change without notice.**

## Aims
* Support as many N64 FlashCarts as possible.
* Be open source using preferably MIT licensed third party licensed libraries where possible.
* Be testable, where possible using CTest, but if not, in an emulated environment.
* Encourage active development from community members and N64 FlashCart owners.
* Support [PixelFX N64Digital](https://gitlab.com/pixelfx-public/n64-game-id)


## Basic usage

### SC64
Download the `sc64menu.n64` ROM from the latest action run assets.
Add it to the root folder on your SD card.

(Optional):
Create a file called `config.sc64.toml.txt`
Add the following content, replacing the comments.
```
[last_rom]
    rom_path = "<!-- path to your ROM from the root of your SD card, note the quotes -->"
    save_path = "<!-- path to a save file from the root of your SD card, note the quotes -->"
    save_type = <!-- a number representing the save type (see save types) -->
    auto_load = <!-- a boolean value of `true` or `false` -->
    save_writeback = false

[last_state]
    auto_load_last_rom = false
    current_directory = "sd://"

[boot_params]
    device_type = 0;
    reset_type = 1;
    detect_tv_type = true;
    detect_cic_seed = true;
```
Save it to the root folder on your SD card.

#### Save types
`0` = NONE
`1` = EEPROM_4K
`2` = EEPROM_16K
`3` = SRAM
`4` = SRAM_BANKED
`5` = SRAM_128K
`6` = FLASHRAM
~~`15` = CART_HEADER~~


### ED64
Not currently not supported, but there is an aim to do so.
The aim is to replace [Altra64](https://github.com/networkfusion/altra64) and [ED64-UnofficialOS](https://github.com/n64-tools/ED64-UnofficialOS-binaries).


# Developer documentation
**Work in progress!**

You can use a dev container in VSCode to ease development.


## To deploy:

* Download the deployer [here](https://github.com/Polprzewodnikowy/SummerCart64/releases/download/v2.16.0/sc64-deployer-windows-v2.16.0.zip)
* Extract and place `sc64deployer.exe` in the `tools/sc64` directory.

Make sure that your firmware is compatible (currently v2.16.0+)
See: https://github.com/Polprzewodnikowy/SummerCart64/blob/v2.16.0/docs/00_quick_startup_guide.md#firmware-backupupdate



### From the devcontainer
It is not currently possible to directly communicate with USB devices.
BUT, as a work around you can use a proxy TCP/IP connection
Set up a proxy: open a terminal window, `cd ./tools/sc64` and then `./sc64deployer.exe server`

Then in the dev container, use `./remotedeploy.sh`


### From your host (Windows) OS

* Run `./localdeploy.bat` from the terminal


Toggle the N64 power switch to load the ROM.

`ms-vscode.makefile-tools` will help (installed automatically in dev container).
TODO: it does not yet work with `F5`: see https://devblogs.microsoft.com/cppblog/now-announcing-makefile-support-in-visual-studio-code/
WORKAROUND: in the dev container terminal, use make directly, i.e.: `make`
The ROM can be found in the `output` directory.

NOTE: a "release" version of the SC64 menu is called `sc64menu.n64` and can be created for when you want to add it directly to the SDCard. This is generated by running `make all` or running `make sc64_minify`.

# Update Libdragon submodule
This repo currently uses the `unstable` branch as a submodule at a specific commit.
To update to the latest version, use `git submodule update --remote ` from the terminal.

# Generate documentation
Run `doxygen` from the dev container terminal.
Make sure you fix the warnings before creating a PR!
