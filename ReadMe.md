# Libra

Top-down tank playground built on the **Jumble** engine. It's a place to experiment with tile effects and the World / TileMap / Body split from the school Libra project (SD1), not a finished game.

## Building
- Open `Libra.slnx` in Visual Studio 2026 (toolset v145). The engine project lives in the sibling repo at `../Jumble/Code/Engine/Jumble.vcxproj`, so both repos must be checked out side by side under the same parent folder.
- Build Debug or Release, x64 or x86. A post-build step copies the exe (`Libra_<Config>_<Platform>.exe`) into `Run/`, and the debugger runs it from there.
- Audio is turned off in `Code/Game/EngineBuildPreferences.hpp`, so no FMOD DLL is needed. Delete that define when you want sound.

## Controls
The window title shows the current mode, the brush, and messages such as "Saved". Jumble has no text rendering yet.

| Mode | Action | Keys |
|---|---|---|
| Both | Toggle play / edit | Tab |
| Both | Save / load `Run/Data/Maps/Playground.txt` | F6 / F9 |
| Play | Drive (8-way, the hull turns toward the heading) | E S D F |
| Play | Aim turret | I J K L |
| Play | Fire | Space |
| Edit | Pick brush | 1–7, or click a swatch |
| Edit | Paint / erase to grass | Left / right mouse |
| Edit | Flood-fill the hovered region | F |
| Edit | Eyedropper | Q |

### Debug keys
- **P** pause, **O** step one frame, **T** (hold) 10% speed
- **F8** rebuild the game (reloads the map from disk)
- **Esc** quit

## Tiles
Defined in one table in `Code/Game/TileDefinition.cpp`. Add a row there and it shows up in the palette.

| Tile | Glyph | Effect |
|---|---|---|
| Grass | `.` | Normal drag |
| Stone | `#` | Blocks tanks and bullets |
| Brick | `B` | Blocks tanks and bullets; a bullet breaks it |
| Mud | `m` | High drag |
| Ice | `i` | Almost no drag, so the tank slides |
| Sand | `s` | Medium drag |
| Water | `~` | Blocks tanks, but bullets fly over it |

Bullets bounce off walls up to 3 times. The area outside the map counts as stone.

## How it's put together
- **TileMap**: just the grid. It batches all tiles into one vertex array and rebuilds it only after an edit. It also handles map files (one text row per tile row, top row first).
- **Tank**: owns a Jumble `Body` and reads input. It knows nothing about tiles.
- **World**: owns the map, the tank and the bullets. Each frame it updates the entities, then pushes the tank out of solid neighbors and eases the tank's drag toward the drag of the tile under it.
- **TilePalette**: the edit-mode tool. **Game** switches modes and owns the cameras.
