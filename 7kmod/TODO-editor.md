# 7kmod-editor: editor de escenarios

Rama `7kmod-editor`, basada en `7kmod`, para que los escenarios funcionen en las tres
versiones del mod (`7kaa-mod.exe`, `7kaa-gameplay.exe` y `7kaa-pathing.exe`).

**Estado:** diseño en refinamiento. Todavía no hay nada implementado.

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

- [ ] **Fase 1: Save as Scenario.** Guardar la partida actual en `SCENMOD` con su `.SCT`
  (pedir nombre, dificultad, bonus y descripción) y su `.SCP` (generado del minimapa).
  Agregar la carpeta `SCENMOD` a la lista de escenarios. Con los trucos actuales ya
  permite crear escenarios.
- [ ] **Fase 2: Modo editor sobre el mapa.** Pausa, sin niebla, barra superior, paleta
  de colocar (unidades, pueblos, edificios, recursos, guaridas), selección, borrar,
  cambiar de dueño, propiedades de lo seleccionado. Botón Probar.
- [ ] **Fase 3: Reinos, objetivos y datos del escenario.** Las ventanas Escenario,
  Reinos y Objetivos.
- [ ] **Fase 4: Acceso desde la pantalla Scenario.** Botones New y Edit; mapa nuevo con
  el generador actual (tamaño, semilla, terreno).
- [ ] **Fase 5 (opcional): Pintar terreno.** Altura, agua, bosques, rocas, con
  recálculo de regiones al salir del editor.
- **Fuera de alcance por ahora:** disparadores y eventos ("si pasa X, aparece Y"). El
  juego no tiene sistema de eventos; sería un proyecto aparte (ver `IDEAS.md` si se
  retoma).

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

- Nombre de la carpeta (`SCENMOD`) y la marca en la lista.
- Qué propiedades se editan de cada cosa seleccionada.
- Si el editor permite mapas de cualquier tamaño o solo el máximo.
