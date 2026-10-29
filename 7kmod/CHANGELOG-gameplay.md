# Changelog de 7kmod-gameplay

Cambios de la rama `7kmod-gameplay` (ejecutable `7kaa-gameplay.exe`), que modifican el
estilo de juego. Esta rama incluye todo lo de la rama `7kmod`, documentado en
`CHANGELOG.md`; aquí solo se anota lo propio de esta rama.

## Cómo se usa este archivo

- Cada cambio nuevo se anota primero en **Sin publicar**.
- Al publicar una versión, esa sección se renombra con el número y la fecha, y se crea
  el tag `7kmod-gameplay-X.Y.Z` en el commit correspondiente.
- Cada versión indica en qué versión de `7kmod` está basada.
- Versiones: el tercer número sube con correcciones, el segundo con funciones nuevas o
  cambios de balance, y el primero queda en 0 mientras el mod siga en pruebas.
- Secciones de cada versión: **Agregado**, **Cambiado**, **Corregido**, **Compatibilidad**.

---

## [Sin publicar]

Basada en **7kmod 0.2.0**, que agrega el editor de escenarios (ver `CHANGELOG.md` y
`TODO-editor.md`).

### Agregado
- **Greater Beings** (detalles en `GREATER_BEINGS.md`):
  - **Dragon normando: bola de fuego.** Ya no ataca; su botón de poder (con su bola de
    fuego como ícono) lanza una bola de 100 puntos con daño base 150 (antes 15) y 2
    casillas más de área: radio de daño de 109 pixeles (antes 45), bajando al 50% en la
    orilla, y fuego a 5 casillas (antes 3). Contra armadura 4 quita unos 37 de vida al
    centro y 20 a tres casillas. Los edificios reciben el daño ÷3. La IA la lanza contra
    grupos de 5 o más enemigos.
  - **Djini mughul: convierte a un soldado.** Además de bajar la lealtad, el soldado con
    menos combate del área (no generales, reyes ni espías; empates: el de menor lealtad)
    pasa a tu reino, como una unidad que cambia de bando.
  - **Phoenix griego: exponer espías.** Botón de poder (el ojo de los espías): sobre un
    pueblo de tu reino, expone y ejecuta al espía ajeno de menor habilidad, con un anillo
    de fuego sobre el pueblo (el del Mind Turner en naranja). Cuesta 100 puntos, aunque no
    haya espías; fuera de un pueblo tuyo no gasta. La IA lo usa en sus pueblos con espías.
- **Fryhtans invocados en el editor de escenarios:** entrada **Fryhtan** en la pestaña
  Units, con las opciones **Type** (los 10 Fryhtans que se pueden invocar, sin importar
  la raza del reino) y **Combat** (50 por omisión, como al invocar; de 10 en 10). Sale
  como un Fryhtan invocado del reino elegido (`FirmBase::invoke_fryhtan`): vida completa
  y todo lo de los invocados, sin gastar puntos de oración. Sobre un fuerte del reino
  entra como soldado. Al seleccionarlo se cambia su Combat. Necesita un reino; los
  Fryhtans salvajes siguen saliendo de las guaridas. Código en `ModEditorFryhtan.cpp`.
- **Los disparos a distancia ya no se bloquean** por edificios, murallas, montañas ni otros obstáculos (`BulletArray::bullet_path_possible`). Antes, si el tiro recto (flechas, jabalinas, etc.) pasaba por una casilla no caminable, el arquero tenía que moverse a otra casilla para disparar, y a veces quedaba pegado al enemigo. Ahora todos los disparos pasan por encima de todo, como ya lo hacían los proyectiles curvos de las armas de asedio. Aplica a todas las unidades a distancia de todos los reinos. Efecto secundario: se puede disparar a unidades detrás de murallas o al otro lado de una montaña si están dentro del alcance.
- **Espionaje: Propaganda en pueblos independientes.** En un pueblo independiente el modo "Sow Dissent" del espía se llama **"Propaganda"** y es mucho más potente (ver `SPYING.md` y `TODO-espionaje.md`):
  - **Solo cuenta el espía de más habilidad de cada reino** en el pueblo (tener más espías no acelera nada): cada 30 días la resistencia hacia tu reino baja su habilidad ÷ 20, sin dividir entre la población (antes habilidad ÷ 5 ÷ población de su raza). **Debajo de 30 de resistencia el efecto se reduce a la mitad.**
  - **La raza importa:** efecto completo sobre la gente de la raza del espía y una cuarta parte sobre las otras razas del pueblo (antes, ninguno). Lo pensado es contratar en una Inn un espía de la raza del pueblo.
  - **Los espías solos no conquistan:** sin un fuerte tuyo enlazado al pueblo y con general, la propaganda no baja la resistencia de 30. Un fuerte vacío o con el enlace desactivado no cuenta.
  - **Sube la influencia del fuerte enlazado:** habilidad del mejor espía ÷ 5 si es de la raza del general del fuerte (÷ 20 si es de otra raza), máximo +20. La meta del fuerte (100 − influencia) baja, así que un general de liderazgo medio ya no se queda atorado.
  - **Aldeanos que se unen:** cada 30 días, un pueblo independiente con tus espías en Propaganda tiene una probabilidad de que **un** aldeano lo deje y se una a tu reino (máximo 1 por pueblo al mes). Probabilidad, por raza: habilidad del mejor espía (uno de otra raza cuenta una cuarta parte) ÷ 12 × población de esa raza ÷ 30 %, **máximo 20%** (un espía de 100 en un pueblo de 60: unos 2 aldeanos al año). El aldeano tiene el combate de los defensores del pueblo ±10, una habilidad al azar (construcción, minería, fabricación, investigación o liderazgo) de 10 a 10 + la habilidad del mejor espía ÷ 2 y lealtad 60. El pueblo nunca se queda vacío. Aparece un mensaje.
  - **Los pueblos independientes ya no atrapan espías** (antes, 1 entre 100 + habilidad al mes). Ahí solo te pueden atrapar los contraespías de otros reinos.
  - **Contraespionaje en pueblos independientes:** modo nuevo **"Counter-Spy"** (en un pueblo independiente el ciclo queda Sleep → Propaganda → Counter-Spy). Cada 30 días tu mejor contraespía en el pueblo puede atrapar y ejecutar a un espía de otro reino que esté en Propaganda: habilidad × 10 ÷ (100 + habilidad del enemigo) % por espía (un espía de 50 contra uno de 50: 3.3% al mes), como mucho uno al mes por pueblo. Los dormidos y los contraespías no se pueden atrapar así.
  - **Confrontación entre contraespías:** si hay contraespías de dos reinos en el pueblo, cada uno puede encontrar al otro (habilidad × 5 ÷ (100 + habilidad del otro) % al mes). El que lo encuentra inicia la confrontación y gana con (su habilidad + 30) ÷ (su habilidad + 30 + habilidad del otro); el que pierde es ejecutado, aunque sea el que atacó. Como mucho una confrontación al mes por pueblo. Mensajes para los dos lados. La IA no usa el contraespionaje.
  - **La IA** ya usaba este modo en pueblos independientes, así que usa la propaganda. Ahora saca a su espía del pueblo cuando la resistencia de su raza llega a 30 y no tiene un fuerte enlazado con general (antes revisaba la lealtad, que los pueblos independientes no tienen).
- **Espionaje: accidentes.** En cualquier edificio con trabajadores (no los fuertes ni la Sede de Poder), de cualquier reino, puede ocurrir un accidente: 1 entre 3,650 al día por edificio (1 cada 10 años en promedio). En los accidentes normales muere 1 trabajador el 90% de las veces y 2 el 10%; nunca un espía. Los espías enemigos en **Sabotaje** lo hacen mucho más probable según su habilidad: el multiplicador es 1 + la suma de su habilidad ÷ 10, máximo ×16 (un espía de habilidad 100: ×11, unos 1 accidente al año en ese edificio). Los accidentes que causan matan a 1 o 2 trabajadores (50% cada uno). **Un espía que causa un accidente no puede causar otro en 180 días:** mientras espera su habilidad no cuenta, y el siguiente accidente se atribuye al de más habilidad que no esté esperando. Límites por reino en 365 días: 1 accidente normal y 3 causados por espías. Mensajes: "An accident occurred at your <edificio> near <pueblo>..." y, si lo causó tu espía, "Your spy caused an accident at the <edificio> of <reino>...". Los accidentes que causan tus espías se listan en el reporte de Espías, pestaña nueva **"Sabotage"** (fecha, reino, edificio, pueblo cercano, muertos y el espía de más habilidad), con el total al final; se guardan los últimos 30.
- **Espionaje: porcentaje de éxito del soborno.** Cada botón del menú de soborno muestra la probabilidad de éxito, por ejemplo "Offer $1,000 (35%)". Si el objetivo es un espía de otro reino el soborno siempre falla, pero el porcentaje se muestra como para una unidad normal, para no delatarlo.
- **Espionaje: reportes de inteligencia** (reemplazan "Steal secrets", que se quitó):
  - Modo nuevo del espía, **"Gather Intel"**, en pueblos y edificios de otros reinos. Mientras recolecta lo pueden atrapar, como en los otros modos activos.
  - El reporte tarda **de 2 a 6 meses** según la suma de la habilidad de tus espías en Gather Intel en ese reino: 180 días con 20 o menos, 60 días con 200 o más, proporcional en medio (por ejemplo 160 días con 1 espía de 50). Se recibe un reporte de cada reino a la vez, sin importar cuántos espías tengas en él; lo hace el espía de más habilidad.
  - Secciones según la habilidad del espía: Pueblos (20), Economía (30), Comercio (30), Reino (40), Tecnología (40), Militar (50), Espías (90).
  - Los números salen como un rango con un error de ±(100 − habilidad) ÷ 2 % (exactos con habilidad 100); el valor real queda en un punto al azar del rango. Las tecnologías y las relaciones son exactas.
  - Se consultan en el reporte de Espías, pestaña nueva **"Intelligence"**: la lista de reportes (fecha, reino, espía, habilidad) y el detalle del seleccionado, con el mismo fondo blanco translúcido que las listas de las demás ventanas de reportes. Se guardan los últimos 30 reportes de cada reino.
  - Al terminar un reporte aparece un mensaje. La IA no usa este modo.
  - Ya no existe la exposición del espía al cerrar los reportes secretos, ni su error del original (ver `SPYING.md`, sección 9).
- **Invocar Fryhtan:** botón nuevo en la Sede de Poder (icono de contratar unidad, último lugar de la fila) que invoca un Fryhtan de tu reino con combate 50. Cada raza tiene su Fryhtan aliado, según la raza de la Sede:
  - Normandos → Holgh, Mayas → Deezboanz, Griegos → Sauroid, Vikingos → Haubudam, Persas → Rokken.
  - Chinos → Wyrm, Japoneses → Rattus, Egipcios → Karrotten, Mughules → Doink, Zulúes → Broosken.
  - **Costo en puntos de oración,** según la fuerza de cada Fryhtan (vida × daño por segundo con combate 50, ver `COMBAT_ANALYSIS.md`), con −5 para los lentos: Rokken 90; Sauroid 80; Wyrm y Haubudam 75; Holgh 60; Rattus 55; Karrotten 50; Deezboanz 45; Broosken 40; Doink 30. Ajustados con duelos simulados contra soldados (ver `COMBAT_ANALYSIS.md`). La barra tiene 400 puntos.
  - **Daño:** los Fryhtans invocados hacen 25% más daño que los salvajes, para compensar que su daño por golpe es bajo para su tamaño. Se suma al bono del general si salen del fuerte con uno.
  - El poder de ataque que muestra el panel del Fryhtan (ver `CHANGELOG.md`) incluye este bono.
  - **Doink:** el único Fryhtan con ataque a distancia. Su alcance sube de 5 a 6, igual que los arqueros humanos (también para los Doink salvajes). El Doink invocado tiene +1 de daño base por golpe (6 en lugar de 5), antes del +25%.
  - **Rokken:** el Rokken invocado tiene armadura 5 (los salvajes y los demás Fryhtans, 4). Con eso es el tanque de los Fryhtans: aguanta mejor contra soldados con general.
  - **Broosken:** su velocidad sube de 3 a 4, la normal de las demás unidades (también para los salvajes).
  - Son permanentes: no gastan puntos por día ni mueren si la Sede es destruida.
  - Si tienes espías propios dentro de la Sede, ese lugar lo ocupa el botón de espías, como en el juego original.
  - Los Fryhtans de un reino no deambulan solos, atacan según su modo agresivo, no entran a guaridas Fryhtan, no dan reputación a quien los ataca y no cuentan para el objetivo de destruir a los Fryhtans.
  - **Regeneración:** recuperan 1 punto de vida cada 10 días de juego (unos 8 segundos a la velocidad normal), aunque se estén moviendo o peleando. Dentro del fuerte también, además de la recuperación normal de los soldados.
  - **Curación del Greater Being persa:** su poder ahora cura también a los Fryhtans de tu reino, tanto en el mapa como dentro de los fuertes. Antes solo curaba unidades humanas.
  - **Costo visible:** el botón muestra el costo en puntos. Al pasar el ratón aparece el nombre del Fryhtan, su costo y los puntos que tienes.
  - Sus gráficos no tienen zonas con el color del reino, así que se ven igual que los Fryhtans salvajes.
  - Los Fryhtans no tienen sonidos de selección ni de orden en los datos del juego; usan su sonido de muerte en su lugar.
  - **Voces propias:** si existe `MODSOUND\<código>.WAV` en la carpeta del juego, el Fryhtan lo usa como sonido de selección y de orden. El código es el nombre interno del Fryhtan, el mismo de su archivo en `SPRITE` (el del Doink es `GREMJERM`). El Rokken tiene su voz en `MODSOUND\ROCKMAN.WAV`; los archivos de voces del mod están en `7kmod/MODSOUND` del repositorio.
- **Puntos de la Sede de Poder:** el panel de la Sede muestra los puntos de oración como número ("Points: 123 / 400") debajo de la barra.
- **Documentación:** `COMBAT_ANALYSIS.md` explica cómo funciona el combate (vida, fórmula de daño, velocidad de ataque, poder y ataques especiales) y compara a los soldados de cada raza y a los Fryhtans con tablas de daño por segundo y duelos simulados. Incluye que la armadura de los datos no se usa en el código. El simulador está en `7kmod/tools/combat_sim.py` para volver a calcular los números después de un cambio.
- **Fryhtans en el fuerte:** los Fryhtans de tu reino pueden entrar a tus fuertes como soldados, igual que las armas de asedio, y salir conservando su especie. En el panel del fuerte se muestra su combate. No entrenan con el general. Al salir del fuerte tienen el botón de regresar al fuerte.
- **Retrato pequeño de los Fryhtans:** los datos del juego no tienen retratos pequeños para los Fryhtans (usaban uno persa). Ahora se generan al iniciar el juego reduciendo su retrato grande, y se ven en la lista de soldados del fuerte.
- **Tecnología Building Repair:** nueva tecnología en Technologies de la torre de ciencia. Todos los edificios de tu reino recuperan 1 punto de vida cada 5 días (un edificio de 200 tarda unos 1000 días en llenarse), sin necesidad de constructor. Se suma a la reparación del constructor, si hay uno. Sigue reparando durante los ataques, pero se detiene si tu tesoro es negativo. No repara muros. Por ahora su investigación es muy rápida, para probarla; la IA no la investiga.
- **Tecnología Fryhtan Slayers:** nueva tecnología en Technologies de la torre de ciencia. Los soldados de tu reino (unidades humanas: soldados, generales y rey) hacen 30% más daño a los Fryhtans, tanto salvajes como invocados por otros reinos. El bono se aplica antes de la armadura, como el del general. No aplica a las armas de asedio, los barcos ni a tus Fryhtans invocados. Por ahora su investigación es muy rápida, para probarla; la IA no la investiga.
- **Los Fryhtans invocados comen:** consumen comida al año según su costo de invocación: Rokken 50, Sauroid 45, Wyrm y Haubudam 40, Holgh 35, Rattus y Karrotten 30, Deezboanz y Broosken 25, Doink 20 (una persona come 10). Comen en el mapa y dentro del fuerte. Sin comida pierden 1 punto de vida por día; se suma a la pérdida por falta de dinero (2 por día si faltan los dos). Los Fryhtans salvajes no comen. Por ahora el consumo no se muestra en la interfaz.

- **Íconos de las tecnologías del mod:** Elite Warriors, Elite Workers, Building Repair y Fryhtan Slayers tienen su propio ícono, con el marco y la paleta de los íconos del juego. Se ven en el menú Technologies de la torre de ciencia (como los de War Machines) y en el panel mientras se investiga (antes, un cuadro con el texto "Tech").
- **Ayuda en Technologies:** al pasar el ratón sobre una tecnología aparece su nombre y su descripción. El botón ya no muestra la descripción, que no cabía junto al ícono.

### Cambiado
- **Greater Beings:** el Jing Nung chino gasta 5 puntos por ataque (antes 10) y el
  Phoenix cuesta 1.5 por día (antes 2.5). Los cambios a la tabla `GOD` se aplican en el
  código (`GodRes::apply_mod_changes`), también al cargar una partida guardada.
- **Tecnologías del mod:** Building Repair y Fryhtan Slayers cuestan lo mismo que Elite
  Warriors (antes eran muy baratas, para pruebas).
- **Balance de razas:**
  - **Armadura por raza:** el juego original usa armadura 4 para todas las unidades e ignora la de los datos. Ahora la armadura sube con el combate en algunas razas: los Normandos tienen 4 y suben a 5 con combate 50; los Griegos tienen 4, suben a 5 con combate 30 y a 6 con combate 70; los Mayas tienen 3 mientras su combate es menor a 50 y 4 desde 50. Las demás razas, los Fryhtans, las armas y los barcos siguen con 4. Cada punto de armadura quita 0.25 de daño a cada golpe recibido. El panel de la unidad muestra su armadura actual.
  - **Chinos:** el ataque a distancia se aprende con combate 40 (antes 30).
  - **Egipcios:** el ataque a distancia se aprende con combate 30 (antes 40).
  - **Japoneses:** el ataque especial se aprende con combate 50 (antes 55).
  - **Mughules:** el segundo ataque especial se aprende con combate 50 (antes 60).
  - **Zulúes:** velocidad de movimiento 5 (antes 4), un 25% más rápidos.
  - **Bono del general en batalla:** un general en el mapa con al menos 4 de sus soldados (las unidades que él dirige) a 10 casillas o menos recibe su propio liderazgo como bono de daño (por ejemplo +60% con 60 de liderazgo) y +1 de armadura. Es el mismo alcance del bono que les da a sus soldados. También aplica al rey con sus tropas. Solo cuentan las unidades que tienen a ese general (o al rey) como líder: las que salen del fuerte con el botón de patrulla o en la defensa automática del fuerte; las que sacas una por una con clic derecho no cuentan. El panel del general muestra el bono con la flecha hacia arriba, por ejemplo "Armor 5 ↑ (+1)".
  - Los cambios se aplican al cargar los datos; los archivos del juego no se modifican. El efecto en los combates está en la sección 8 de `COMBAT_ANALYSIS.md`.
- **Greater Being persa (Lord of Healing) cura más en los fuertes:** dentro de un fuerte o de la Sede de Poder, los soldados (y sacerdotes) recuperan entre 1/5 y 1/3 de su vida máxima (antes entre 1/7 y 1/4), y el general al mando cura igual que sus soldados (antes curaba como en el mapa, entre 1/9 y 1/6). En el mapa cura igual que antes.
- **Pueblos de otro reino que se rinden:** empiezan con 15 más de lealtad (55 para la raza de tu rey y 45 para las demás, antes 40 y 30) y no pueden rebelarse durante 90 días, la misma espera que ya había entre dos rebeliones. Antes quedaban justo encima del límite de rebelión (29) y a veces se rebelaban casi de inmediato. No aplica a los pueblos independientes capturados ni cuando un reino completo se rinde. Aplica igual a la IA.
- **Versión en el menú principal:** se muestra la versión de esta rama y la de `7kmod` en la que está basada, por ejemplo "7kmod-gameplay 0.1.0 (7kmod 0.2.0)". La versión está en `include/ModVersion.h`.

### Compatibilidad
- **Partidas guardadas:** versión 299 (296 agrega Building Repair, 297 Fryhtan Slayers, 298 el espionaje: fechas de los accidentes, progreso de la recolección y reportes de inteligencia, 299 la lista de accidentes causados por espías y 300 la espera de los espías después de un accidente). Las partidas de la versión 2.00 y posteriores se siguen reconociendo como del mismo formato base aunque la versión llegue a 300. Las partidas anteriores se siguen cargando (las tecnologías nuevas quedan sin investigar y sin reportes de inteligencia). Los Fryhtans de un reino se guardan como unidades normales.
- **Multijugador:** invocar un Fryhtan usa el mensaje de red nuevo `MSG_F_BASE_INVOKE_FRYHTAN`. Todos los jugadores necesitan el mismo ejecutable.
- La IA no invoca Fryhtans.
- **Balance de razas:** no cambia el formato de las partidas guardadas; al cargar una partida se aplican las reglas nuevas. En multijugador, todos necesitan el mismo ejecutable.

---

## [0.1.0] - 2026-10-18

Basada en **7kmod 0.1.0**. Primera versión documentada; reúne todos los cambios hechos
hasta ahora.

### Agregado

#### Puerto: comercio de materias primas
- **Botón Trade** (icono de impuestos) que cambia al panel de comercio:
  - El puerto funciona como una mina sin trabajadores: produce 10 unidades cada 3 días, con un almacén de hasta 500.
  - **Rotar producción:** cambia entre arcilla, cobre y hierro. Se pierde lo almacenado, como al cambiar la producción de una fábrica.
  - **Hire Shipment** (icono de Grant): cuesta $500, se registra como gasto de importaciones y agrega 500 al suministro. Sin suministro no hay producción.
  - Botón de regresar (icono de barco).
- Los mercados y fábricas conectados toman materia prima tanto de minas como de puertos.

#### Reclutamiento en edificios de producción
- **Botón Recruit (icono de Train)** en minas, fábricas, torres de ciencia y fábricas de guerra, igual que en el fuerte:
  - Recruit mete la unidad directamente al edificio.
  - Train entrena la profesión del edificio (minería, manufactura o investigación) y la unidad entra sola al terminar.
- **Botón de reclutamiento automático** (icono de defensa) en esos edificios del jugador, para activarlo o desactivarlo.
- "Back to <edificio>" se acorta a "Back" si el nombre no cabe.

#### Árbol de tecnologías
- **Conduct Research** en la torre de ciencia pregunta primero: **Technologies** o **War Machines**.
- Tecnologías nuevas, que se investigan en Technologies:
  - **Elite Warriors:** los soldados entrenados en Leadership empiezan con 30 de combate (antes 20).
  - **Elite Workers:** las unidades entrenadas en las demás profesiones empiezan con 30 de habilidad (antes 20).
- El tiempo de investigación de ambas es 3/4 del de la catapulta nivel 1.
- Al terminar aparece una noticia. La IA no las investiga.

#### Pueblos: Train Captain
- **Botón Train Captain** (icono de promover), en la segunda fila del panel del pueblo:
  - Entrena un **general** de la raza del rey con 30 de liderazgo, 20 de combate y potencial de liderazgo aleatorio de 50 a 100.
  - Cuesta $150 y 1 habitante, y tarda 4 veces lo de un soldado normal (20 días).
  - Solo funciona en pueblos donde la raza del rey es la mayoría.
  - Clic izquierdo agrega a la cola, clic derecho quita uno. El botón muestra cuántos hay en entrenamiento.

#### Documentación
- `TODO.md`: lista de ideas pendientes y su estado.

### Cambiado

#### Pueblos independientes
- **Máximo 2 razas** en los pueblos independientes (antes hasta 3), tanto los iniciales como los que aparecen después.
- **55% de los pueblos iniciales tienen 2 razas** y 45% una sola. La segunda raza siempre es distinta de la primera.

#### Edificios de producción
- **Reclutamiento automático desactivado por defecto** en minas, fábricas, torres de ciencia y fábricas de guerra del jugador. La IA sigue reclutando automáticamente.
- **Espacio para el botón de espías:**
  - Torre de ciencia y fábrica de guerra: los botones subieron un poco y el de espías va en una segunda fila.
  - Fábrica: usa las páginas con flechas, como el fuerte.

### Corregido
- La barra de progreso de las tecnologías ya no se sale del panel ni deja restos en el mapa.

### Compatibilidad
- **Partidas guardadas:** el mod carga partidas de versiones anteriores, incluidas las del juego oficial 2.15.x y las de `7kmod`. El juego oficial y `7kaa-mod.exe` no cargan las partidas de esta rama.

  | Versión de partida | Agrega |
  |---|---|
  | 291 | Comercio del puerto |
  | 292 | Suministro por envíos contratados |
  | 293 | Reclutamiento automático por edificio |
  | 294 | Tecnologías del mod |
  | 295 | Elite Workers |

- **Multijugador:** cada acción nueva tiene su mensaje de red (`MSG_F_HARBOR_TRADE_RAW`, `MSG_F_HARBOR_HIRE_SHIPMENT`, `MSG_FIRM_TOGGLE_AUTO_RECRUIT`, `MSG_F_RESEARCH_START_MOD_TECH`). Todos los jugadores necesitan el mismo ejecutable.
