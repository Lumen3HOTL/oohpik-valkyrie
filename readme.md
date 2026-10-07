# Ookpik - Custom Dragonfly-Based... Rogue-Like?

**Team Mustelid**
Lana Cagle (lqcagle@wpi.edu)
Joshua Majors (jmajors@wpi.edu)

Ookpik is a turn-based roguelike built on our own C++ implementation of the Dragonfly game engine.
The engine was primarily developed by Joshua during his Project 2 submissions, but was fleshed out by the full team
for this project.
You guide an owl through a randomly generated forest,
collecting seeds and reaching the exit to move on to a new map, without flying into a tree.
The game keeps track of the number of seeds collected, moves made, and maps completed across runs, with your goal
being to collect the most seeds in the fewest moves across as many maps as possible. 
A large part of the game's difficulty is oriented around its odd control scheme.

---

## Platform

| | |
|---|---|
| OS | Windows 10/11, 64-bit |
| IDE / compiler | Visual Studio 2026 (v18) Community |

---

## How to play

| Key | Action |
|---|---|
| **W** or **Space** | Hop forward one tile in the direction you face |
| **A** | Turn left 90° |
| **D** | Turn right 90° |
| **Esc** | Quit |

| Symbol | Meaning |
|---|---|
| `^` `>` `v` `<` | The owl (you), pointing the way it faces |
| `#` (green) | Tree. Moving into one kills you |
| `@` (yellow) | Seed. Walk over it to collect it |
| `E` (cyan) | Exit. Walk onto it to generate a new map |

- The **top row** is for UI. It shows seeds collected, seeds remaining on this map, moves, and maps completed.
- Every key press that does something (move or turn) counts as a move.
- Hitting a tree kills the player, ending the run, and shows a death screen with your moves, seeds collected and maps reached.
  After about a second, press any key to quit.
- Each map is freshly generated, stats are tracked between maps.

---

## How to build and run

### 1. Install SFML 3.1 and set up comaptibility
Download SFML 3.1 for **Visual C++ 64-bit** from sfml-dev.org and unzip it so the folder is named `SFML-3.1`.
The build looks for it in this order (see `SFML.props`):

1. The `SFML_DIR` environment variable, if set (point it at the `SFML-3.1` folder)
2. `SFML-3.1` **next to** the repository folder
3. `SFML-3.1` **one level above** the repository folder

*Note: This structure comes from attempting to support both of our SFML installs during development but should be robust
for all installation. Hopefully you will not have any issues! As far as I can tell this should work pretty conveniently for
Windows 11 users. -Lana*

```
code-folder/
|-- SFML-3.1/
|-- oohpik-valkyrie/
```

### 2. Open and build
1. Open `transmission/MakeADragonFly.slnx` in Visual Studio Community 2026
2. Select your desired build branch and `x64` in the toolbar.
3. In Solution Explorer, right-click `oohpik` -> **Set as Startup Project**.
4. Build and run

The engine library builds automatically first. After building, the SFML DLLs, the engine font (`df-font.ttf`)
and the game's `Resources/` folder are copied next to the executable, so the game also runs by double-clicking
`oohpik/oohpik/bin/x64/Debug/oohpik.exe`.

#### 2.5. Build Branches
There are two different build branches, `Release` and `Debug`. The release branch builds the actual Game.cpp contents for play,
while the Debug branch builds the EngineTests project, which runs tests on the engine and game files. The test suite is currently
not properly updated to reflect the current state of the engine, so it is not recommended to run it unless you are working on the engine itself.

### Running the engine tests
Set `EngineTests` as the startup project and run it. It runs from `transmission/` (where its test sprite files are)
and writes results to `transmission/dragonfly.log`.

### Logs
The game writes `dragonfly.log` next to `oohpik.exe`. Map generation errors are logged there.

---

## Code structure

### Solution
| Project | Output | Purpose |
|---|---|---|
| `MakeADragonFly` | static `.lib` | The engine |
| `EngineTests` | `.exe` | Engine unit/integration tests |
| `oohpik` | `.exe` | The game |

Both executables link the engine library, so engine changes are picked up automatically when either is built.

### Map generation (`MapBuilder`)
Generation happens in two phases so the game window stays responsive:

1. **Plan, worker thread** (`generateMap`): validates the config, starts from a grid of trees, and carves out
   rooms and paths using shapes. It makes sure there is enough open space, scatters extra trees,
   finds separate open areas and joins them with paths so the map can be traversed, then places
   seeds, the exit and the owl's start. It gives a logged error if it exceeds the configured timeout.
2. **Build, main thread during step events** (`buildMap`): takes the plan and turns them into engine objects
   and moves the owl to its start tile. 

`Ground` objects for open tiles are currently disabled (`CREATE_GROUND_OBJECTS` in `MapBuilder.cpp`): they are
invisible and only cost frame time. They are mostly legacy code but could come in handy down the line.

---

## Known issues and limitations

TODO: confirm and trim this list before submission.

- **The player can move while a new map is still generating** (input isn't locked during generation)
- **Rare unreachable seeds**
- **Tests out of date**, testing has mostly been done manually

---

### AI use
See `AIUSE.md`
