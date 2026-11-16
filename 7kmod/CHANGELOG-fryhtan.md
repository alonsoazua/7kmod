# Changelog de 7kmod-fryhtan (experimental)

[English](CHANGELOG-fryhtan.en.md) | **Español**

Rama experimental para jugar como un **reino Fryhtan**. Está basada en `7kmod-gameplay`
e incluye todo lo de esa rama y de `7kmod` (ver `CHANGELOG-gameplay.md` y
`CHANGELOG.md`). Su ejecutable es `7kaa-fryhtan.exe`.

La traducción al inglés está en `CHANGELOG-fryhtan.en.md`; los dos se actualizan juntos.

---

## [Sin publicar]

---

## [0.1.0] - 2026-11-14

Incluida en **New Adversaries 0.4.0** (gameplay 0.3.0, 7kmod 0.4.0). La rama
`7kmod-fryhtan` sigue con los ajustes experimentales del reino Fryhtan.

### Agregado
- **Reino Fryhtan** (fase 1; solo partidas de un jugador):
  - En New Game, página básica, dos pestañas de texto encima de las razas: **Humans**
    (las 10 razas) y **Fryhtans** (las 13 especies en el mismo lugar, 10 por página, con
    flechas ▲ ▼ para cambiar de página); la elegida va subrayada. Debajo
    del nombre se ve cómo quedará el reino, por ejemplo "Fryhtan kingdom: Al Rokken
    Horde". La pestaña y la especie se recuerdan.
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
  - Botón **Train** en la guarida: un Fryhtan de la especie de la guarida por **$200**
    (**$150** si la especie es pequeña) y **20 de comida**, en **15 días** (10 si es
    pequeña), con combate 20. Cola de hasta 8; clic derecho quita uno y devuelve el oro y
    la comida.
  - **Población Fryhtan** (en prueba, **sin límite**): cada Fryhtan del reino, rey
    incluido, usa 2 (pequeño) o 3 (grande), y el reporte de reinos muestra cuántos hay
    ("Fryhtan Population"). El tope (base 20 más la gente esclavizada) queda desactivado
    en el código (`FRYHTAN_POPULATION_LIMIT`); en su lugar, entrenar cuesta oro.
  - **Promote / Demote:** un soldado Fryhtan se asciende a **Ordo** (general) con
    Promote y recibe un **nombre propio** (por ejemplo "Kzorr"); conserva su combate y
    liderazgo. Demote lo regresa a soldado. Los soldados empiezan con **liderazgo 20** (se
    ve en su panel) y lo aprenden en la guarida como los humanos (solo los que tienen
    potencial).
  - El liderazgo de un Ordo sube **×1.5** más rápido que el de un general humano.
  - **Train Ordo** en la guarida: un Ordo por el mismo oro que un soldado de su especie y
    40 de comida, en 15 o 10 días, con liderazgo y combate 30.
  - **Precio por especie** (soldado y Ordo): **$100** Ick, Broosken y Doink; **$150**
    Deezboanz, Rattus y Pfith (Pfith cuenta como chica: población 2 y 10 días); **$200**
    las grandes.
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
    un mínimo que depende del liderazgo del general: **60** con liderazgo 30 o menos
    hasta **45** con 100 (cuenta la mejor guarida). El liderazgo también decide qué tan
    rápido. Para bajarla a 0 hay que
    pelear.
  - Un pueblo de un reino humano se esclaviza bajando la lealtad de su gente a 0
    peleando.
  - En los dos casos hace falta una guarida enlazada al pueblo.
  - El pueblo esclavizado no recluta, no entrena y no paga impuestos: paga un **tributo
    de $3 por persona al mes** ("Tribute from Enslaved Towns"), que sube con el liderazgo
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
- **Raid de pueblos independientes:** en un pueblo enlazado a una guarida con general,
  el botón **Raid** (ícono de Collect Tax, con barra de espera; oscurecido y con la razón
  en su ayuda si no se puede) abre una vista con la raza principal, la población, la
  resistencia, lo que da el Raid y los defensores, con los botones **Raid village** y
  **Cancel**. Una vez cada 2 meses por pueblo:
  - Roba **$2 y 2 de comida por persona** (hasta 60, es decir 30 personas), por
    (100 + liderazgo) / 100 del general de la guarida que hace el Raid: el doble con
    liderazgo 100. Baja la resistencia 5.
  - Salen hasta 1 defensor por cada 10 personas, más 1 (y +1 con resistencia mayor de
    50, +2 mayor de 80), y atacan la guarida del mejor general. El primero es el líder:
    combate +20 y se lleva el doble de resistencia. Cada defensor que muere baja la
    resistencia del pueblo; los que regresan la devuelven.
  - Con resistencia de 30 o menos no se puede: "This village is weakened and ready to
    be attacked directly."
  - Por ahora no mata aldeanos.
  - Noticia con lo robado y los defensores.
- El **All High del inicio** tiene liderazgo **100** (antes 60).
- **Las construcciones del reino se reparan solas**, como si tuvieran un constructor de
  skill 20: 1 punto de vida cada 5 días, salvo mientras las atacan o con el oro en
  negativo.
- **Ninguna unidad humana se une al reino Fryhtan:**
  - Los espías humanos no pueden disfrazarse de un reino Fryhtan (ni en su menú ni los
    de la IA, que tampoco los mandan a pueblos esclavizados). Antes un espía de la IA
    salía de un pueblo disfrazado de tu reino con el mensaje de que una unidad se había
    unido.
  - Un espía enemigo que estaba en un pueblo que esclavizaste sale de ahí como unidad
    independiente, y si deja de ser espía se queda como aldeano.
  - Las unidades que traicionan, los rebeldes sin líder y los aldeanos de la propaganda
    no se pasan a un reino Fryhtan, y los aldeanos de un pueblo esclavizado no le
    entregan un edificio humano (captura por trabajadores).
- **Ordo dentro de un pueblo esclavizado:** con clic derecho sobre un pueblo esclavizado,
  un Ordo entra en él (uno por pueblo). El botón de espionaje (el ojo) del pueblo abre su
  vista, donde la fila **Role** con el botón **>** cambia entre tres acciones:
  - **Subjugate:** la lealtad no baja del liderazgo del Ordo (tope 50).
  - **Kidnap:** llegan **2 aldeanos** nuevos de las razas del pueblo, en promedio cada 8
    meses con liderazgo 30 y cada 4 con 100 (nunca más de 12 meses), con la noticia "Your
    Ordo … has kidnapped 2 villagers into your village of …".
  - **Taskmaster:** cada aldeano da de +$0.5 a +$2 más al mes y de +5 a +20 de comida más
    al año, según el liderazgo.
  - Adentro, el Ordo **gana liderazgo y combate** como un general en su guarida (cada 5
    aldeanos cuentan como un soldado, hasta 8) y recupera vida.
  - **Leave village** lo saca. Si el pueblo cambia de dueño, el Ordo sale.
  - Con un Ordo seleccionado, sobre el pueblo se ve la flecha de entrar.
  - El nombre del pueblo en el mapa y en el reporte de pueblos muestra el rol, por
    ejemplo "Rouen (Subjugate)".
- **Corregido:** al morir un Ordo, la noticia mostraba texto basura en lugar del
  nombre. Ahora dice "Your Ordo, <nombre>, has been slain."; también se corrigió el
  nombre en las noticias de asesinato, traición y honores.
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
  - **Botín:** matar una unidad humana da oro según su **nivel de combate**: un soldado
    $2 con combate 10 hasta **$45** con combate 100 (en línea recta); un defensor de
    pueblo la mitad; un general ×3 ($135 con combate 100); un rey ×22 (**$990** con
    combate 100), y los siguientes reyes del mismo reino la mitad. Un aldeano da $2. La
    comida es fija: 5 (soldado, aldeano o defensor), 20 (general) o 100 (rey). En el
    reporte de economía aparece como "Spoils of War and Treasure".
  - Los **edificios humanos** que pasen al reino Fryhtan (por ejemplo, si un reino se le
    rinde) se **destruyen**, con quien esté adentro.

### Compatibilidad
- **Carpeta propia:** `%APPDATA%\7kfans.com\7kaa-fryhtan\` (partidas, `CONFIG.DAT`,
  `7KMOD.DAT` y `config.txt`), aparte de los demás ejecutables del mod.
- **Partidas guardadas:** versión 310 (`GAME_VERSION_7KMOD_FRYHTAN`): guarda la especie
  de cada reino, la especie de cada guarida y su cola de Train. Carga las partidas de gameplay (versión 300 o anterior). Los demás
  ejecutables del mod no cargan las partidas de esta rama.
- **Multijugador:** la opción Fryhtan no existe en multijugador.
