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
  - El reino empieza con su rey, un All High de la especie con combate 60 y
    liderazgo 60.
  - Se llama "<nombre> <especie> Horde", también en las noticias.
  - **Sin reputación** (queda en 0) y sus Fryhtans **sin lealtad**.
  - **Sin edificios humanos:** sus unidades no tienen menú de construcción.
  - Los aldeanos de pueblos independientes no se unen a un reino Fryhtan.
- **Guaridas** (fase 2):
  - El rey y los generales Fryhtan tienen **Build → Lair**: una guarida de su especie
    que funciona como un fuerte (un general y hasta 8 soldados). Cuesta **$500** y
    **$100 al año**.
  - Botón **Train** en la guarida: un Fryhtan de la especie de la guarida por **20 de
    comida**, en **20 días**, con combate 20. Cola de hasta 8; clic derecho quita uno y
    devuelve la comida. No se puede pasar del tope de población.
  - **Población Fryhtan:** cada Fryhtan del reino, rey incluido, usa 1 (pequeño) o 2
    (grande). El tope es 18. Se ve en la ayuda de Train y en el reporte de reinos
    ("Fryhtan Population").
  - **Promote:** un soldado Fryhtan se puede ascender a general (Ordo) con liderazgo 20.
  - Los Fryhtans entrenan su combate en la guarida con el general y ganan experiencia
    al pelear.
  - La guarida no tiene Reward ni Recruit; sus Fryhtans muestran lealtad "N/A".
- **Inicio:** el reino empieza con el rey como general de una guarida ya construida y
  4 soldados de su especie.
- **El rey** se llama "<nombre> The All High" en su panel y en el de la guarida (por
  ejemplo "Al The All High"), sobre "All High <especie>".
- **Barra de progreso** del Fryhtan en entrenamiento en el botón Train. Con el rey fuera,
  la guarida dice "Outside of the lair".
- **Pueblos esclavizados** (fase 4):
  - Una guarida con general enlazada a un pueblo independiente baja su resistencia hasta
    **45**; el liderazgo del general decide qué tan rápido. Para bajarla a 0 hay que
    pelear.
  - Un pueblo de un reino humano se esclaviza bajando la lealtad de su gente a 0
    peleando.
  - En los dos casos hace falta una guarida enlazada al pueblo.
  - El pueblo esclavizado no recluta, no entrena y no paga impuestos: paga un **tributo
    de $5 por persona al mes** ("Tribute from Enslaved Towns"). Produce comida, crece, no
    se rebela y su gente no se va.
  - Cada persona esclavizada sube en 1 el **tope de población** Fryhtan.
  - Si pierde el enlace con todas las guaridas, se vuelve independiente.
  - Al destruir o vender una guarida ya no aparece un fuerte humano.
- **Economía** (fase 3):
  - **Botín:** matar una unidad humana da **$5 y 5 de comida** (soldado, aldeano o
    defensor de un pueblo), **$20 y 20** (general) o **$100 y 100** (rey). En el reporte
    de economía aparece como "Spoils of War and Treasure".
  - Los **edificios humanos** que pasen al reino Fryhtan (por ejemplo, si un reino se le
    rinde) se **destruyen**, con quien esté adentro.

### Compatibilidad
- **Carpeta propia:** `%APPDATA%\7kfans.com\7kaa-fryhtan\` (partidas, `CONFIG.DAT`,
  `7KMOD.DAT` y `config.txt`), aparte de los demás ejecutables del mod.
- **Partidas guardadas:** versión 310 (`GAME_VERSION_7KMOD_FRYHTAN`): guarda la especie
  de cada reino, la especie de cada guarida y su cola de Train. Carga las partidas de gameplay (versión 300 o anterior). Los demás
  ejecutables del mod no cargan las partidas de esta rama.
- **Multijugador:** la opción Fryhtan no existe en multijugador.
