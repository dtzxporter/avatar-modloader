# avatar-modloader
Overrides unpacked assets from the game directory when they exist allowing you to mod the game easily.

### Usage:
- Download the mod from the releases page.
- Extract the files and copy them to the game folder next to `afop.exe`.
- Select "YES" to override `dbgcore.dll`.
- Copy any extracted or modded assets to the game's directory relative to the `afop.exe` and the game will prefer the on-disk copy over the packed one.

### Complete file list:
- 12/12/2023: [File List](https://mega.nz/file/ERgiUYRK#ZJm2ON8CGtsN1UWFwG3D_KqlPcsg5ctbMHdVGz8K8iU)

### Texture modding:
- In order to mod textures, the DDS needs to be converted to a 'DDS' with STF header.
- You need to reverse the mipmap order on disk.
- Then copy the proper STF header extracted from the game: [STF Headers](https://mega.nz/file/4chxAJBZ#GvW1ojCapMK--PtzPGr_B9g0xvIRRkSDEfTP8JZ9gUI).
- The extension is still `.dds`.

# Credits:
- Uses @Nukem9's Detours Library.
- DTZxPorter.
