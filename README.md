# 7kmod

Mod de **Seven Kingdoms: Ancient Adversaries** basado en la versión de código abierto
**7kaa 2.15.7** de [7kfans](https://7kfans.com/) (tag `v2.15.7` de este repositorio).

El mod agrega mejoras de interfaz y comodidad, un editor de escenarios dentro del juego y,
en ramas aparte, cambios de reglas, de pathfinding y de IA.

> **Desarrollo con IA.** Este mod se desarrolla con ayuda de inteligencia artificial
> (Claude, de Anthropic): buena parte del código y de la documentación la escribió la IA
> siguiendo mis indicaciones, y yo reviso, pruebo y decido cada cambio. Los commits
> hechos así llevan la línea `Co-Authored-By: Claude`.

## Ramas

Cada rama incluye todo lo de la anterior (se actualizan con merge, nunca con rebase):

```
master  ──>  7kmod  ──>  7kmod-gameplay  ──>  7kmod-pathing  ──>  7k-mod-IA
```

| Rama | Ejecutable | Qué tiene | Changelog |
|---|---|---|---|
| `master` | — | El juego original 7kaa 2.15.7, sin cambios. Sirve de referencia. | — |
| **`7kmod`** (principal) | `7kaa-mod.exe` | Mejoras de interfaz y comodidad, correcciones y el **editor de escenarios**. No cambia el estilo de juego. | [`7kmod/CHANGELOG.md`](7kmod/CHANGELOG.md) |
| `7kmod-gameplay` | `7kaa-gameplay.exe` | Todo `7kmod` más **cambios de reglas**: comercio por puerto, Fryhtans invocados con el color de su reino, la IA usa Fryhtans, etc. | `7kmod/CHANGELOG-gameplay.md` (en esa rama) |
| `7kmod-pathing` | `7kaa-pathing.exe` | Todo `7kmod-gameplay` más el **pathfinding experimental**. | `7kmod/CHANGELOG-pathing.md` (en esa rama) |
| `7k-mod-IA` | `7kaa-ia.exe` | Todo `7kmod-pathing` más la **IA experimental** y límites por reino (población, unidades militares, impuestos), todo como opciones de `config.txt`. Usa su propia carpeta de configuración (`%APPDATA%\7kfans.com\7kaa-ia\`). | `7kmod/CHANGELOG-ia.md` (en esa rama) |

- Las ramas experimentales (`7kmod-pathing` y `7k-mod-IA`) pueden cambiar mucho o romper cosas.
- Cada rama guarda partidas con su propia versión de archivo: no abras en una rama las
  partidas guardadas con otra.
- Los ejecutables de Windows se publican en **Releases**, no dentro del repositorio.

## Documentación

Todo está en la carpeta [`7kmod/`](7kmod/), en español:

- `CHANGELOG*.md`: cambios por versión; cada rama tiene el suyo además de los de las
  ramas anteriores.
- `TODO*.md`: pendientes, ideas y el detalle de cómo se hizo cada cambio
  (`TODO-editor.md` describe el editor de escenarios).
- `ADVANCED_CONFIG.md`: opciones de `config.txt`.
- `KEYBINDINGS.md`: atajos de teclado.

## Compilar

Se compila igual que 7kaa: ver [`README.source`](README.source) y el
[`README`](README) original del juego.

## Licencia

Igual que el juego original: **GNU GPL versión 2** (ver [`COPYING`](COPYING)).
Seven Kingdoms es de Trevor Chan y Enlight Software, liberado como código abierto por
7kfans. Este mod no está afiliado con ellos.
