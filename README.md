# Fork of cube.c with SDL2

An optimized, hardware-accelerated 3D spinning cube demo in standard C utilizing the SDL2 framework. This project adapts the classic terminal-based `cube.c` engine into a high-performance desktop window application.

<img width="960" height="720" alt="image" src="https://github.com/user-attachments/assets/ffba0be7-440e-460b-9190-7d2439b4b180" />

---

## Key Features

* **Pixel Buffer Rendering Pipeline:** Replaces slow sequential per-pixel draw operations with an inline-optimized engine writing directly to a memory frame block, which is uploaded to an `SDL_Texture` in one unified GPU block copy.
* **Encapsulated State Engine:** Eliminates volatile global scopes by bundling rotational mechanics, perspective limits, and viewport configurations into a dedicated structural handle (`EngineState`).
* **Cross-Platform Resolution Scaling:** Uses `SDL_RenderSetLogicalSize` to handle crisp, pixelated upscaling across resizable window footprints automatically.

---

## Dependencies

Before compiling, ensure you have the **SDL2** development libraries installed on your machine.

* **Ubuntu/Debian:** `sudo apt install libsdl2-dev`
* **macOS (Homebrew):** `brew install sdl2`
* **Windows (MinGW):** Download the development library package directly from the [SDL GitHub Releases](https://github.com/libsdl-org/SDL/releases).

---

## Building and Running

You can compile this project using either `CMake` or standard GNU `make`.

### Option 1: Using the Makefile (Recommended for a quick run)
To compile using the cross-platform platform-detecting `Makefile`:
```bash
make
./cube_demo
