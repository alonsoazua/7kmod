# 7kmod-editor: editor de escenarios

Rama `7kmod-editor`, basada en `7kmod`, para que los escenarios funcionen en las tres
versiones del mod (`7kaa-mod.exe`, `7kaa-gameplay.exe` y `7kaa-pathing.exe`).

**Estado:** Fases 0 a 4 implementadas (MVP, guardar, colocar y editar, mover y meter unidades, ventanas, Edit). Fase 5: hechas 5a (árboles y rocas) y 5b (tipo de suelo); queda 5c (agua, tierra y colinas). El resto del diseño sigue en refinamiento.
Ejecutable: `7kaa-editor.exe`. Cambios en `CHANGELOG-editor.md`.

---

## Idea general

Un escenario (`.SCN`) es una **partida guardada**: `Game::run_scenario`
(`src/OGAMSCEN.cpp`) lo carga con `game_file.load_game`. Por eso el editor no necesita
un formato nuevo: se arma una partida con herramientas de edición y se guarda con el
guardado normal, junto con su texto (`.SCT`) y su imagen (`.SCP`).

- Editor **integrado en el juego** (se descartó una app aparte porque tendría que
  reproducir todo el formato del guardado y se rompería con cada cambio del mod).
- Se reutilizan las funciones que ya crean unidades, pueblos, edificios y recursos (los
  trucos de depuración de `src/OSYS.cpp` lo hacen: `m` recursos, `-` terminar edificios,
  `;`/`q`/`w` población, `z` mover unidades).
- Se reutilizan los botones, fuentes y paneles del juego (`Button`, `ButtonCustom`,
  `vga_util.d3_panel_*`, `font_san`, el campo de texto `GetA`), para que se vea igual
  que el resto del juego.

### Archivos de un escenario

| Archivo | Contenido |
|---|---|
| `X.SCN` | La partida guardada. |
| `X.SCT` | Texto: nombre, `Difficulty:N`, `Bonus:N`, una línea separadora y la descripción. |
| `X.SCP` | La imagen de la pantalla de escenarios (`put_large_bitmap` en `src/OGAMSCE2.cpp`). |
| `config.txt` | Opcional, por carpeta (`config_adv.scenario_config`). |

---

## Acceso

- **Pantalla "Scenario"** (Single Player → Scenario, `src/OGAMSCE2.cpp`): agregar dos
  botones junto a **Start** y **Cancel**:
  - **New:** abre el editor con un mapa nuevo, con la configuración de la última partida
    (`CONFIG.DAT`) y una semilla al azar, sin pasar por la pantalla de "New Game". Las
    opciones se cambian con el botón **New** del editor (ver "Mapa nuevo" en las fases).
  - **Edit:** abre en el editor el escenario seleccionado.
  - Los botones del juego son imágenes (`START-U`, `CANCEL-U`); los nuevos serían
    botones con texto, con la fuente del juego.
- No se cambia el menú de un jugador: es una sola imagen y sus opciones son zonas fijas
  (`single_player_option_array`, `src/OGAMMAIN.cpp`).
- **Carpeta propia:** los escenarios del editor se guardan en una tercera carpeta (por
  ejemplo `SCENMOD`; hoy `MAX_SCENARIO_PATH` es 2: `SCENARIO` y `SCENARI2`). Los
  originales nunca se sobrescriben: editar uno original guarda una copia en `SCENMOD`.
- En la lista aparecen con los demás (ordenados por dificultad), con una marca, por
  ejemplo "(mod)".
- Se juegan como cualquier escenario: se seleccionan y Start.
- Opcional: "Save as Scenario" en el menú de una partida en curso, y un parámetro de
  línea de comandos (`-editor archivo.SCN`) para desarrollo.

---

## Interfaz

Se usa la misma distribución de la pantalla del juego (800×600): mapa a la izquierda
(576×544), minimapa arriba a la derecha y panel de información abajo a la derecha
(`INFO_X1..X2`, unos 205×325 px).

```
┌──────────────────────────────────────────────────────────────┐
│ [Escenario] [Reinos] [Objetivos]   Pintar como: [■ Reino 2 ▼]│ ← barra superior
│ [Probar] [Guardar] [Salir]                  $ 5,000    3,000 │   (reemplaza los reportes)
├────────────────────────────────────────┬─────────────────────┤
│                                        │     minimapa        │
│                                        │                     │
│             mapa normal                ├─────────────────────┤
│                                        │[Unid][Pueb][Edif][Nat]│ ← pestañas
│  · clic izq.: colocar lo elegido       │  [⚔] [🏹] [⛵]       │ ← paleta
│  · clic der.: seleccionar / cancelar   │  Raza:  [Maya   ◄►] │
│  · Supr: borrar lo seleccionado        │  Nivel: [30     ◄►] │ ← opciones de lo
│  · vista previa verde/roja             │  ─── seleccionado ──│   que se coloca
│                                        │  Pueblo "Tula"      │ ← o propiedades
│                                        │  Población [40 ◄►]  │   de lo seleccionado
│                                        │  Dueño [Reino 1 ▼]  │
└────────────────────────────────────────┴─────────────────────┘
```

- **Modo editor:** el juego en pausa total (sin tiempo ni IA) y sin niebla; el mapa, el
  minimapa y la selección funcionan como siempre.
- **Panel derecho:** pestañas **Unidades** (soldados, armas, barcos, reyes, generales,
  espías), **Pueblos**, **Edificios** y **Naturaleza** (recursos, guaridas de Fryhtan;
  árboles y rocas si se hace la fase de terreno). Iconos que ya existen (botones de
  construir y entrenar, retratos). Debajo, las opciones de lo que se coloca y, si hay
  algo seleccionado, sus propiedades (dueño, población, vida, lealtad, espía...).
- **Colocar como construir:** la vista previa verde/roja de los edificios, para todo.
- **Barra superior:** reemplaza los botones de los reportes. **Pintar como** elige el
  reino dueño de lo que se coloca. **Probar** guarda una copia temporal, arranca el
  escenario y al salir regresa al editor.
- **Ventanas grandes** (pantalla completa sobre el mapa, como los reportes, con fondo
  translúcido):
  - **Escenario:** nombre, descripción, dificultad, bonus.
  - **Reinos:** una fila por reino: color, raza, rey, IA/humano, dinero, comida,
    reputación; botones para sus tecnologías y relaciones.
  - **Objetivos:** las opciones de la pantalla de configuración de partida (destruir
    reinos, población, economía, límite de tiempo...).

---

## Fases

- [x] **Fase 0: MVP (acceso, interfaz, probar y salir).**
  - **Acceso:** botón **New** en la pantalla Scenario (arriba de "Your Name"); lleva a la
    pantalla de opciones de New Game y de ahí al editor con el mapa generado (el mismo
    contenido que una partida nueva). **Edit** aparece oscurecido.
  - **Modo editor:** pausa forzada (las teclas de velocidad no hacen nada y no aparece
    "Game Paused"), todo el mapa visible sin tocar lo explorado (el editor solo lo
    **dibuja** visible: `loc_reveal_all_flag` en `Location::explored()` y
    `visibility()`), sin órdenes a las unidades, solo funcionan F11, F12 y Esc.
  - **Barra superior** sobre los botones de los reportes: Scenario, Kingdoms, Goals y
    Save oscurecidos; **Test (F12)** y **Exit** funcionan; ayuda al dejar el ratón.
  - **Panel derecho:** pestañas Units, Towns, Buildings y Nature (por ahora con un texto
    de lo que vendrá) y **Selected:** el nombre y el reino de lo seleccionado. Clic para
    seleccionar; clic derecho o Esc para quitar la selección.
  - **Test:** guarda `EDITOR.TMP` (en la carpeta de configuración) y juega normal, con la
    niebla y los objetivos elegidos. **F12** o **Return to Editor** (en lugar de
    "Training" en el menú del juego) cargan la copia y regresan al editor como estaba.
    Durante la prueba, "Load Game" está desactivado.
  - **Exit:** pregunta y regresa al menú principal.
  - Menú del juego en el editor: Save Game, Load Game, Training y Retire desactivados.
  - Si la partida de prueba termina (victoria o derrota), se regresa al menú principal.
- [x] **Fase 1: Guardar el escenario.** El botón **Save** del editor abre la ventana
  "Save Scenario" y guarda los tres archivos en `SCENMOD`.
  - **Carpeta:** `SCENMOD` dentro de la carpeta de configuración del juego (la de las
    partidas guardadas del mod: `%APPDATA%\7kfans.com\7kaa-mod\SCENMOD\`). Se crea sola.
    Es la tercera carpeta de escenarios (`MAX_SCENARIO_PATH` 3,
    `ModEditor::init_scenario_dir()`).
  - **Ventana:** nombre (hasta 40 caracteres), dificultad (1 a 10, como los originales),
    bonus (0 a 500, de 50 en 50), descripción (6 líneas de hasta 75 caracteres; Enter o
    flecha abajo pasa a la siguiente) y el nombre del archivo que se va a usar.
  - **Archivo:** sale del nombre: letras y números en mayúsculas, `-` en lugar de los
    espacios, hasta 20 caracteres (`Mi Escenario` → `MI-ESCENARIO.SCN`). No se usa `_`
    porque los textos del juego lo toman como subrayado. Si ya existe un escenario con ese
    archivo (y no es el que se guardó en esta sesión), pregunta antes de reemplazarlo.
  - **`.SCT`:** nombre, `Difficulty:N`, `Bonus:N`, la línea separadora y las líneas de
    la descripción (sin las vacías del final).
  - **`.SCP`:** la franja central (758×142) de la vista del mapa al guardar, dibujada en
    dos partes para cubrir el ancho, con todo visible y sin selección.
  - **`.SCN`:** la partida guardada tal como se jugará (con la niebla y lo explorado
    reales, no lo que muestra el editor).
  - Los datos de la ventana se recuerdan mientras el editor está abierto (también al
    probar y volver).
  - En la lista de escenarios aparecen con los demás, ordenados por dificultad, con
    "(mod)" después del nombre. Se juegan con Start como cualquier escenario.
  - Pendiente para otras fases: el botón **Scenario** (editar estos datos sin guardar)
    sigue desactivado, y **Edit** (abrir un escenario guardado) es de la fase 4.
- [x] **Fase 2: Colocar y editar en el mapa** (panel derecho, `src/ModEditorPanel.cpp`).
  - **Reino de lo que se coloca:** arriba del panel, `< ■ Reino >`, incluye
    "Independent". Va en el panel y no en la barra superior porque la barra ya está llena.
  - **Units:** Soldier y General (opciones Race y Combat, de 10 en 10; el general tiene
    liderazgo igual a su combate), las 6 armas y los 4 barcos. Necesitan un reino.
  - **Towns:** Town (opciones Race y Population, de 5 en 5). Con "Independent" es un
    pueblo independiente con la resistencia normal; con un reino, lealtad 80.
  - **Buildings:** Fort, Mine, Factory, Inn, Market, Tower of Science, War Factory y
    Harbor (opción Race, para la apariencia). Se colocan terminados y vacíos, sin cobrar
    su costo. Necesitan un reino.
  - **Nature:** Clay, Copper, Iron (opción Reserve, de 1,000 en 1,000, hasta 20,000) y
    Fryhtan Lair (opción Fryhtan: el tipo, solo los que tienen guarida).
  - **Colocar:** clic en una entrada de la paleta (otra vez para soltarla); en el mapa se
    ve el área con la marca del juego (con color: se puede; negra: no se puede); clic
    coloca, se pueden colocar varios seguidos; clic derecho o Esc deja de colocar.
    Mientras se coloca, los clics en el mapa no seleccionan.
  - **Lo seleccionado** (abajo del panel): nombre, reino, una propiedad con `< >`
    (Combat de una unidad, Population de un pueblo, Reserve de un recurso sin mina) y los
    botones **Set Owner** (le pone el reino de arriba) y **Delete** (también la tecla
    Supr). Con varias unidades seleccionadas, Set Owner y Delete aplican a todas.
  - Delete: los edificios se borran sin ruinas y las unidades de adentro salen; las
    guaridas se borran con sus Fryhtan; un recurso con mina no se borra; los reyes no se
    borran ni cambian de reino.
- [x] **Fase 3: Ventanas** (barra superior, `src/ModEditorWin.cpp` y `ModEditorSave.cpp`).
  - **Scenario:** la misma ventana de Save, con OK, para editar nombre, dificultad,
    bonus y descripción sin guardar. Cancel deja los datos como estaban.
  - **Kingdoms:** una fila por reino con Treasure y Food (de 1,000 en 1,000) y
    Reputation (de 10 en 10, de −100 a 100). Clic en el nombre elige el reino de abajo:
    sus **tecnologías** (nivel de cada una con `< >`, botones "All technologies" y "No
    technologies") y sus **relaciones** con cada reino: clic avanza y clic derecho
    regresa entre No contact, War, Tense, Neutral, Friendly y Alliance (se aplica a los
    dos reinos). Se cierra con Close o Esc.
  - **Goals:** las metas de la pestaña Goals de New Game, con los mismos pasos y límites
    (Fryhtan, población, puntaje económico, puntaje total, años). Al cerrar se calcula la
    fecha límite como al empezar un escenario.
  - Las ventanas se dibujan sobre una copia del mapa en cada cambio, para que el fondo
    translúcido no se aclare.
- **Pendiente (no entró en las fases 2 y 3):** reyes, espías y Seat of Power (necesitan
  más lógica), agregar o quitar reinos y cambiar cuál es el del jugador (por ahora se
  eligen en la pantalla de New Game), y propiedades de los edificios.
- [x] **Fase 2b: Mover unidades y meterlas o sacarlas de edificios.** Todo al instante
  (el editor está en pausa, una orden normal de mover no se cumpliría hasta probar).
  - **Clic derecho cambia:** con unidades seleccionadas, clic derecho mueve o mete. Quitar
    la selección pasa a Esc o a un clic izquierdo en un lugar vacío, como en el juego.
  - **Mover:** clic derecho en el mapa pone las unidades seleccionadas ahí al instante
    (como el truco `z` de `src/OSYS.cpp`). Un grupo se acomoda alrededor del punto en
    casillas libres (`World::locate_space`, como al salir de un edificio). Barcos solo en
    agua, y en agua solo barcos.
  - **Meter:** clic derecho en un edificio o pueblo **de su mismo reino** (si no, primero
    Set Owner; el editor no mezcla reinos en un edificio):
    - **Fuerte:** un general (o el rey) y hasta 8 soldados. Si ya tiene general, el nuevo
      lo reemplaza y el anterior sale.
    - **Mina, fábrica, torre de ciencia, fábrica de armas:** solo trabajadores con la
      habilidad de ese edificio.
    - **Pueblo propio:** la unidad se vuelve habitante (como un aldeano que regresa).
    - Las que no quepan o no sirvan se quedan afuera, con un mensaje de cuántas no
      entraron.
    - Atajo: colocar un Soldier o General de la paleta sobre un fuerte de su reino lo mete
      directo.
  - **Worker en la paleta de Units:** nueva entrada con opción **Skill** (Mining,
    Manufacture, Research, Construction) y su nivel, para llenar minas, fábricas, torres
    de ciencia y fábricas de armas.
  - **Sacar:** al seleccionar un fuerte o edificio, el recuadro de lo seleccionado
    muestra su gente ("General: <nombre>", "Soldiers: 5 / 8", o los trabajadores) y los
    botones **Take out all** (todos salen alrededor del edificio) y **Take out general**
    (en fuertes).
  - Hecho. Notas de la implementación:
    - Los botones quedaron como **All out** y **General out** (u **Overseer out**) por el
      espacio.
    - Las reglas son las del juego (`Unit::firm_can_assign()`): en un fuerte también
      entran trabajadores (el juego permite entrenar a cualquier aldeano como soldado), y
      un general nuevo reemplaza al que está, aunque sea el rey (el anterior sale).
    - Si el edificio está lleno no se reemplaza a nadie: la unidad se queda afuera.
    - Clic izquierdo en un lugar vacío o Esc quitan la selección.
  - Para después: una lista dentro del panel para sacar uno por uno.
  - Fuera de esta fase: espías dentro de edificios enemigos (va con colocar espías).
  - Se usan las funciones del juego para asignar y sacar unidades (`Firm::assign_unit`,
    `assign_overseer`, `assign_worker`, `mobilize_worker`, `UnitArray::disappear_in_town`).
- [x] **Fase 4: Acceso desde la pantalla Scenario.** **New** (desde la fase 0) y **Edit**:
  abre en el editor el escenario seleccionado (`Game::editor_edit_scenario()`,
  `src/OGAMSCEN.cpp`) y lee su `.SCT` (nombre, dificultad, bonus y descripción). Al
  guardar, un escenario de SCENMOD se reemplaza; uno original se guarda como copia en
  SCENMOD con el nombre de archivo que sale de su nombre.
  - La descripción se acomoda en las líneas de la ventana, que ahora son **9** de 75
    caracteres (con 6 no cabían las de los escenarios originales). Al guardar, cada línea
    de la ventana queda como una línea del `.SCT`.
- [ ] **Fase 5 (opcional): Pintar terreno.** Dividido en tres
  partes de menor a mayor riesgo. Pestaña nueva **Terrain** en el panel, con pinceles de
  tamaño 1, 3 o 5 casillas (opción **Brush**); clic coloca, arrastrar no (por ahora).

  **Cómo está hecho el terreno del juego** (`src/OGENMAP.cpp`, `OGENHILL.cpp`,
  `OW_PLANT.cpp`, `OW_ROCK.cpp`):
  - El generador crea un mapa de alturas (plasma) y de él saca el tipo de terreno de cada
    esquina: mar, pasto oscuro, pasto claro, tierra oscura. Cada casilla guarda un
    `terrain_id`: un dibujo que une los tipos de sus 4 esquinas (`set_tera_id`,
    `TerrainRes::scan`). El mapa de alturas no se guarda.
  - Solo hay dibujos de transición entre tipos vecinos (mar–pasto oscuro, pasto oscuro–
    pasto claro, pasto claro–tierra): no se puede poner mar junto a tierra sin los
    anillos intermedios.
  - Encima van, por separado: **colinas** (`gen_hills`, bloquean el paso), **árboles**
    (`set_plant`, bloquean el paso, el juego los hace crecer y morir), **rocas**
    (`add_rock`, bloquean) y **manchas de tierra** (`add_dirt`, decorativas).
  - Las **regiones** (`set_region_id`) agrupan las casillas a las que se puede llegar por
    tierra o por mar; de ellas dependen el pathfinding, los barcos, los puertos
    (`set_harbor_bit`) y la IA (`region_array.init_region_stat`). Solo cambian si cambia
    qué es tierra y qué es agua.

  - [x] **5a: Árboles y rocas** (riesgo bajo). Entradas **Tree** (poner o quitar
    árboles, con la planta que corresponda al terreno, como `plant_init`), **Rock**
    (rocas de los tamaños del juego) y **Clear** (quita árboles, rocas y manchas en el
    área del pincel). No cambian las regiones. Solo en casillas sin unidades, edificios,
    pueblos ni recursos.
  - [x] **5b: Tipo de suelo** (riesgo medio). Entradas **Dark grass**, **Light grass** y
    **Dirt**: cambian el tipo de las esquinas del pincel y vuelven a elegir el
    `terrain_id` de las casillas tocadas y de las vecinas con `TerrainRes::scan`. Si dos
    tipos no tienen transición, se pinta el anillo intermedio automáticamente. Todo sigue
    siendo tierra: no cambian las regiones. No se puede pintar debajo de edificios ni
    pueblos.
  - [ ] **5c: Agua, tierra y colinas** (riesgo alto). Entradas **Water** (con su anillo
    de pasto oscuro a la orilla), **Land** y **Hill**. Cambian qué se puede caminar o
    navegar, así que al salir del modo de pintar (o al probar o guardar) se recalcula:
    banderas de cada casilla (`set_loc_flags`), regiones (`set_region_id`), puertos
    (`set_harbor_bit`), estadísticas de regiones (`region_array.init_region_stat`), el
    `region_id` de pueblos y edificios y las regiones de la IA (`update_ai_region`). Solo
    en áreas vacías (sin unidades, edificios, pueblos ni recursos).

  En las tres: después de pintar se vuelven a dibujar el mapa y el minimapa
  (`zoom_need_redraw`, `map_need_redraw`). Propuesta: hacer 5a y 5b primero y probarlas;
  5c después, solo si hace falta.

  **Cómo quedaron 5a y 5b** (`src/ModEditorTerrain.cpp`):
  - Pestaña **Terrain** (la sexta): **Trees**, **Clear**, **Small rock**, **Large rock**,
    **Dark grass**, **Light grass** y **Dirt**, con la opción **Brush** (1×1, 3×3, 5×5,
    centrado en la casilla). Las rocas no usan el pincel.
  - **Trees:** un árbol ya crecido del tipo de suelo en cada casilla libre del pincel
    (como `plant_init`). **Clear:** quita árboles, rocas (completas) y manchas de tierra
    en el pincel; al quitar una roca se vuelve a poder construir donde estaba (se quita su
    "power off", salvo en orillas del mapa, costas y junto a rocas o colinas).
  - **Rocas:** chica (hasta 2×2) o grande (hasta 4×4), del tipo de suelo, como
    `gen_rocks`; la marca es negra donde no caben.
  - **Suelo:** se reconstruye el nivel de cada esquina (tipo y subtipo) desde las casillas
    que la tocan. Revisé los 3 juegos de terreno: hay dibujo para cualquier casilla cuyas
    4 esquinas estén a 3 niveles o menos (de 12). Las esquinas del pincel toman el nivel
    medio del tipo elegido y las de alrededor lo siguen (hasta 4 anillos) para no pasar
    de 3 niveles: así salen solos los anillos de transición (por ejemplo, de pasto a
    tierra pasa por pasto claro). Si alguna casilla no tendría dibujo, no se pinta nada.
  - No cambian (y frenan al pincel) las esquinas de casillas con mar, colinas, edificios,
    pueblos, recursos naturales o casillas especiales. Así lo que se camina o navega no
    cambia y las regiones siguen igual. Los árboles que no crecen en el suelo nuevo se
    quitan.
  - Se ve en bloques (las transiciones siguen la cuadrícula de esquinas). Para después:
    pintar arrastrando el ratón.
- [x] **Mapa nuevo (New) sin la pantalla de New Game.**
  - **New** en la pantalla Scenario abre directo el editor con un mapa generado con la
    configuración actual (`CONFIG.DAT`, con las mismas correcciones que la pantalla de New
    Game: reinos de la computadora en 0 → 6, nivel de dificultad aplicado) y una semilla
    al azar (`Game::editor_new_scenario()`, `src/OGAMSING.cpp`).
  - Botón **New** en la barra del editor (segunda fila: **New**, **Test (F12)**,
    **Exit** y el texto "Editor"). Abre la ventana **New Map**, del mismo estilo que
    Kingdoms y Goals (`ModEditor::edit_new_map()`, `src/ModEditorWin.cpp`), con una copia
    de la configuración:
    - Izquierda: **Nationality**, **Your Color**, **Computer Kingdoms**, **Difficulty**
      (Level I a V y Custom, como `change_difficulty`), **Terrain** (Standard, Snow,
      Badlands), **Land Mass**, **Your Name**, **Map I.D.** (campo de texto) con el botón
      **Random Map I.D.**, y el **Difficulty Rating** calculado.
    - Derecha: **World Map**, **Fog of War**, **Human's Treasure**, **Computer's
      Treasure**, **AI Aggressiveness**, **Fryhtans**, **Random Kingdoms**, **Resources at
      Start**, **Resources Nearby?**, **Independent Villages**, **Village Resistance**,
      **New Villages Emerge?**, **New Kingdoms Emerge?**, **Random Events** y **Seats of
      Power** (Unlocked/Locked).
    - Cada opción con `<` y `>`; las listas dan la vuelta y los números se detienen en
      sus límites. Como en New Game, cambiar una opción avanzada (o los reinos de la
      computadora) pone la dificultad en Custom; raza, color, terreno, masa de tierra,
      nombre y Seats of Power no.
    - Las metas no están en la ventana: se quedan las de la ventana **Goals**.
    - **Create** pregunta "Create a new map? Unsaved changes will be lost."; con Yes
      guarda la configuración en `CONFIG.DAT`, termina el ciclo principal y
      `Game::editor_new_map()` genera el mapa con el Map I.D. (si no es un número, uno al
      azar). **Cancel** o Esc cierran sin cambiar nada.
  - El mapa nuevo borra los datos del escenario (nombre, descripción, dificultad, bonus y
    archivo), como un escenario nuevo. También funciona desde un escenario abierto con
    **Edit**.
- [ ] **Cambiar la raza (pendiente).** Objetivo: si haces un mapa y al final decides
  jugarlo con otra raza, debe ser fácil cambiarla. Fila **Race** (`<` `>`) en el cuadro
  de lo seleccionado; el cuadro crece unos 20 píxeles hacia arriba y la paleta se hace un
  poco más corta (las unidades ya tienen Combat y Leadership).
  - **Pueblo:** los aldeanos sin trabajo pasan a la raza nueva, con la misma población.
    Los que trabajan en edificios conservan su raza (son trabajadores del edificio).
  - **Edificio:** cambia su `race_id` y su dibujo (`firm_build_id` de esa raza). Si el
    dibujo de la otra raza ocupa otro tamaño, ese edificio no permite cambiar la raza.
    Soldados y trabajadores de adentro no cambian. Las guaridas no tienen raza.
  - **Unidades** (soldados, generales, trabajadores, aldeanos; también varias
    seleccionadas): la figura depende de la raza (`unit_id`), así que se crea una unidad
    nueva de esa raza en el mismo lugar y se quita la anterior. Conserva reino, rango,
    combate, liderazgo o habilidad y nivel; recibe un nombre de la raza nueva y queda
    seleccionada. No aplica a armas, barcos, Fryhtans ni unidades dentro de edificios.
  - **Rey:** la raza del rey es la del reino (`nation->race_id`): lealtad por raza,
    resistencia y rendición de pueblos independientes, reclutar, ícono en los reportes,
    IA, y el nombre del reino sale del nombre del rey. Cambiar la raza del rey cambia la
    del reino: se crea un rey nuevo de esa raza (mismo combate y liderazgo) y se pone
    como rey del reino. La lealtad se ajusta sola con el tiempo.
    - El rey **conserva su nombre** (y el reino, el suyo); solo cambian la figura y la
      raza.
    - Con **Seats of Power Unlocked**, el reino debe conocer el Seat of Power de la raza
      nueva en vez de la vieja.
- [ ] **Bajar la población sin el grito (pendiente).** El `<` de Population en el editor
  quita aldeanos sin el sonido de muerte (`Town::kill_town_people` llama a
  `se_res.sound(... "DIE")`); lo demás igual (primero los que no tienen trabajo, los
  espías se cuentan bien).
- [ ] **Mina: mineral y reserva (pendiente).** Al seleccionar una mina, el cuadro de lo
  seleccionado muestra **Mineral** (None, Clay, Copper, Iron) y **Reserve** con `<` `>`.
  - En el juego una mina toma su mineral y su reserva del sitio de mineral (`Site`) que
    tiene debajo; si no hay sitio, la mina no produce nada (`raw_id` 0). El editor los
    cambia juntos: la mina (`FirmMine::raw_id`, `reserve_qty`) y su sitio.
  - **Mina sin mineral:** se puede poner una mina en cualquier lugar. Al elegir un
    mineral, el editor crea el sitio debajo con la reserva base: la opción **Reserve** de
    la pestaña Nature (hoy 10,000 por omisión).
  - **Reserve:** de 1,000 en 1,000, hasta 20,000 (`MAX_RAW_RESERVE_QTY`, el máximo del
    juego).
  - **Cambiar de mineral** conserva la reserva (por ejemplo: Copper, se sube a 19,000,
    se cambia a Iron: queda Iron con 19,000). Lo que la mina ya extrajo y tiene guardado
    (`stock_qty`) se vacía.
  - **None** quita el sitio de debajo y la mina queda sin mineral.
  - Se actualizan las cuentas del reino y del mapa (`raw_count_array` del reino,
    `untapped_raw_count`, `has_mine` del sitio).
  - Hoy **Reserve** está desactivada en un sitio con mina; con esto se cambia desde la
    mina.
- [ ] **Menús de la barra del editor (pendiente).** Con una ventana abierta (Scenario,
  Kingdoms, Goals, Save o New), presionar otro botón de la barra cierra la ventana y abre
  la del botón presionado; presionar el mismo botón la cierra. Hoy hay que cerrarla
  primero. Las ventanas son ciclos propios (`edit_kingdoms`, `edit_goals`,
  `edit_scenario_info`, `edit_new_map`), así que cada una revisa los clics en la barra y
  devuelve qué botón se presionó; `detect_top_bar` abre la siguiente. Test y Exit
  también funcionan así (cierran la ventana y hacen lo suyo). Los cambios de una ventana
  con Cancel (Scenario, Save, New) se descartan al cambiar, como con Cancel.
- **Fuera de alcance por ahora:** disparadores y eventos ("si pasa X, aparece Y"). El
  juego no tiene sistema de eventos; sería un proyecto aparte (ver `IDEAS.md` si se
  retoma).

---

## Correcciones

- [x] **Armas colocadas sin la tecnología del reino** (visto en pruebas: cañones y
  unicornios a veces sin gráfico, sin atacar, y a veces el juego truena). En las armas,
  `nation_contribution` guarda la **versión del arma** (`get_weapon_version()`, I a III).
  `Unit::init_unit_id()` (`src/OUNIT.cpp`) solo la pone si el reino ya tiene esa
  tecnología; si no, queda en 0. Con versión 0, `Unit::fix_attack_info()` no elige el
  ataque de una versión: deja todos los ataques de todas las versiones, y al elegir el
  gráfico del ataque se sale de los datos. Por eso falla a veces: solo cuando el reino no
  tiene la tecnología del arma (en una partida nueva casi nunca la tiene). Es el mismo
  error que el juego original ya corrige en `FirmWar::change_nation()` (`src/OF_WAR.cpp`)
  para las fábricas de armas que cambian de reino.
  - Hecho: toda arma colocada recibe una versión (`set_weapon_version()` y
    `fix_attack_info()`), la de la opción **Version** (I, II, III) de la paleta, limitada
    a la versión máxima de esa arma (el nivel máximo de su tecnología).
  - Hecho: lo seleccionado muestra **Version** con `< >` en las armas.
  - También al cambiar de reino con Set Owner un arma que ya tiene versión no se afecta;
    revisar que ninguna otra ruta del editor cree armas con versión 0.

- [x] **El "Soldier" de la paleta sale como Peasant** (visto en pruebas). En el juego, un
  soldado es una unidad humana con la habilidad de **liderazgo** (`SKILL_LEADING`); sin
  habilidad es un civil (`Unit::is_civilian()`, `src/OUNIT.cpp`) y el panel lo muestra
  como "Peasant" (`src/OUNITIF.cpp`). El editor crea el soldado con su nivel de combate,
  pero sin habilidad (`place_object()`, `src/ModEditorPanel.cpp`); el General sí recibe
  liderazgo. Además de verse como Peasant, como civil no se comporta como soldado (por
  ejemplo, en la selección del ejército y al defenderse).
  - Corrección: al colocar un Soldier, darle `SKILL_LEADING` con un nivel, como los
    soldados que el juego crea al empezar una partida (`Battle::create_unit()`,
    `src/OBATTLE.cpp`).
  - **Decidido:**
    - Combate por defecto de Soldier y General: **20** (como un soldado entrenado).
    - Nueva opción **Leadership** en la paleta de Units, aparte de Combat, de 10 a 100 de
      10 en 10, por defecto **20**. Aplica a Soldier y General, para poder hacer, por
      ejemplo, un general de combate 100 y liderazgo bajo. El General deja de copiar su
      liderazgo del combate.
    - La paleta de Units queda con tres opciones (Race, Combat, Leadership): hay que
      acomodar el texto "Click on the map to place it." para que no se encime.
    - En **lo seleccionado**, las unidades humanas muestran Combat y también su habilidad
      (Leadership, o la de un trabajador) con `< >`; el recuadro de abajo crece un
      renglón.
  - Hecho como se decidió. Las armas y barcos pasaron a una pestaña nueva, **Weapons**
    (ahora son 5 pestañas en 3 columnas), para que quepan las opciones.

---

## Riesgos y puntos por revisar

- **Compatibilidad:** un `.SCN` del mod (versión de guardado 300) no abre en el juego
  original. Hecho en `7kmod-editor`, debe abrir en las tres ramas; hay que confirmar que
  `7kmod-gameplay` lee bien un guardado sin los datos de ModSpy.
- **Estado al cargar:** `run_scenario` aplica `config.txt` e `init_scenario_var`; revisar
  que no borren la niebla, los objetivos ni el dinero configurados en el editor.
- **IA:** los reinos creados en el editor deben quedar con su IA bien inicializada.
- **Terreno y regiones:** transiciones de terreno, colinas (`src/OGENHILL.cpp`) y el
  recálculo de regiones (`region_array.init`, `init_region_stat` en `src/OGENMAP.cpp`),
  del que dependen el pathfinding, los barcos y la IA.
- **Formato del `.SCP`:** confirmar el formato de `put_large_bitmap` para generarlo.
- **Interfaz:** es la mayor parte del trabajo (la del juego es de bajo nivel).

---

## Por definir

- Si el editor permite mapas de cualquier tamaño o solo el máximo.
