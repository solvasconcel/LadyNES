LadyNES (fork of LaiNES)
======

A friendly fork of LaiNES, a compact and cycle-accurate NES emulator, now featuring a modern Bento-style launcher, native file dialogs, and customizable controls.

## Features
- **Modern Bento Launcher**: Clean interface with recent ROMs, and organized GUI acessible settings.
- **Native File Dialogs**: Uses Native File Dialog Extended (NFD) for native file browsing and selection instead of the LaiNES menu.
- **Customizable Controls**: Intuitive keybinding window for keyboard and joystick configuration
- **Dynamic Window Title**: Shows current game name in window title
- **Savestate Support**: Save and load states with full emulator state preservation
- **Multiple Mapper Support**: Compatible with most common NES mappers
- **Expansion Audio**: VRC6 audio support
- **Fast Forward**: About 8x speed with Tab key

## Requirements
LadyNES should run on any Unix system compatible with the following tools:
- SCons (build system)
- C++14 compatible compiler (e.g., clang++)
- SDL2 (including sdl2-ttf and sdl2-image)
- dbus-1 (for Native File Dialog Portal mode on Linux)

## Building and running
Install the dependencies:
```sh
# Arch Linux:
sudo pacman -S clang scons sdl2 sdl2_image sdl2_ttf dbus

# Debian-based systems:
sudo apt-get install clang scons libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev libdbus-1-dev

# Mac OS X:
brew install scons sdl2 sdl2_image sdl2_ttf
```

Clone and build:
```sh
git clone --recursive <your-repo-url> && cd LadyNES
scons -j4
./LadyNES
```

## Usage
LadyNES features a modern Bento-style launcher with easy ROM selection, recent ROMs list, and organized settings.

### Launcher Features
- **Logo Display**: Shows LadyNES logo on startup
- **Recent ROMs**: Quick access to recently played games (persistent across sessions)
- **Native File Dialog**: Click "Select ROM" to browse with native system dialog (NFD)
- **Settings**: Access video, audio, and control settings
- **Controller Setup**: Click the 🎮 button to configure keyboard and joystick bindings

### Gameplay Controls
- **ESC**: Toggle between gameplay and launcher
- **Tab** (hold): Fast forward mode (8x speed, indicated by ">>" in top-right corner)

### Keybinding Configuration
Access the controller configuration window (🎮 button) to customize:
- **Keyboard bindings**: Set keys for Player 1 and Player 2 (Up, Down, Left, Right, A, B, Start, Select)
- **Joystick bindings**: Configure button mappings for connected controllers
- Uses SNES9x-style interface with tabs for easy configuration

### Savestate Support
LadyNES includes savestate functionality accessible through the launcher (**Save State** and **Load State** options). Savestates preserve the complete emulator state including CPU, PPU, APU, mapper state, and expansion audio. Savestates are automatically stored per-ROM and can be loaded at any time.

### Window Features
- **Dynamic Title**: Window title shows "LadyNES - [Game Name]" when a game is loaded
- **Resizable**: Window size is customizable (default 2x = 512x480)
- **Fullscreen**: Toggle fullscreen mode from settings
- **Multiple Controllers**: Supports keyboard and joystick input for two players

## Compatibility
LadyNES implements the most common mappers, providing good compatibility with a wide range of NES games:
- NROM (Mapper 000)
- MMC1 / SxROM (Mapper 001)
- UxROM (Mapper 002)
- CNROM (Mapper 003)
- MMC3, MMC6 / TxROM (Mapper 004)
- MMC5 / ExROM (Mapper 005)
- AxROM (Mapper 007)
- MMC2 / PxROM (Mapper 009)
- MMC4 / FxROM (Mapper 010)
- Color Dreams (Mapper 011)
- VRC6 (Mapper 024) - with expansion audio support
- BNROM / NINA-001 (Mapper 034)
- GxROM (Mapper 066)

You can check the compatibility for each ROM in the following list:
http://tuxnes.sourceforge.net/nesmapper.txt

## Technical notes
The 6502 CPU and the PPU were originally implemented in just 219 and 283 lines of code respectively, though they have since grown to include full unofficial opcode support and enhanced accuracy.
Meta-programming is used extensively to keep the codebase compact.

### Key Technical Features:
- **Cycle-accurate emulation**: CPU runs at 1.79 MHz, PPU at 5.37 MHz (3x CPU speed)
- **Template-based instruction decoding**: Compact opcode implementation using C++ templates
- **Native File Dialogs**: Integrated NFD with Portal mode for Linux (requires dbus-1)
- **Modern UI**: ImGui-based Bento launcher with dark theme
- **Dynamic window management**: SDL2 window with runtime title updates
- **Input flexibility**: Supports both keyboard and joystick with runtime rebinding

Here is an example of the meta-programming approach used:
```c++
/* Cycle emulation.
 *     For each CPU cycle, we call the PPU thrice, because it runs at 3 times the frequency. */
#define T   tick()
inline void tick() { PPU::step(); PPU::step(); PPU::step(); ... }
...

/* Addressing modes.
 *     These are all the possible ways instructions can access memory. */
typedef u16 (*Mode)(void);
inline u16 imm() { return PC++; }
...
inline u16 zpx() { T; return (zp() + X) % 0x100; }
...

/* Fetch parameter.
 *     Get the address of the opcode parameter in a, and the value in p. */
#define G  u16 a = m(); u8 p = rd(a)
...

/* Instruction emulation (LDx where x is in registers {A, X, Y}).
 *     upd_nz, not shown, just updates the CPU flags register. */
template<u8& r, Mode m> void ld() { G; upd_nz(r = p); }
...

/* Execute a CPU instruction.
 *     Opcodes are instantiated with the right template parameters
 *     (i.e. register and/or addressing mode).*/
void exec()
{
    switch (rd(PC++))  // Fetch the opcode.
    {
        // Select the right function to emulate the instruction:
         ...
         case 0xA0: return ld<Y,imm>();  case 0xA1: return ld<A,izx>();
         ...
    }
}
```

## Known issues
* If you're experiencing audio issues on Linux, try typing `export SDL_AUDIODRIVER=ALSA` before running the emulator.
* NFD Portal mode requires dbus-1 to be installed and running.

## Contributors
* [Jeff Katz](https://github.com/kraln) - Major enhancements including:
  - Additional mapper support: MMC5 (005), VRC6 (024), MMC2/MMC4 (009/010), and mappers 002, 003, 007, 011, 034, 066
  - VRC6 expansion audio support
  - Savestate system with full mapper/audio state preservation
  - Comprehensive accuracy improvements: CPU timing, PPU sprite overflow, APU IRQ, DMA timing, NMI timing
  - Complete unofficial 6502 opcode support
  - Fast forward mode and enhanced menu navigation
  - Configuration saving system
* [PudgeMa](https://github.com/PudgeMa) - Scrollable menu and bug fixes.
* [tyfkda](https://github.com/tyfkda) - Show error message instead of segfault for unsupported mappers.
* **LadyNES Fork** - Additional features:
  - Modern Bento-style launcher with logo display
  - Native File Dialog Extended (NFD) integration
  - SNES9x-style keybinding configuration window
  - Dynamic window title with current game name
  - Recent ROMs system with persistence
  - Improved UI/UX with dark theme

## References and credits
- Special thanks to [Ulf Magnusson](https://github.com/ulfalizer) for the invaluable [PPU diagram](http://wiki.nesdev.com/w/images/d/d1/Ntsc_timing.png) and for his [excellent implementation](https://github.com/ulfalizer/nesalizer) which was a big source of inspiration.
- blargg's APU sound chip emulator: http://blargg.8bitalley.com/libs/audio.html#Nes_Snd_Emu
- Complete hardware reference: http://problemkaputt.de/everynes.htm
- Tick-by-tick breakdown of 6502 instructions: http://nesdev.com/6502_cpu.txt
- [Dear ImGui](https://github.com/ocornut/imgui) - Immediate mode GUI for C++
- [Native File Dialog Extended](https://github.com/btzy/nativefiledialog-extended) - Native file dialogs for Linux, Windows, and macOS
* If you're experiencing audio issues on Linux, try typing `export SDL_AUDIODRIVER=ALSA` before running the emulator.
* NFD Portal mode requires dbus-1 to be installed and running.

## Contributors
* [Jeff Katz](https://github.com/kraln) - Major enhancements including:
  - Additional mapper support: MMC5 (005), VRC6 (024), MMC2/MMC4 (009/010), and mappers 002, 003, 007, 011, 034, 066
  - VRC6 expansion audio support
  - Savestate system with full mapper/audio state preservation
  - Comprehensive accuracy improvements: CPU timing, PPU sprite overflow, APU IRQ, DMA timing, NMI timing
  - Complete unofficial 6502 opcode support
  - Fast forward mode and enhanced menu navigation
  - Configuration saving system
* [PudgeMa](https://github.com/PudgeMa) - Scrollable menu and bug fixes.
* [tyfkda](https://github.com/tyfkda) - Show error message instead of segfault for unsupported mappers.
* **LadyNES Fork** - Additional features:
  - Modern Bento-style launcher with logo display
  - Native File Dialog Extended (NFD) integration
  - SNES9x-style keybinding configuration window
  - Dynamic window title with current game name
  - Recent ROMs system with persistence
  - Improved UI/UX with dark theme

## References and credits
- Special thanks to [Ulf Magnusson](https://github.com/ulfalizer) for the invaluable [PPU diagram](http://wiki.nesdev.com/w/images/d/d1/Ntsc_timing.png) and for his [excellent implementation](https://github.com/ulfalizer/nesalizer) which was a big source of inspiration.
- blargg's APU sound chip emulator: http://blargg.8bitalley.com/libs/audio.html#Nes_Snd_Emu
- Complete hardware reference: http://problemkaputt.de/everynes.htm
- Tick-by-tick breakdown of 6502 instructions: http://nesdev.com/6502_cpu.txt
- [Dear ImGui](https://github.com/ocornut/imgui) - Immediate mode GUI for C++
- [Native File Dialog Extended](https://github.com/btzy/nativefiledialog-extended) - Native file dialogs for Linux, Windows, and macOS
