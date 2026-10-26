# 7kmod-editor: editor de escenarios

Rama `7kmod-editor`, basada en `7kmod`, para que los escenarios funcionen en las tres
versiones del mod (`7kaa-mod.exe`, `7kaa-gameplay.exe` y `7kaa-pathing.exe`).

**Estado:** Fases 0 a 3 implementadas (MVP, guardar, colocar y editar, ventanas). El resto del diseño sigue en refinamiento.
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
  - **New:** la pantalla de configuración de "New Game" (tamaño del mapa, terreno,
    reinos, semilla); en vez de jugar, abre el editor con ese mapa.
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
- [ ] **Fase 4: Acceso desde la pantalla Scenario.** Botones New y Edit; mapa nuevo con
  el generador actual (tamaño, semilla, terreno).
- [ ] **Fase 5 (opcional): Pintar terreno.** Altura, agua, bosques, rocas, con
  recálculo de regiones al salir del editor.
- **Fuera de alcance por ahora:** disparadores y eventos ("si pasa X, aparece Y"). El
  juego no tiene sistema de eventos; sería un proyecto aparte (ver `IDEAS.md` si se
  retoma).

---

## Correcciones

- [ ] **Armas colocadas sin la tecnología del reino** (visto en pruebas: cañones y
  unicornios a veces sin gráfico, sin atacar, y a veces el juego truena). En las armas,
  `nation_contribution` guarda la **versión del arma** (`get_weapon_version()`, I a III).
  `Unit::init_unit_id()` (`src/OUNIT.cpp`) solo la pone si el reino ya tiene esa
  tecnología; si no, queda en 0. Con versión 0, `Unit::fix_attack_info()` no elige el
  ataque de una versión: deja todos los ataques de todas las versiones, y al elegir el
  gráfico del ataque se sale de los datos. Por eso falla a veces: solo cuando el reino no
  tiene la tecnología del arma (en una partida nueva casi nunca la tiene). Es el mismo
  error que el juego original ya corrige en `FirmWar::change_nation()` (`src/OF_WAR.cpp`)
  para las fábricas de armas que cambian de reino.
  - Corrección: al colocar un arma, si la versión quedó en 0, ponerle la versión de la
    tecnología del reino o, si no la tiene, la versión I (`set_weapon_version()` y
    `fix_attack_info()`).
  - Opcional: una opción **Version** (I, II, III) en la paleta de Units para las armas,
    hasta la versión máxima de cada una.
  - También al cambiar de reino con Set Owner un arma que ya tiene versión no se afecta;
    revisar que ninguna otra ruta del editor cree armas con versión 0.

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
