# 7kmod

**English** | [Español](README.es.md)

A mod of **Seven Kingdoms: Ancient Adversaries** based on the open source version
**7kaa 2.15.7** by [7kfans](https://7kfans.com/) (tag `v2.15.7` in this repository).

The mod adds interface and quality of life improvements, an in-game scenario editor and,
in separate branches, rule changes, pathfinding changes and AI changes.

> **AI-assisted development.** This mod is developed with the help of artificial
> intelligence (Claude, by Anthropic): much of the code and documentation was written by
> the AI following my directions, and I review, test and decide on every change. Commits
> made this way carry the line `Co-Authored-By: Claude`.

## Branches

Each branch includes everything in the previous one (they are updated with merge, never
with rebase):

```
master  ──>  7kmod  ──>  7kmod-gameplay  ──>  7kmod-pathing  ──>  7k-mod-IA
```

| Branch | Executable | What it has | Changelog |
|---|---|---|---|
| `master` | — | The original game, 7kaa 2.15.7, unchanged. Kept as a reference. | — |
| **`7kmod`** (main) | `7kaa-mod.exe` | Interface and quality of life improvements, fixes and the **scenario editor**. Does not change how the game plays. | [`7kmod/CHANGELOG.en.md`](7kmod/CHANGELOG.en.md) |
| `7kmod-gameplay` | `7kaa-gameplay.exe` | All of `7kmod` plus **rule changes**: harbor trade, invoked Fryhtans with their kingdom's color, the AI uses Fryhtans, etc. | `7kmod/CHANGELOG-gameplay.en.md` (in that branch) |
| `7kmod-pathing` | `7kaa-pathing.exe` | All of `7kmod-gameplay` plus the **experimental pathfinding**. | `7kmod/CHANGELOG-pathing.en.md` (in that branch) |
| `7k-mod-IA` | `7kaa-ia.exe` | All of `7kmod-pathing` plus the **experimental AI** and per-kingdom limits (population, military units, taxes), all as `config.txt` options. Uses its own settings folder (`%APPDATA%\7kfans.com\7kaa-ia\`). | `7kmod/CHANGELOG-ia.en.md` (in that branch) |

- The experimental branches (`7kmod-pathing` and `7k-mod-IA`) may change a lot or break
  things.
- Each branch saves games with its own file version: don't open in one branch the games
  saved with another.
- The Windows executables are published in **Releases**, not inside the repository.

## Installation

1. Install **Seven Kingdoms: Ancient Adversaries 2.15.7** ([7kfans.com](https://7kfans.com/)).
2. Download the executable of the branch you want from **Releases** and copy it to the
   game folder, next to `7kaa.exe`. Start the game with it.

## Documentation

Everything is in the [`7kmod/`](7kmod/) folder. The player documents have an English
version with the same name ending in `.en.md`:

- `CHANGELOG*.en.md`: changes by version; each branch has its own besides those of the
  previous branches.
- `ADVANCED_CONFIG.en.md`: `config.txt` options.
- `KEYBINDINGS.en.md`: keyboard shortcuts.
- `GREATER_BEINGS.en.md`, `SPYING.en.md`, `SIEGE_WEAPONS.en.md`: how Greater Beings,
  spying and siege weapons work.
- `TODO*.md` and the analysis documents (Spanish only): pending work, ideas and the
  details of how each change was made (`TODO-editor.md` describes the scenario editor).

## Building

Built the same way as 7kaa: see [`README.source`](README.source) and the original
game's [`README`](README).

## License

Same as the original game: **GNU GPL version 2** (see [`COPYING`](COPYING)).
Seven Kingdoms is by Trevor Chan and Enlight Software, released as open source by
7kfans. This mod is not affiliated with them.
