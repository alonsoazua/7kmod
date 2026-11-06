# Changelog de 7kmod

Cambios de la rama `7kmod` (ejecutable `7kaa-mod.exe`) respecto a la versión original
**Seven Kingdoms: Ancient Adversaries 2.15.7** (tag `v2.15.7`).

La rama `7kmod` tiene mejoras de comodidad e interfaz y algunos ajustes de balance.
Los cambios que modifican el estilo de juego están en la rama `7kmod-gameplay`, que
incluye todo lo de `7kmod` y documenta lo suyo en `CHANGELOG-gameplay.md`. El editor de
escenarios (desde la 0.2.0) está descrito en `TODO-editor.md`.

## Cómo se usa este archivo

- Cada cambio nuevo se anota primero en **Sin publicar**.
- Al publicar una versión, esa sección se renombra con el número y la fecha, y se crea
  el tag `7kmod-X.Y.Z` en el commit correspondiente.
- Versiones: el tercer número sube con correcciones, el segundo con funciones nuevas o
  cambios de balance, y el primero queda en 0 mientras el mod siga en pruebas.
- Secciones de cada versión: **Agregado**, **Cambiado**, **Corregido**, **Compatibilidad**.

---

## [Sin publicar]

---

## [0.3.0] - 2026-11-06

Más funciones del editor (reinos, reyes, espías, mapa en limpio) y correcciones.

### Agregado
- **Editor: agregar y quitar reinos.** En la ventana Kingdoms, **Add Kingdom** crea un
  reino de la IA sin rey y **Remove Kingdom** quita uno con sus pueblos, edificios y
  unidades; también se cambia el color de un reino. El rey se coloca con la entrada
  **King** de la pestaña Units, y Delete ya borra reyes. Test y Save avisan si a algún
  reino le falta rey. Detalles en `TODO-editor.md`.
- **Editor: mapa en limpio.** Opción **Base** en la ventana New Map: Generated, **Land** o
  **Water** (todo el mapa de pasto o de agua, sin nada encima ni reyes).
- **Editor: espías.** Entrada **Spy** en la pestaña Units, con opciones Race, Spying y
  Cloak. Sobre un pueblo o edificio (de cualquier reino) entra como espía; en el mapa
  queda como unidad disfrazada. Al seleccionarlo se ve su reino verdadero y se cambian
  su habilidad y su disfraz; los pueblos y edificios muestran cuántos espías tienen, y
  los pueblos los sacan con **Spies out**. Detalles en `TODO-editor.md`.

### Corregido
- **Recuadro sobre unidades de otros reinos:** a veces algunas unidades del recuadro no
  quedaban en la lista ni mostraban su barra de vida, y si la unidad elegida era una de
  ellas, no había lista. La lista comprueba que cada unidad siga siendo la misma
  comparando su nombre, y guardaba el número del nombre en una variable que volvía
  negativos los números mayores a 32,767 (`inspect_name_id_array`): esas unidades nunca
  coincidían y se quitaban. Depende del nombre de cada unidad, por eso no era constante.
- **Ayuda de los poderes de los Greater Beings mughul, egipcio y zulú:** sus botones no
  tenían texto de ayuda al pasar el ratón, porque el archivo de ayuda del juego
  (`RESOURCE/HELP.RES`) no trae sus claves (`GODMUGL`, `GODEGYPT`, `GODZULU`). Es un
  error del juego original. Ahora muestran "Lower Enemy Loyalty", "Increase Population"
  e "Increase Leadership" con su explicación, como los demás poderes
  (`UnitGod::power_help`).
- **Retratos de los Fryhtans en la lista de unidades seleccionadas:** salían con un
  retrato persa, porque los datos del juego no traen retratos chicos de Fryhtans (el
  juego original nunca los muestra). Ahora se hacen al cargar el juego reduciendo el
  retrato grande de cada Fryhtan, para todos sus rangos (soldado, Ordo y All High)
  (`make_small_icon`, `src/OUNITRES.cpp`).

### Compatibilidad
- Sin cambios en las partidas guardadas ni en el multijugador.

---

## [0.2.0] - 2026-10-28

Agrega el **editor de escenarios** (antes en la rama `7kmod-editor`). Su diseño, fases y
detalles técnicos están en `TODO-editor.md`.

### Agregado
- **Contribución y honores de los generales:** un general se vuelve infeliz cuando su contribución (1 punto por cada golpe al enemigo) es más del doble de lo que se le ha dado en recompensas; pierde hasta 50 de lealtad. El juego original no lo mostraba en ningún lado. Ahora:
  - **Reporte militar (F5) con pestañas** en el panel de abajo:
    - **Units:** lo que mostraba antes (unidades por tipo).
    - **Contributions:** la lista de tus generales, ordenada por riesgo, con Loyalty (actual y hacia dónde va), Contrib., Status (Content, Unrewarded o At risk! si su lealtad objetivo es menor a 30 y te puede traicionar), Penalty (cuánta lealtad pierde) y Honors (lo que cuesta quitar el penalty, a $30 por recompensa).
    - **Pay selected** paga los honores del general seleccionado y **Pay all** los de todos, en orden de riesgo, hasta que se acaba el dinero. Usan la recompensa normal del juego. Doble clic en un general lleva a él.
  - **Petición de honores:** cuando uno de tus generales llega al penalty máximo (−50), aparece la noticia "Your general, <nombre>, asks to be honored for his service: $<costo>". Al darle clic, la cámara se mueve al general y lo selecciona (o a su fuerte, si está dentro), y se abre el reporte de reinos (F1), igual que con una propuesta diplomática, con tu reino seleccionado y ninguna pestaña seleccionada. Abajo aparece la petición, por ejemplo "General Itzcoatl asks $150 to be honored for his service to the kingdom.", con las opciones **Accept.** (paga los honores) y **Reject.** (no paga). Si no te alcanza el dinero, una segunda línea lo indica y solo aparece Reject. Al elegir, la noticia se quita y regresas a la vista anterior. El juego sigue mientras está abierto. Si el general muere, cambia de reino o deja de ser general, la noticia se elimina y la petición se cierra. Pagar sus honores por otro medio también quita la noticia.
- **Desglose de la lealtad:** al dejar el ratón sobre la lealtad de un general o un soldado (en el panel de la unidad, del fuerte o de la Sede de Poder) aparece de qué se compone su lealtad objetivo: pueblos enlazados, tropas, raza, reputación, liderazgo del rey y penalty por falta de recompensas para los generales; liderazgo del general, combate y raza para los soldados. Solo para tus unidades. Como toda la ayuda del juego, aparece en partidas de un jugador con la ayuda activada.
- **Unidades seleccionadas:** al seleccionar varias unidades, el número de unidades seleccionadas aparece a la derecha de la barra superior del panel de la unidad.
- **Poder de ataque y armadura en el panel de las unidades:** debajo del combate (y arriba de la contribución) se muestran:
  - **Melee pwr:** el poder del ataque cuerpo a cuerpo: el daño promedio por minuto antes de la armadura del enemigo, a la velocidad normal del juego, con el mismo cálculo del juego. Combina el daño de cada golpe y la velocidad de ataque. Contra un enemigo real, la armadura le quita armadura ÷ 4 a cada golpe (1 con la armadura normal), así que el daño real es menor, sobre todo en unidades de golpes débiles. Se muestra sin armadura para que los bonos se lean exactos: un bono de +100% duplica el poder.
  - **Ranged pwr:** lo mismo para el ataque a distancia, si la unidad lo tiene.
  - **Bono del general:** si el general de la unidad está a 10 casillas o menos, el poder incluye su bono y se muestra con una flecha hacia arriba, como en la lealtad, y el porcentaje: por ejemplo "62 ↑ (+100%)". El porcentaje es el liderazgo del general y es exactamente lo que sube el poder.
  - **Armor:** la armadura de la unidad. En `7kaa-mod.exe` es 4 para todas, como en el juego original.
  - No incluye los ataques especiales que gastan poder. No se muestra en civiles ni en tus propios espías (el menú de espía necesita ese espacio).
  - El panel crece según las líneas que muestra y los botones se recorren hacia abajo.
- **Panel de los Fryhtans:** al seleccionar un Fryhtan se muestra su combate, poder de ataque y armadura. Antes no mostraba ningún dato.
- **Documentación:** `GREATER_BEINGS.md` explica en detalle cómo funciona cada Greater Being (cómo se obtienen, costos, poderes y efectos exactos), según el código y los datos del juego. Incluye lo que revela el Fénix (la información de unidades y edificios enemigos cercanos) y que su detección de espías está desactivada en el código original.
- **Documentación:** `ADVANCED_CONFIG.md` explica las opciones avanzadas ocultas de `config.txt` del juego original: dónde va el archivo, su formato, qué hace cada opción y cómo se comporta en multijugador y en las partidas guardadas.
- **Documentación:** `KEYBINDINGS.md` lista todas las teclas y combinaciones con el ratón del juego y del mod: generales, mapa, grupos, paneles de unidades, edificios y pueblos, colas de producción, menús y trucos, con el nombre de cada acción para cambiarla con `bindkey`.
- **Documentación:** `SIEGE_WEAPONS.md` explica las armas de asedio: costos de construcción y mantenimiento, qué pasa sin dinero, estadísticas por versión, daño contra unidades y edificios, daño en área, escudos contra proyectiles y detalles de la Spitfire, la Unicorn y la Porcupine.
- **Aviso "Game Paused":** mientras el juego está en pausa aparece "Game Paused" centrado en la parte de arriba del mapa, tanto si pausaste tú (`Espacio` o `0`) como si el juego se pausó solo al perder el foco de la ventana. Desaparece al continuar. En multijugador lo ven todos, porque la pausa es compartida. Antes no había ningún aviso.
- **Unidades fuera del fuerte:** el panel del fuerte sigue mostrando a las unidades que salieron de él, oscurecidas, para que se vea a quién pertenece el fuerte y dónde están sus tropas:
  - **General (o rey):** si el fuerte se queda sin general, en su lugar aparece el retrato oscurecido del general que salió, con su nombre y "Outside of the fort". Si otro general entra al fuerte, se muestra el nuevo como siempre.
  - **Soldados:** los que salieron aparecen oscurecidos, con su barra de vida, en los lugares vacíos de la lista de soldados (hasta completar 8). No cuentan como soldados del fuerte.
  - Al dar clic en uno, la cámara se mueve a esa unidad y la selecciona.
  - Solo se muestran mientras están en el mapa: si mueren, entran a otro edificio o cambian de reino, ya no aparecen. Solo en tus fuertes.
- **Botón Withdraw en el fuerte:** en la página 1 de los botones del fuerte, en el lugar de Alert / At ease. Hace que todas las unidades que salieron del fuerte (general o rey, soldados, armas y Fryhtans) regresen a él, con la misma orden de "regresar al fuerte" del juego. Se desactiva si no hay nadie afuera. El botón **Alert / At ease pasa a la página 2**, en el primer lugar; por eso tus fuertes ahora siempre tienen las flechas de página. Cuando hay espías, su botón va en el segundo lugar de la página 2.
- **Lista de las unidades seleccionadas:** con varias unidades seleccionadas, junto a su número (a la derecha de la barra del nombre del panel) aparece un botón **">"**. Reemplaza el resto del panel con una lista de las unidades, como la de los soldados del fuerte: retrato, barra de vida y nivel de combate (o de habilidad para los civiles). El botón cambia a **"<"** para volver. El orden es rey y generales primero, luego por tipo de unidad y combate. Clic izquierdo en una unidad la selecciona sola y abre su panel; clic derecho la quita de la selección. Al pasar el ratón aparece su nombre. Con más de 32 unidades hay páginas con flechas. La lista se cierra sola si queda una unidad seleccionada.
- **Recuadro sobre unidades de otros reinos:** antes, un recuadro sin unidades tuyas no seleccionaba nada. Ahora, si dentro hay unidades de otros reinos, Fryhtans salvajes, rebeldes o independientes, se selecciona la más importante (rey, luego general, luego la de más vida; no se usa el combate para no revelarlo) y se ve su panel. Igual que con tus unidades, si hay alguna unidad militar en el recuadro solo se toman las militares (con `Alt`, todas). Como la profesión de las unidades de otros reinos no se ve (y así no se delatan los espías), todas sus unidades humanas cuentan como militares; solo las caravanas y los barcos Trader y Transport cuentan como civiles. El número y el botón ">" muestran la lista de todas las del recuadro (hasta 100), agrupadas por reino (primero el de la unidad elegida, al final las que no tienen reino), cada una con el color de su reino en una esquina y **sin nivel de combate**. No reciben órdenes: clic izquierdo abre el panel de una (la lista se conserva y ">" la vuelve a mostrar) y clic derecho la quita de la lista. Mientras el panel muestra una de ellas, todas las de la lista muestran su barra de vida sobre ellas en el mapa, como las unidades seleccionadas.
- **Selección con prioridad militar:** al seleccionar con el recuadro, si dentro hay al menos una unidad militar (soldados, generales, rey, armas de asedio, barcos de guerra Caravel y Galleon, Fryhtans y Greater Beings), solo se seleccionan las militares y se ignoran los civiles (constructores, mineros, trabajadores, investigadores, sacerdotes, caravanas y barcos Trader y Transport). Tus espías cuentan según la habilidad que aparentan: los que parecen soldados (sin habilidad o con liderazgo) se seleccionan con el ejército, y los que aparentan un oficio civil quedan fuera. Si solo hay civiles, se seleccionan como antes. Con `Alt` presionado el recuadro selecciona todas las unidades, como en el juego original. También aplica con `Shift` para agregar a la selección.
- **Retrato del rey en el informe de reinos (F1):** en la lista de reinos, después del color de cada reino aparece el retrato de su rey (por ejemplo, el del rey japonés si el rey es japonés). Se usa la raza del rey actual, porque el sucesor puede ser de otra raza; si el reino se queda un momento sin rey, se muestra el rey de la raza del reino. Al dejar el ratón sobre el retrato aparece la raza del rey (como toda la ayuda del juego, en partidas de un jugador con la ayuda activada).

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
- **Editor de escenarios, mapa nuevo:**
  - **New** en la pantalla Scenario ya no muestra la pantalla de New Game: abre el
    editor con un mapa nuevo con la configuración de la última partida y un Map I.D. al
    azar.
  - Botón **New** en la barra del editor: abre la ventana **New Map** con todas las
    opciones de New Game (las básicas y las avanzadas, el nombre y el Map I.D., con
    **Random Map I.D.**) y la dificultad calculada. **Create** pregunta (se pierden los
    cambios sin guardar) y genera el mapa; **Cancel** cierra sin cambiar nada. Las metas
    se siguen editando en **Goals**.
  - El mapa nuevo empieza sin nombre ni descripción de escenario, como uno nuevo.

- **Editor de escenarios, cambiar la raza:** fila **Race** al seleccionar:
  - **Pueblo:** los aldeanos sin trabajo pasan a la raza nueva (los que trabajan en
    edificios y los espías conservan la suya).
  - **Unidades** (una o varias): soldados, generales, trabajadores y aldeanos cambian de
    raza conservando reino, rango, combate, liderazgo o habilidad, lealtad y equipo.
    Reciben un nombre de la raza nueva.
  - **Rey:** cambia también la raza del reino, y toma un nombre de la raza nueva (el
    reino también), salvo con Lock Kingdom Name. Con Seats of Power Unlocked, el reino
    conoce la Sede de su raza nueva.
  - Las construcciones no tienen fila de raza (la del Seat of Power se elige al
    ponerlo).
  - La unidad queda en la misma casilla.
  - El cuadro de lo seleccionado crece hacia arriba según las filas que necesita.
- **Editor de escenarios, mina:** al seleccionar una mina, **Mineral** (None, Clay,
  Copper, Iron) y **Reserve** (de 1,000 a 20,000). Una mina sin mineral puede tenerlo:
  se crea el sitio de mineral debajo con la reserva de la pestaña Nature. Cambiar de
  mineral conserva la reserva y vacía lo ya extraído; None quita el sitio.
- **Editor de escenarios, barra:** con una ventana abierta, otro botón de la barra cambia
  a su ventana y el mismo botón la cierra.

- **Editor de escenarios, fase 5c (agua, tierra y colinas):** entradas **Water**,
  **Land** y **Hill** en la pestaña Terrain. Water pinta mar con sus orillas, Land pinta
  tierra (también sobre el mar) y Hill levanta una meseta rodeada de colinas; los
  trazos se unen, así se hacen caminos altos y mesetas de cualquier forma, con rampas
  automáticas donde caben (bordes rectos). La entrada **Ramp** pone o quita una rampa.
  Después se recalculan las costas, lo que se camina y navega, las regiones del mapa y
  las de pueblos, edificios, puertos y la IA. No se pinta donde hay
  unidades, edificios, pueblos, recursos, muros o rocas.

- **Editor, Seat of Power:** en la paleta de Buildings, con la opción Race. Al ponerlo,
  el reino recibe el pergamino de su raza; al quitarlo o cambiarlo de reino, el reino se
  queda sin pergamino si ya no tiene un Seat de esa raza.

- **Editor, nombres:** botón **Re-roll** para unidades y pueblos (otro nombre de su
  raza; los edificios con el nombre del pueblo cambian con él) y **Rename** para reyes.
  En la ventana Kingdoms, **Lock Kingdom Name** evita que el nombre del rey cambie al
  hacer re-roll o al cambiarle la raza.
- **Editor, Workers:** opción Skill con **Praying** (Seat of Power) y **None** (aldeano
  sin oficio).

### Cambiado
- **Versión en el menú principal:** en lugar de "7kmod-test" se muestra la versión del mod, por ejemplo "7kmod 0.2.0". La versión está en `include/ModVersion.h`.

- **Editor, paleta:** Tower of Science se muestra como "Twr. Science" porque no cabía.
- **Editor, Hill con tierra (5d):** Hill pinta la tierra de la meseta con cualquier
  pincel (como la antigua Dirt) y las colinas salen solas en su contorno; la meseta sigue
  la forma pintada, con esquinas y escalones, y las partes angostas quedan como muro. Se
  quitó la entrada **Dirt**. **Land** baja o recorta las mesetas; **Clear** solo quita
  árboles, rocas y manchas de tierra; **Water**, Dark grass y Light grass no cambian las
  mesetas.
- **Editor, pintar arrastrando:** en la pestaña Terrain, con el botón presionado se pinta
  por donde pasa el ratón (todas las entradas de pincel; rocas y Ramp siguen siendo de
  un clic). Lo que no se puede pintar a media pasada se salta sin aviso.
- **Pestaña Buildings:** se quitó la opción Race; en el juego los edificios no tienen raza.
  Tampoco el fuerte tiene ya la fila Race (la raza de su general) en lo seleccionado.
- **Bajar la población** de un pueblo en el editor ya no hace sonar el grito de los
  aldeanos.

### Corregido
- **Unidades fuera del fuerte:** al dar clic en un soldado oscurecido del fuerte ya no quedan restos de las barras de vida de la lista del fuerte encima del panel del soldado.
- **Fondo de los paneles nuevos:** el fondo de la lista de unidades se aclaraba un poco más cada vez que la lista cambiaba (por ejemplo, al morir una unidad), y el del general fuera del fuerte cada vez que cambiaba. El panel se volvía a pintar encima de sí mismo, y como no es opaco aclaraba lo que ya estaba aclarado. Ahora se pinta de nuevo toda la interfaz, como hace el juego.
- **Lista de unidades de otros reinos:** a veces aparecían unidades que no estaban en el recuadro, como un soldado saliendo de un fuerte al otro lado del mapa. Cuando una unidad muere o entra a un edificio, el juego le da su número interno a la siguiente unidad que se crea; la lista ahora comprueba que cada número siga siendo la misma unidad (tipo, nombre y reino).
- **Documentación:** en `ADVANCED_CONFIG.md`, la lista de acciones para `bindkey` estaba incompleta; faltaban `KEYEVENT_CHEAT_ENABLE1` a `3`, `KEYEVENT_SEL_GROUP_7` a `9` y `KEYEVENT_SET_GROUP_1` a `9`.
- **Nombres repetidos en las noticias:** en mensajes con dos nombres, como "El espía <nombre> del reino de <rey> ha sido descubierto y ejecutado", el nombre de la unidad salía igual al del rey. La función que arma los nombres usaba una sola memoria compartida, y con el compilador de los ejecutables del mod el segundo nombre sobrescribía al primero. Es un error del juego original que no se nota en la versión oficial por el orden en que su compilador evalúa los nombres.

- **Cierre del juego después de un escenario:** después de jugar cualquier escenario, el juego tronaba al cerrarse ("double free"). Al empezar un escenario se hace una copia de la configuración avanzada para restaurarla al final, y la copia compartía con la original el texto de `mp_service_addr`, que se liberaba dos veces. Es un error del juego original; no afectaba las partidas.

- **Editor, cambiar la raza de una unidad:** ya no la mueve una casilla abajo, y las
  unidades del reino se cuentan bien.
- **Editor, gastos del año:** colocar un edificio ya no suma su costo a los gastos del
  año del reino (el dinero ya se restauraba).
- **Armas colocadas con el editor:** a veces no tenían gráfico, no atacaban o hacían
  tronar el juego, porque quedaban sin versión cuando el reino no tenía su tecnología.
  Ahora toda arma colocada tiene versión (opción **Version** en la nueva pestaña
  **Weapons**, y también en lo seleccionado).
- **Soldados colocados con el editor:** salían como Peasant porque no tenían la habilidad
  de liderazgo. Ahora Soldier y General tienen la opción **Leadership** (20 por defecto)
  además de Combat (20 por defecto), y lo seleccionado muestra las dos.

### Compatibilidad
- Sin cambios en las partidas guardadas ni en el multijugador. Los escenarios del editor
  son partidas guardadas normales (más su texto e imagen) en la carpeta `SCENMOD`.

---

## [0.1.0] - 2026-10-18

Primera versión documentada. Reúne todos los cambios hechos hasta ahora sobre la 2.15.7.

### Agregado

#### General
- **Marca en el menú principal:** el texto "7kmod-test" aparece junto a la versión, para distinguir el mod del juego original.
- **Carpeta de configuración y partidas propia (`7kaa-mod`):** el mod no mezcla sus partidas guardadas ni su configuración con las del juego oficial.

#### Fuerte: reclutar y entrenar desde el fuerte
- **Botón Recruit en el fuerte** (icono de Train):
  - Si hay varios pueblos propios conectados al fuerte, muestra la lista para elegir uno; si hay uno solo, va directo.
  - Abre el pueblo en "modo fuerte", con solo tres botones: Recruit, Train y "Back to Fort" en una línea. Tax y Grant se ocultan.
- **Recruit en modo fuerte:** la unidad de la raza elegida entra directamente al fuerte.
- **Train en modo fuerte:** pone en cola un soldado de Leadership sin pasar por el menú de entrenamiento. Shift + clic agrega 8.
- **Los soldados entrenados desde el fuerte entran solos al fuerte** al terminar, si sigue siendo tuyo y tiene espacio. Si no, se quedan junto al pueblo como siempre.
- **Contador en el botón Train:** muestra cuántos soldados se están entrenando para ese fuerte.
- **Páginas de botones para el espía:** si tienes espías en tu propio fuerte, aparecen flechas "<" ">" bajo la fila de botones para cambiar a la página con el botón de espías.

#### Opciones de partida
- **Seats of Power Unlocked?** (página Advanced Options II, partida de un jugador):
  - Con "Yes", tu reino empieza sabiendo construir la Sede de Poder (Seat of Power) de su raza, como si tuviera el pergamino.
  - Solo aplica al reino del jugador, no a la IA, y no funciona en multijugador.
  - La elección se guarda en `7KMOD.DAT`; `CONFIG.DAT` conserva el formato original.

### Cambiado

#### Pueblos independientes
- **Población equilibrada al inicio:** todos los pueblos independientes iniciales tienen de 15 a 29 habitantes, sin importar cuántas razas tengan. Antes, los mixtos eran más pequeños (2 razas: 12–22, 3 razas: 11–19).
- **Menor resistencia en pueblos mixtos:** −8 con 2 razas y −12 con 3 o más, con un mínimo de 10. Aplica a los pueblos iniciales y a los que aparecen después.

#### Interfaz
- El botón Cancel de la lista de pueblos es más alto, para que el texto quepa.

### Compatibilidad
- **Partidas guardadas:** versión de partida **290** (agrega el fuerte destino de las unidades en entrenamiento). El mod carga partidas del juego oficial 2.15.x; el juego oficial no carga las partidas del mod.
- **Multijugador:** el entrenamiento para el fuerte usa el mensaje de red nuevo `MSG_TOWN_TRAIN_FOR_FIRM`. Todos los jugadores necesitan el mismo ejecutable del mod.
