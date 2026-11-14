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
  - En New Game, página básica, dos pestañas de texto encima de las razas: **Humans**
    (las 10 razas) y **Fryhtans** (las 13 especies en el mismo lugar, 10 por página, con
    flechas ▲ ▼ para cambiar de página); la elegida va subrayada. Debajo
    del nombre se ve cómo quedará el reino, por ejemplo "Fryhtan kingdom: Al Rokken
    Horde (Large: 3 population)". La pestaña y la especie se recuerdan.
  - Los nombres de las especies, las pestañas y la etiqueta "Seats of Power Unlocked?"
    usan letra suavizada (TeX Gyre Pagella, tipo Palatino), prerenderizada con
    `7kmod/tools/make_smooth_text.py`.
  - El reino empieza con su rey, un All High de la especie con combate 60 y
    liderazgo 100.
  - Se llama "<nombre> <especie> Horde", también en las noticias.
  - **Sin reputación** (queda en 0) y sus Fryhtans **sin lealtad**.
  - **Sin edificios humanos:** sus unidades no tienen menú de construcción.
  - Los aldeanos de pueblos independientes no se unen a un reino Fryhtan.
- **Guaridas** (fase 2):
  - El rey y los generales Fryhtan tienen **Build → Lair**: una guarida de su especie
    que funciona como un fuerte (un general y hasta 8 soldados). Cuesta **$500** y
    **$100 al año**.
  - Botón **Train** en la guarida: un Fryhtan de la especie de la guarida por **20 de
    comida**, en **20 días** (10 si la especie es pequeña), con combate 20. Cola de hasta 8; clic derecho quita uno y
    devuelve la comida. No se puede pasar del tope de población.
  - **Población Fryhtan:** cada Fryhtan del reino, rey incluido, usa 2 (pequeño) o 3
    (grande). El tope base es 20. Se ve en la ayuda de Train y en el reporte de reinos
    ("Fryhtan Population").
  - **Train Ordo:** un segundo botón entrena un general (Ordo) de la especie de la
    guarida: **40 de comida**, **40 días** (20 si es pequeña), liderazgo y combate **30**. Sale junto a la
    guarida con un **nombre propio** (por ejemplo "Kzorr"). Los soldados ya no se pueden
    ascender (no hay Promote ni Demote).
  - Los Fryhtans entrenan su combate en la guarida con el general y ganan experiencia
    al pelear.
  - La guarida no tiene Reward ni Recruit; sus Fryhtans muestran lealtad "N/A".
- **Guaridas por especie:** la guarida que construye un Ordo es de la especie de ese
  Ordo (por ejemplo, el de una guarida salvaje capturada), y Train entrena la especie de
  la guarida.
- En el mapa **no hay guaridas salvajes de la especie del jugador**.
- Al terminar de entrenar un Fryhtan suena el **"ready"** de las unidades humanas.
- **Inicio:** el reino empieza con el rey como general de una guarida ya construida y
  4 soldados de su especie, y con un **pueblo esclavizado de 20 personas** de una raza
  al azar, enlazado a la guarida (donde un reino humano tiene su pueblo inicial). Con
  el rey en la guarida, paga tributo desde el primer mes; el tope de población empieza
  en 40.
- **El rey** se llama "<nombre> The All High" en su panel y en el de la guarida (por
  ejemplo "Al The All High"), sobre "All High <especie>".
- El rey y los Ordos llevan el **ícono de rey o de general** sobre la unidad.
- En la **lista de partidas** guardadas, una partida Fryhtan muestra el retrato del All
  High de su especie y "<nombre> The All High".
- Un reino Fryhtan **no puede dar dinero** (Grant) a pueblos independientes.
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
    de $5 por persona al mes** ("Tribute from Enslaved Towns"), que sube con el liderazgo
    del mejor general de las guaridas enlazadas (el doble con liderazgo 100). Produce comida, crece, no
    se rebela y su gente no se va.
  - Cada persona esclavizada sube en 1 el **tope de población** Fryhtan.
  - **Periodo de gracia:** durante el primer mes después de esclavizarlo, el pueblo no
    se puede rebelar, para que el general tenga tiempo de regresar a la guarida (la
    lealtad empieza en 30 y sin general baja).
  - Si pierde el enlace con todas las guaridas, se vuelve independiente.
  - Al destruir o vender una guarida ya no aparece un fuerte humano.
- **Cierre** (fase 7):
  - **Sucesión:** si muere el All High, el Ordo con más combate pasa a ser el nuevo
    All High (si no hay Ordos, el Fryhtan con más combate) y el reino toma su nombre
    ("Zhovrek The All High", "Zhovrek Wyrm Horde").
  - **Derrota:** el reino pierde solo cuando ya no tiene guaridas ni Fryhtans.
  - Fin de partida con textos "Your Horde has …" e imágenes propias (los Fryhtans si
    ganas, los humanos celebrando si pierdes). El Hall of Fame muestra "<nombre> The
    All High".
  - Reportes: "Lair" con su costo en Villages, "In Lair" en Military, y los Fryhtans
    cuentan en el puntaje de población del Ranking.
  - Al colocar una guarida se ven las **líneas hacia las guaridas salvajes** que quedarían
    enlazadas.
- **Diplomacia y Fryhtans salvajes** (fase 6):
  - Con un reino Fryhtan no hay comercio, amistad, alianza, embargo, compra de comida ni
    rendición. Sí hay guerra, cese al fuego, tributo, ayuda y tecnología.
  - Los reinos de la IA no se rinden ante un reino Fryhtan.
  - Los **Fryhtans salvajes no atacan** al reino Fryhtan (sí se defienden si los atacas).
- **Pueblos esclavizados sin general:** si ninguna guarida enlazada tiene un Ordo o el
  rey adentro, el pueblo no paga tributo y su lealtad baja. Si llega al punto de
  rebelión, los rebeldes salen a atacar la guarida más cercana.
- **Saqueo de pueblos:** al atacar un pueblo, cada aldeano que muere da $2 y 5 de comida,
  como matar a un aldeano.
- **Raid de pueblos independientes:** un panel en el pueblo enlazado a una guarida con
  general (como el de capturar guaridas salvajes), con el botón **Raid village**, una vez
  cada 2 meses por pueblo y con barra de espera:
  - Roba $1 y 1 de comida por persona (hasta 30) y baja la resistencia 5.
  - Salen hasta 1 defensor por cada 10 personas, más 1 (y +1 con resistencia mayor de
    50, +2 mayor de 80), y atacan la guarida del mejor general. El primero es el líder:
    combate +20 y se lleva el doble de resistencia. Cada defensor que muere baja la
    resistencia del pueblo; los que regresan la devuelven.
  - Con resistencia de 30 o menos no se puede: "This village is weakened and ready to
    be attacked directly."
  - Por ahora no mata aldeanos.
  - Noticia con lo robado y los defensores.
- El **All High del inicio** tiene liderazgo **100** (antes 60).
- **Pueblos esclavizados sin guarida:** un pueblo esclavizado que se queda sin ninguna
  guarida enlazada sigue siendo del reino **3 meses** (sin tributo y sin rebelarse); si
  en ese tiempo vuelve a tener una guarida enlazada, sigue esclavizado. Al terminar se
  independiza con la noticia "Your village of … has declared independence."
- **Corregido:** el pueblo que se independizaba se rendía enseguida a otro reino, porque
  conservaba resistencia 0 hacia él de antes. Ahora queda con la resistencia de un
  pueblo independiente nuevo (60 a 100) hacia todos los reinos, y 100 hacia el reino
  Fryhtan.
- **Capturar guaridas salvajes** (fase 5):
  - Una guarida salvaje de cualquier especie a 8 casillas o menos de una guarida del
    reino muestra una **oferta**: un Ordo con el combate del mejor Ordo de la guarida y
    liderazgo 30–60, y 4–6 soldados (especie grande) o 6–8 (pequeña) con combate 20–40.
  - El tributo base es la suma de los combates × $10, y lo ajusta el liderazgo del mejor
    general de tus guaridas enlazadas: ×1 con liderazgo 100 y ×3 con 30 o menos. Sin un
    Ordo o el All High en una guarida enlazada no se puede pagar.
  - Con **Pay Tribute** la guarida pasa al reino con ese Ordo y esos soldados; los
    Fryhtans salvajes de adentro se van y los de afuera siguen salvajes. Se puede pasar
    del tope de población.
  - Una **línea de enlace** une tus guaridas con las guaridas salvajes enlazadas.
  - Cada guarida capturada entrena su propia especie.
- **Economía** (fase 3):
  - **Botín:** matar una unidad humana da **$2 y 5 de comida** (soldado, aldeano o
    defensor de un pueblo), **$10 y 20** (general) o **$100 y 100** (rey). En el reporte
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
