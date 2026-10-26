# Changelog de 7kmod-editor

Cambios de la rama `7kmod-editor` (ejecutable `7kaa-editor.exe`), basada en `7kmod`. Lo
de `7kmod` está en `CHANGELOG.md`. El diseño y las fases están en `TODO-editor.md`.

---

## [Sin publicar]

### Agregado
- **Editor de escenarios, fase 0 (MVP):**
  - **Acceso:** Single Player → Scenario → botón **New** (arriba de "Your Name"). Lleva a
    la pantalla de opciones de New Game; al darle Start, el mapa se abre en el editor. El
    botón **Edit** está desactivado por ahora.
  - **Modo editor:** el juego queda en pausa (las teclas de velocidad no hacen nada) y se
    ve todo el mapa. Lo explorado no cambia: al probar el escenario se usa la niebla y el
    mapa elegidos. No se dan órdenes a las unidades; solo funcionan las teclas F11
    (captura), F12 (probar) y Esc (quitar la selección).
  - **Barra superior** en lugar de los botones de los reportes: **Scenario**,
    **Kingdoms**, **Goals** y **Save** (oscurecidos, para las siguientes fases),
    **Test (F12)** y **Exit**.
  - **Panel derecho:** pestañas **Units**, **Towns**, **Buildings** y **Nature** (por
    ahora solo con un texto de lo que vendrá) y lo seleccionado en el mapa (nombre y
    reino). Clic derecho o Esc quita la selección.
  - **Test (F12):** guarda una copia (`EDITOR.TMP`, en la carpeta de configuración del
    juego) y juega el escenario normal. **F12** o **Return to Editor** (en el lugar de
    "Training" en el menú del juego) regresan al editor tal como estaba antes de probar.
  - **Exit:** pregunta y regresa al menú principal.
  - En el editor, el menú del juego desactiva Save Game, Load Game, Training y Retire.
    Durante la prueba se desactiva Load Game.
  - El menú principal muestra "7kmod-editor 0.1.0 (7kmod 0.1.0)".
- **Editor de escenarios, fase 1 (guardar):**
  - El botón **Save** del editor abre la ventana **Save Scenario**: nombre, dificultad
    (1 a 10), bonus (0 a 500), descripción (6 líneas) y el archivo que se va a usar.
  - Guarda el escenario en la carpeta `SCENMOD` de la carpeta de configuración del juego
    (`%APPDATA%\7kfans.com\7kaa-mod\SCENMOD\`): la partida (`.SCN`), el texto (`.SCT`)
    y la imagen (`.SCP`), que es la franja central de la vista del mapa al guardar.
  - El nombre del archivo sale del nombre del escenario (`Mi Escenario` →
    `MI-ESCENARIO`). Si ya existe, pregunta antes de reemplazarlo.
  - Los escenarios guardados aparecen en Single Player → Scenario con "(mod)" después del
    nombre y se juegan como los demás.
- **Editor de escenarios, fases 2 y 3 (colocar, editar y ventanas):**
  - **Panel derecho:** arriba, el reino de lo que se coloca (`< ■ Reino >`, incluye
    Independent). Pestañas con paleta:
    - **Units:** Soldier y General (raza y nivel de combate), armas y barcos.
    - **Towns:** pueblo (raza y población); independiente si el reino es Independent.
    - **Buildings:** Fort, Mine, Factory, Inn, Market, Tower of Science, War Factory y
      Harbor, terminados, vacíos y sin costo.
    - **Nature:** Clay, Copper, Iron (reserva) y guaridas de Fryhtan (tipo).
  - **Colocar:** clic en la paleta y clic en el mapa, con la marca del juego (con color se
    puede, negra no). Clic derecho o Esc deja de colocar.
  - **Lo seleccionado:** nombre, reino, una propiedad con `< >` (combate, población o
    reserva) y los botones **Set Owner** y **Delete** (o Supr).
  - **Barra superior:** **Scenario** (nombre, dificultad, bonus y descripción sin
    guardar), **Kingdoms** (dinero, comida, reputación, tecnologías y relaciones de cada
    reino) y **Goals** (las metas de New Game).
- **Editor de escenarios, fases 2b y 4:**
  - **Mover:** con unidades seleccionadas, clic derecho en el mapa las pone ahí al
    instante (un grupo se acomoda alrededor del punto; barcos solo en agua).
  - **Meter:** clic derecho en un fuerte, edificio o pueblo de su reino las mete, con las
    reglas del juego. Colocar un Soldier, General o Worker de la paleta sobre un edificio
    de su reino lo mete directo.
  - **Sacar:** al seleccionar un edificio, el panel muestra su general y sus soldados o
    trabajadores, con los botones **All out** y **General out**.
  - **Worker** en la paleta de Units, con habilidad (Mining, Manufacture, Research,
    Construction) y nivel.
  - **Edit** en la pantalla Scenario: abre el escenario seleccionado en el editor con su
    nombre, dificultad, bonus y descripción. Un escenario original se guarda como copia en
    SCENMOD.
  - Quitar la selección: Esc o clic izquierdo en un lugar vacío (el clic derecho ahora
    mueve).
- **Editor de escenarios, fase 5a y 5b (terreno):** pestaña **Terrain** con pincel de
  1×1, 3×3 o 5×5:
  - **Trees** (árboles del tipo de suelo), **Clear** (quita árboles, rocas y manchas de
    tierra), **Small rock** y **Large rock**.
  - **Dark grass**, **Light grass** y **Dirt**: pintan el suelo con sus transiciones
    automáticas. No se pinta en costas, colinas, edificios, pueblos ni recursos, así que
    el agua y las regiones no cambian.

### Corregido
- **Armas colocadas con el editor:** a veces no tenían gráfico, no atacaban o hacían
  tronar el juego, porque quedaban sin versión cuando el reino no tenía su tecnología.
  Ahora toda arma colocada tiene versión (opción **Version** en la nueva pestaña
  **Weapons**, y también en lo seleccionado).
- **Soldados colocados con el editor:** salían como Peasant porque no tenían la habilidad
  de liderazgo. Ahora Soldier y General tienen la opción **Leadership** (20 por defecto)
  además de Combat (20 por defecto), y lo seleccionado muestra las dos.
