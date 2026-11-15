# PlaygroundTopDown2D

Top-down 2D playground. It's an empty ProtoGame2D-style template on the **Jumble** engine, for playing around.

## Building
- Open `PlaygroundTopDown2D.slnx` in Visual Studio 2026 (toolset v145). The engine project lives in the sibling repo at `../Jumble/Code/Engine/Jumble.vcxproj`, so both repos must be checked out side by side under the same parent folder.
- Build Debug or Release, x64 or x86. A post-build step copies the exe (`PlaygroundTopDown2D_<Config>_<Platform>.exe`) into `Run/`, and the debugger runs it from there, so `Data/...` paths resolve against `Run/`.
- Audio is turned off in `Code/Game/EngineBuildPreferences.hpp`, so no FMOD DLL is needed. Delete that define when you want sound (and put `fmod64.dll` in `Run/`).

## Layout
- `Main_Windows.cpp`: `WinMain` creates the `App`, then runs Startup, the main loop and Shutdown.
- `App`: creates the Jumble `Engine` and the `Game`, runs the frame (BeginFrame → Update → Render → EndFrame) and owns the game clock.
- `Game`: the empty game, with a world camera (200x100 world units) and a screen camera (1600x800).
- `GameCommon`: the `g_theApp` / `g_theRenderer` / `g_theInput` / `g_theAudio` / `g_theWindow` shortcuts and world/screen size constants.

## Debug keys
- **P** pause, **O** step one frame, **T** (hold) 10% speed
- **F8** restart the game
- **Esc** quit

## History
The first commit holds an earlier prototype, kept for reference: a tank playground with a tile-palette editor (as Libra).
