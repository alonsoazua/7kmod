# Changelog de 7kmod-fryhtan (experimental)

[English](CHANGELOG-fryhtan.en.md) | **Español**

Rama experimental para jugar como un **reino Fryhtan**. Está basada en `7kmod-gameplay`
e incluye todo lo de esa rama y de `7kmod` (ver `CHANGELOG-gameplay.md` y
`CHANGELOG.md`). Su ejecutable es `7kaa-fryhtan.exe`. El plan y el detalle de cada
cambio están en `TODO-fryhtan.md`.

La traducción al inglés está en `CHANGELOG-fryhtan.en.md`; los dos se actualizan juntos.

---

## [Sin publicar]

Versión **0.1.0** en desarrollo, basada en **7kmod-gameplay 0.2.0** (7kmod 0.3.0).

### Agregado
- **Reino Fryhtan** (fase 1 de `TODO-fryhtan.md`; solo partidas de un jugador):
  - En New Game, página básica, el botón **Fryhtan** (a la derecha de Maya) y el selector
    de especie **< Rokken >** (a la derecha de Zulu), con las 13 especies. Debajo del
    nombre se ve cómo quedará el reino, por ejemplo "Fryhtan kingdom: Al Rokken Horde
    (Large: 2 population)". Elegir una raza vuelve a un reino humano.
  - El reino empieza **solo con su rey**, un All High de la especie con combate 60 y
    liderazgo 60.
  - Se llama "<nombre> <especie> Horde", también en las noticias.
  - **Sin reputación** (queda en 0) y sus Fryhtans **sin lealtad**.
  - **Sin edificios humanos:** sus unidades no tienen menú de construcción.
  - Los aldeanos de pueblos independientes no se unen a un reino Fryhtan.
  - Por ahora el reino no puede hacer nada más: las guaridas llegan en la fase 2.

### Compatibilidad
- **Carpeta propia:** `%APPDATA%\7kfans.com\7kaa-fryhtan\` (partidas, `CONFIG.DAT`,
  `7KMOD.DAT` y `config.txt`), aparte de los demás ejecutables del mod.
- **Partidas guardadas:** versión 310 (`GAME_VERSION_7KMOD_FRYHTAN`): guarda la especie
  de cada reino. Carga las partidas de gameplay (versión 300 o anterior). Los demás
  ejecutables del mod no cargan las partidas de esta rama.
- **Multijugador:** la opción Fryhtan no existe en multijugador.
