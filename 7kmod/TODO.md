# 7kmod-gameplay: pendientes

Rama para los cambios que modifican el estilo de juego. La rama `7kmod`
queda solo con mejoras de comodidad (interfaz) sobre la 2.15.7.

---

## Checklist del próximo parche

Cosas anotadas para agregar en un parche. `[x]` = implementado, falta probarlo en partida.
Greater Beings: los poderes del Dragon, el Djini y el Phoenix se probaron en el editor
(F12) con los puntos de oración llenos; falta probar la IA en partida.

- [x] **Los disparos a distancia ya no se bloquean por edificios ni obstáculos.** Hoy, un
  proyectil recto (flechas, jabalinas, etc.) no puede pasar por casillas no caminables:
  edificios, murallas, montañas y otros obstáculos del mapa
  (`BulletArray::bullet_path_possible`, `src/OBULLETA.cpp`). Las unidades ya no
  bloqueaban (esa revisión está desactivada en el original), y los proyectiles curvos
  (tipo 'P', como los de las armas de asedio) nunca se revisan. Cuando el tiro está
  bloqueado, el arquero busca otra casilla desde donde disparar y se mueve, a veces
  quedando pegado al enemigo. Con el cambio, todos los disparos pasan por encima de
  todo, como los proyectiles curvos.
  - Aplica a todas las unidades a distancia de todos los reinos (también la IA, los
    Fryhtans como el Doink y los barcos).
  - Efecto secundario: se puede disparar a unidades detrás de murallas o al otro lado
    de una montaña, si están dentro del alcance.
  - Va en `7kmod-gameplay` (y por lo tanto en `7kmod-pathing`).

- [x] **Tooltip de los poderes de los Greater Beings mughul, egipcio y zulú.** El botón
  del poder (`UnitGod::disp_info`, `src/OU_GOD.cpp`) usa como ayuda el nombre de su ícono,
  y `RESOURCE/HELP.RES` no trae `GODMUGL`, `GODEGYPT` ni `GODZULU` (sí los de Persia,
  Japón, Maya y Thor): esos tres botones no tienen tooltip. Es un error del juego
  original. Arreglo: ayuda propia (`help.set_custom_help`), sin tocar los datos, con el
  estilo de los demás:
  - Mughul: "Lower Enemy Loyalty": el cursor cambia a un círculo; sobre unidades de
    reinos en guerra contigo o independientes, les baja la lealtad (y, con el cambio de
    abajo, convierte a un soldado).
  - Egipcio: "Increase Population": sobre un pueblo de tu reino, le agrega habitantes.
  - Zulú: "Increase Leadership": sobre un general o rey zulú de tu reino, le sube el
    liderazgo.
  - Va en `7kmod` (es un error del original) y se pasa a gameplay y pathing.

- [x] **Djini (Greater Being mughul): además convierte a un soldado.** Hoy su poder baja
  −20 a −30 de lealtad a las unidades visibles del área que sean de reinos en guerra
  contigo o de nadie (`UnitGod::indian_cast_power`). Nuevo: en cada lanzamiento, además,
  **el soldado con menos combate del área** (rango soldado, no general ni rey) pasa a tu
  reino (`Unit::change_nation`).
  - Solo uno por lanzamiento, entre las mismas unidades a las que baja la lealtad.
  - Empates de combate: el de menor lealtad (después de bajarla).
  - Solo personas (no armas de asedio ni Fryhtans invocados).
  - Espías disfrazados: no se convierten.
  - Llega a tu reino con la lealtad de una unidad que se rinde.
  - El tooltip del poder lo menciona.
  - Va en `7kmod-gameplay` (cambia el balance).

- [x] **Phoenix (Greater Being griego): poder para exponer espías y menor costo.** Hoy el
  Fénix solo explora y revela información; no tiene poder (`can_cast_power` en 0) y es
  el que más cuesta mantener. Nuevo:
  - **Botón de poder** en su panel, como los de los demás seres (`UnitGod::disp_info`,
    `src/OU_GOD.cpp`): al dar clic, se elige un **pueblo de tu reino** (alcance de una
    casilla, como Isis). Si en el pueblo hay espías de otros reinos, **expone a uno**: si
    hay varios, al de **menor habilidad de espionaje**.
  - **Costo de mantener: de 2.5 a 1.5 por día** (en la tabla `GOD` son 50 puntos de
    existencia ÷ 20; quedaría en 30), igual que la mayoría. Se cambia en el código al
    cargar la tabla (`GodRes`), sin tocar los datos.
  - **Exponer = descubrir y ejecutar**, como cuando lo atrapa el contraespionaje
    (`SpyArray::catch_spy`, `Spy::get_killed`), con la noticia normal.
  - **Pueblo sin espías ajenos:** el poder se gasta igual.
  - **Efecto gráfico:** el juego ya tiene uno por poder: el ser lanza una "estrella" que
    se anima sobre el objetivo (`GE_STAR`, `GI_STAR`, `GJ_STAR`, `GP_STAR`, `GZ_STAR`
    en la tabla de ataques de `STD.SET`; el Fénix no tiene). Se puede usar uno de esos
    sobre el pueblo. Candidatos: el anillo morado del Mind Turner (`GJ_STAR`) o los
    destellos de Isis (`GE_STAR`), o uno de ellos con los colores cambiados (naranja,
    como el fuego del Fénix), cambiando los colores al cargarlo.
  - **Elegido: el anillo del Mind Turner (`GJ_STAR`) en naranja**, con los colores
    cambiados al cargarlo (sin tocar los datos).
  - **Costo del poder: 100 puntos.**
  - **Ícono:** el existente que mejor se adapte (por ejemplo el de los espías); tooltip
    propio.
  - **La IA lo usa.**
  - La IA lo usa sobre su pueblo con espías ajenos (el juego sabe dónde están), cuando
    tiene los puntos.
  - Solo pueblos (no edificios).
  - Actualizar `GREATER_BEINGS.md` (hoy dice que el Fénix no detecta espías).
  - Va en `7kmod-gameplay` (cambia el balance).

- [x] **Dragon (Greater Being normando): bola de fuego con botón en vez de atacar.**
  - **Hoy:** es, con el Jing Nung chino, el único ser que ataca solo
    (`UnitGod::pre_process`, `src/OU_GOD.cpp`). Su ataque en la tabla de ataques de
    `STD.SET`: daño base 15, alcance 6, retraso 6, proyectil `DR_ARROW` (la bola de
    fuego). Cada ataque gasta 10 puntos de oración (`power_pray_points`), que también
    son su vida, al terminar el ataque (`UnitGod::process_attack`).
  - **Área del proyectil hoy** (`Bullet`, `src/OBULLET.cpp`): revisa las casillas a 2 o
    menos del objetivo (5×5) y daña a las unidades y edificios a 45 pixeles o menos del
    centro (`bullet_radius`; una casilla mide 32), con el daño bajando del 100% al
    centro al 50% en la orilla. En la práctica, el objetivo y las casillas pegadas. No
    daña a tu reino ni a quien no estás atacando. Además prende fuego en las casillas a
    3 o menos (`fire_radius`, distancia en cuadrícula), más fuerte al centro.
  - **Nuevo:**
    - Ya no ataca solo (como los demás seres, `force_move_flag`).
    - **Botón de poder** en su panel: al dar clic, se elige un punto en el mapa (dentro
      de su alcance, o vuela hasta él como los demás) y lanza la bola de fuego.
    - **Área: +2 casillas.** Propuesta: radio de daño de 45 a 109 pixeles (unas 3.4
      casillas), revisión de casillas de 2 a 4 (9×9) y fuego de 3 a 5.
    - **Costo: 100 puntos** por bola (hoy 10 por ataque).
    - **Daño base 150** (hoy 15): unos 37 de vida al centro, 31 a una casilla, 26 a
      dos y 20 a tres, contra armadura 4.
  - **Edificios:** los daña con la regla normal del juego (`Unit::hit_firm`: el daño
    ÷3, una vez por edificio en el área): unos 12 de vida a un edificio al centro.
  - **Ícono:** el existente que mejor se adapte; tooltip propio.
  - **La IA lo usa**, sobre grupos de enemigos.
  - El daño baja hacia la orilla, como hoy (al 50%).
  - La IA la lanza cuando hay 5 o más unidades enemigas en el área.
  - Referencia del daño: hoy cada bola (base 15) quita unos 3 de vida al centro (÷4 de
    `ATTACK_SLOW_DOWN`, −1 de la armadura 4); 100 puntos son 10 bolas, unos 30 de vida
    sobre un objetivo. Un soldado con combate 20 tiene 40 de vida, con 50 tiene 100.
  - El Jing Nung chino sigue atacando solo (ver abajo).
  - Actualizar `GREATER_BEINGS.md`.
  - Va en `7kmod-gameplay` (cambia el balance).

- [ ] **Sonido del poder del Phoenix: el de Isis.** El Phoenix no tiene animación de
  ataque, así que su poder no suena (los demás seres suenan con el sonido "A1" de su
  sprite en los cuadros de su animación de lanzar, `Sprite::process_attack`,
  `src/OSPRITE2.cpp`). Al exponer espías (`UnitGod::phoenix_expose_spy`) se reproduce el
  sonido de lanzar de Isis (`se_res.sound(..., 'S', <sprite de EG_GOD>, "A1")`, en el
  pueblo), en los cuadros en que suena. Va en `7kmod-gameplay`.

- [x] **Jing Nung (Greater Being chino): cada ataque cuesta 5 puntos en vez de 10.**
  Sigue atacando solo, con su secuencia de 3 ataques (daño base 18). El costo sale de
  `power_pray_points` de la tabla `GOD` (10); se cambia en el código al cargar la tabla
  (`GodRes`), sin tocar los datos. Con eso dura el doble de ataques con los mismos
  puntos (los puntos también son su vida). Actualizar `GREATER_BEINGS.md`. Va en
  `7kmod-gameplay`.

- [ ] **La IA invoca Fryhtans y los usa como armas de asedio.** Hoy la IA nunca usa el
  botón de invocar Fryhtan del Seat of Power (`FirmBase::invoke_fryhtan`); solo invoca a
  su Greater Being cuando los puntos de oración llegan al máximo, 400
  (`FirmBase::think_invoke_god`, `src/OF_BASE2.cpp`). Y si tuviera un Fryhtan, no haría
  nada con él: en `Unit::think_action` (`src/OUNITAI.cpp`) solo piensan los reyes, los
  generales, las armas (`think_weapon_action`, cada 15 días) y las personas.
  - **Propuesta de regla para invocar** (en el `think` del Seat of Power de la IA):
    - **El Greater Being va primero:** solo invoca si su ser ya está en el mapa
      (`god_unit_recno`). Antes de eso guarda los puntos para el ser, como hoy.
    - **Reserva:** solo si le quedan al menos 100 puntos después de pagar el Fryhtan
      (para que el ser siga vivo y pueda usar su poder).
    - **Frecuencia:** lo revisa cada 30 días, y cada Seat espera al menos 90 días entre
      una invocación y la siguiente. Con eso son pocos, como las armas de asedio.
    - **Tope:** como máximo un Fryhtan por fuerte de la IA y 4 en total por reino.
    - **Comida:** solo si su comida no va en déficit, porque cada Fryhtan come de 20 a
      50 al año.
    - **Guerra:** solo si está en guerra o tiene enemigos (si no, los puntos se gastan
      sin uso).
  - **Manejo, como un arma de asedio** (`think_weapon_action`): el Fryhtan busca un
    fuerte de su reino con lugar (`think_assign_weapon_to_camp`; en gameplay los fuertes
    ya aceptan Fryhtans), prefiriendo los cercanos al enemigo, y sale con los ataques
    del fuerte. Si no hay lugar, se queda cerca del Seat como defensa.
  - **Por definir:** los números (reserva, días, tope) y si cambian con la dificultad
    de la IA.
  - Va en `7kmod-gameplay`.

- [ ] **Ícono con el color del reino en los Fryhtans invocados.** Hoy un Fryhtan de un
  reino se ve igual que uno salvaje: el color de reino sale de 8 colores especiales del
  sprite (`0xE0`-`0xE7`, `Game::init_remap_table`, `src/OGAME.cpp`) y los sprites de
  los Fryhtans no los tienen. Nuevo: un ícono de 10×10 como los de rango y oficio (`U_GENE`,
  `U_KING`... en `I_ICON.RES`, `Unit::draw_skill_icon`, `src/OUNITDRW.cpp`), a la
  derecha de la barra de vida.
  - **Dibujo (versión A, elegida):** el cuadro del color del reino, con el borde claro
    arriba y a la izquierda y oscuro abajo y a la derecha (los 4 tonos del color del
    reino), y una cabeza de Fryhtan con cuernos y colmillos en negro. Se dibuja en el
    código con los colores especiales del reino (sin tocar `I_ICON.RES`), así toma el
    color de cada reino.
  - **Cuándo se ve:** igual que los íconos de rango (al seleccionar o señalar la unidad,
    o siempre con la opción de mostrar todos los íconos), pero en los Fryhtans de
    **todos** los reinos, no solo los tuyos. Los Fryhtans salvajes no llevan ícono.
  - Va en `7kmod-gameplay`.

---

## Tecnologías del mod (`ModTech`)

Las tecnologías del mod están en `include/ModTech.h` y `src/ModTech.cpp`,
separadas de las armas de `TechRes`. Se investigan en "Technologies" de la
Torre de Investigación y no tocan la diplomacia, los reportes ni las demás
partes que asumen que una tecnología es un arma.

- **Hechas:** Elite Warriors, Elite Workers, Building Repair y Fryhtan Slayers.
- **Agregar una tecnología:** una entrada en el enum y en
  `mod_tech_info_array` (nombre, descripción, costo, niveles), su efecto con
  `mod_tech_res.get_level(...)`, su ícono en `src/ModTechIcon.cpp` (bitmap de
  46×38 con índices de `PAL_STD.RES`, mismo orden que el enum), y subir `GAME_VERSION_7KMOD` con su
  `MAX_MOD_TECH_BEFORE_...` para leer las partidas anteriores
  (`GameFile::read_file_3` en `src/OGFILE2.cpp`).
- **Pendientes:**
  - [x] Costo real de Building Repair y Fryhtan Slayers (hoy es muy barato, solo para
    pruebas): **el mismo que Elite Warriors** (15, `complex_level` en
    `mod_tech_info_array`).
  - Que la IA investigue las tecnologías del mod: por ahora no.
  - [x] Íconos propios: cada tecnología tiene su ícono de 46×38 con la paleta del juego
    (`src/ModTechIcon.cpp`), en el menú y en el panel de investigación. La descripción
    pasó a la ayuda del botón.
  - Más tecnologías. Ideas: entrenar más rápido o más barato, minas o
    fábricas más productivas, bonos de lealtad.

El análisis original de las mejoras permanentes quedó en el historial de git
de este archivo.
