# 7kmod-fryhtan: reinos Fryhtan

Rama `7kmod-fryhtan`, basada en `7kmod-gameplay`. Permite jugar como un reino Fryhtan.
En la versión 1 **solo el jugador** puede ser un reino Fryhtan; los reinos de la IA siguen
siendo humanos. Solo partidas de un jugador (sin multijugador).

`[ ]` = pendiente, `[x]` = implementado, falta probarlo en partida.

---

## 1. Reglas confirmadas

- **Inicio:** el reino empieza con su rey, un **All High** de la especie elegida, como
  general de una guarida ya construida, con **4 soldados** de su especie (combate 20).
  No hay pueblos al inicio.
- **Guaridas:** funcionan como un **fuerte**: un general (o el rey) y hasta **8 soldados**. Los soldados entrenan con el
  general, salen a patrullar y regresan, igual que en un fuerte.
- **Edificios humanos:** el reino Fryhtan no puede construir ni usar edificios humanos
  (minas, fábricas, mercados, torres de ciencia, posadas, puertos, fuertes, Sedes de
  Poder, etc.). Los que capture se destruyen.
- **Guaridas que construye:** solo de la especie del reino (la de su rey).
- **Reputación:** no tiene; queda fija en 0.
- **Lealtad:** los Fryhtans del reino no tienen lealtad. No se muestra, no baja, no se
  premia y ninguno deserta, se rebela ni cambia de bando por lealtad.
- **Esclavizar pueblos** (en lugar de capturarlos):
  - Hay que construir una guarida **enlazada** al pueblo independiente.
  - El liderazgo del general de la guarida baja la resistencia del pueblo **hasta 45 como
    máximo** (no baja más de ahí).
  - La última parte se hace **peleando**: atacando el pueblo hasta que su resistencia
    llegue a 0, como ya funciona la captura por fuerza en el juego. Al llegar a 0 el
    pueblo queda esclavizado.
- **Pueblo esclavizado:** no se pueden sacar ni entrenar unidades de él ni cobrarle
  impuestos. Su panel solo dice que al final del mes recibirás **X de oro como tributo**.
- **Combate:** las unidades del reino Fryhtan ganan combate al entrenar (con el general
  en la guarida) y al pelear, igual que los soldados de un reino humano.
- **Capturar guaridas independientes:** enlazar una guarida tuya con una guarida
  independiente (de **cualquier especie**) permite capturarla pagando un tributo. No
  recibes los Fryhtans salvajes de adentro: la captura es una **simulación** de un
  general y sus soldados, ver "Captura de guaridas" abajo. Así el reino puede tener
  Fryhtans de varias especies. Cada guarida entrena Fryhtans de su propia especie, y cada
  Fryhtan usa la población de su tamaño (1 o 2).
- **Pueblos de reinos humanos:** también se pueden esclavizar, peleando hasta bajar su
  lealtad a 0, como ya funciona en el juego. La influencia de la guarida (hasta 45) solo
  aplica a pueblos independientes. Los edificios de ese reino enlazados al pueblo no
  cambian de dueño.
- **Guaridas salvajes:** aunque no ataquen al reino Fryhtan, él sí puede atacarlas y
  destruirlas, además de capturarlas con oro.
- **Nombre del reino:** "<nombre del rey> <especie> Horde", por ejemplo "Al Rokken
  Horde". El nombre del rey es el que pones en "Your Name" al crear la partida.
- **Costos:** las guaridas cuestan oro de mantenimiento. Las unidades no cuestan oro,
  solo comida.
- **Matar unidades humanas** da un poco de oro y de comida.
- **Población Fryhtan (tope de unidades):** cuentan **todas las unidades del reino,
  incluido el rey** (en el mapa, en las guaridas y las que se están entrenando). Un
  Fryhtan **pequeño usa 1** de población y uno **grande usa 2**. El tope inicial es de
  **18**, y cada **persona** de población humana capturada (en pueblos esclavizados) lo
  sube en **1**. Por ejemplo, un reino de Rokken (grandes) empieza con su rey (2 de 18) y
  puede tener hasta 9 Rokken; uno de Rattus (pequeños), hasta 18. Con un pueblo
  esclavizado de 30 personas el tope sube a 48.
- **Fryhtans salvajes:** no atacan a los reinos Fryhtan.
- **Diplomacia:** los reinos humanos no pueden comerciar ni hacer pactos de amistad o de
  alianza con un reino Fryhtan. Sí se permiten la paz, el cese al fuego, la guerra, el
  tributo y la ayuda. Los reinos de la IA **no se pueden rendir** a un reino Fryhtan.

---

## 2. Valores aprobados

Valores iniciales aprobados. Los números quedarán en un solo lugar del código para
ajustarlos fácil después de probar.

1. **Grandes y pequeños**, según el nivel de la especie en los datos del juego (las 13
   ocupan una casilla, así que el tamaño no sirve para separarlas):
   - **Pequeños (nivel 5 o menos), 1 de población:** Rattus (3), Ick (4), Deezboanz (5),
     Broosken (5), Doink (5).
   - **Grandes (nivel 6 o más), 2 de población:** Pfith (6), Karrotten (6), Holgh (6),
     Haubudam (7), Wyrm (7), Droog (7), Sauroid (7), Rokken (9).
2. **Quién construye guaridas:** el rey o un general, con un botón **Build Lair** en su
   panel. Al terminar entra a la guarida como su jefe. Costo **$500**, el tiempo de
   construcción de un fuerte.
3. **Mantenimiento de una guarida:** **$100 al año**.
4. **Entrenar soldados:** botón **Train** en la guarida: cuesta **20 de comida**, tarda
   **20 días** y saca un soldado de la especie del reino con combate 20, que entra a la
   guarida. Necesita lugar en la guarida y población libre (1 o 2 según la especie).
5. **Generales:** se promueve a un soldado con un botón **Promote**, como en el fuerte.
   Su liderazgo sube al pelear con sus soldados, como el de un general humano.
6. **Si muere el rey:** el general de más combate pasa a ser el nuevo All High. El reino
   pierde cuando ya no le quedan guaridas ni unidades.
7. **Influencia de la guarida sobre un pueblo:** cualquier guarida con general o rey
   enlazada al pueblo baja la resistencia hasta 45; el liderazgo decide qué tan rápido
   baja (la misma velocidad con la que baja hacia un fuerte).
8. **Tributo:** **$5 por habitante al mes**. Es la única fuente de dinero del reino
   además de matar humanos, por eso es más alto que el impuesto de un pueblo humano
   ($5 por habitante por cobro, con costo de lealtad, más $5 al año). Se revisa en las
   pruebas.
9. **Pueblo esclavizado:**
   - Sus aldeanos **producen comida** para el reino Fryhtan, como los de un pueblo
     normal.
   - Su población **crece** normalmente.
   - Si pierde el enlace con todas tus guaridas, se vuelve **independiente**. Otro reino
     lo puede capturar peleando.
10. **Recompensa por matar humanos:** **$5 y 5 de comida** por soldado o aldeano, **$20 y
    20** por general y **$100 y 100** por un rey.

### Captura de guaridas

- **La oferta:** se genera la primera vez que una guarida tuya se enlaza con la guarida
  salvaje y se queda fija (no cambia aunque se vuelva a enlazar). Se guarda en la
  partida.
  - **Un Ordo (general):** su combate es el del mejor general de la guarida salvaje; su
    liderazgo, al azar entre 30 y 60 (los salvajes no tienen liderazgo).
  - **Soldados:** de 4 a 6 si la especie es grande y de 6 a 8 si es pequeña, con combate
    al azar entre 20 y 40.
- **Panel de la guarida salvaje** (al seleccionarla con el enlace): el Ordo con su
  combate y su liderazgo, cuántos soldados trae y el costo, con un botón **Pay
  Tribute**.
- **Costo:** la suma del combate del Ordo y de los soldados × **$2**.
- **Población:** al capturar **sí se puede pasar del tope** de población Fryhtan.
  Mientras estés por encima del tope no se pueden entrenar más Fryhtans.
- **Al pagar:** la guarida pasa a tu reino como guarida tipo fuerte, con el Ordo y sus
  soldados adentro. Los Fryhtans salvajes que estaban dentro desaparecen.
- **Los salvajes de esa guarida que andan en el mapa** siguen salvajes, sin guarida.
  No atacan al reino Fryhtan, pero sí a los humanos.

### Pantalla de New Game (propuesta)

- En la página de opciones básicas, donde se elige la raza, una fila nueva **Kingdom:
  Human / Fryhtan**.
- Con **Fryhtan**, los botones de raza se oscurecen y aparece un selector de especie
  **< Rokken >** con el retrato del Fryhtan y su tamaño ("Large: 2 population" o
  "Small: 1 population").
- El nombre del rey es el de **Your Name**; abajo se ve cómo quedará el reino ("Al
  Rokken Horde").
- Si no cabe en la página básica, va en Advanced Options II, debajo de "Seats of Power
  Unlocked?", que ya usa ese espacio. Se revisa al hacerlo.
- Solo en Single Player → New Game; en multijugador y en escenarios no aparece.

---

## 3. Diseño técnico

- **Reino Fryhtan:** `Nation` guarda la especie (`mod_monster_id`, 0 = reino humano).
  `race_id` no es una raza humana, así que hay que revisar los lugares que lo usan
  (unos 85 en el código: nombres del rey y del reino, reclutamiento, reportes, espías,
  IA de los otros reinos) para que no fallen ni den resultados raros.
- **Rey y nombre:** el rey es la unidad de la especie con rango rey (All High), con el
  nombre de "Your Name". El reino se llama "<nombre> <especie> Horde".
- **Guarida tipo fuerte:** las guaridas salvajes (`FirmMonster`) no guardan unidades
  reales, solo el combate y la vida de un rey y hasta 8 generales con 8 soldados cada
  uno. Para que la guarida del jugador funcione como un fuerte se usará la lógica de
  `FirmCamp` (general + 8 soldados, entrenamiento, patrulla, defensa, enlaces) con el
  aspecto de la guarida de la especie. Ya hay una base: en gameplay los Fryhtans
  invocados entran a los fuertes como soldados.
- **Sin lealtad:** los Fryhtans del reino se quedan con lealtad fija y se saltan las
  revisiones de lealtad (deserción, rebelión, premios, soborno) y su ayuda en el panel.
- **Experiencia:** `Unit::gain_experience` hoy solo funciona con humanos; se activa
  para los Fryhtans de un reino Fryhtan.
- **Comida:** se usa el consumo de los Fryhtans invocados (`invoked_fryhtan_food_year`,
  de 20 a 50 al año según la especie).
- **Esclavizar:** `Town::update_target_resistance` toma en cuenta las guaridas enlazadas
  con su tope de 45; `Town::surrender` deja el pueblo esclavizado (marca nueva en
  `Town`) cuando el reino es Fryhtan.
- **Diplomacia:** se quitan las opciones de comercio, amistad y alianza del menú de
  diplomacia (F1) y de las propuestas de la IA cuando uno de los dos reinos es Fryhtan.
- **Fryhtans salvajes:** las guaridas salvajes y sus unidades no eligen como objetivo a
  un reino Fryhtan (`FirmMonster::think_attack_human`, defensa de las guaridas).
- **Partidas guardadas:** versión nueva para la especie del reino, los pueblos
  esclavizados y las guaridas tipo fuerte.
- **Ejecutable:** `7kaa-fryhtan.exe`, con su propia carpeta de configuración.

---

## 4. Fases

### Fase 1: reino Fryhtan
- [x] Rama `7kmod-fryhtan`: ejecutable `7kaa-fryhtan.exe`, carpeta de configuración
      `%APPDATA%\7kfans.com\7kaa-fryhtan\`, versión de partida 310 y versión en el menú
      ("7kmod-fryhtan 0.1.0 (gameplay 0.2.0, 7kmod 0.3.0)").
- [x] Opción en New Game, página básica:
  - Botón **Fryhtan** a la derecha de Maya y selector de especie **< Rokken >** a la
    derecha de Zulu, hechos con el mismo aspecto que los botones de raza.
  - Con Fryhtan elegido los botones de raza se oscurecen; elegir una raza vuelve a reino
    humano. Las flechas recorren las 13 especies en orden alfabético.
  - Debajo de Your Name: "Fryhtan kingdom: New Rokken Horde (Large: 2 population)".
  - Se guarda en `7KMOD.DAT` como Seats of Power Unlocked. Sin retrato: no cabe en la
    página; el retrato se ve en el reporte de reinos.
  - El menú se muestra antes de cargar los datos de los Fryhtans, así que los nombres y
    niveles de las especies están en una tabla en `ModFryhtan.cpp`.
- [x] El reino guarda su especie (`ModFryhtan`, en la partida); el rey es el All High
      con combate 60 y liderazgo 60; reputación fija en 0 (`NationBase::change_reputation`).
- [x] Nombre del reino "<nombre> <especie> Horde" en el juego y en las noticias.
- [x] Los Fryhtans del reino no tienen lealtad: el juego ya se salta la lealtad, la
      traición y los premios de las unidades sin raza (`Unit::update_loyalty`,
      `Unit::think_betray`), y su panel no la muestra.
- [x] Inicio solo con el rey, en un lugar elegido como el pueblo inicial de un reino.
      La cámara empieza sobre él.
- [x] Revisión de los usos de la raza del reino (ver abajo).
- [x] Sin edificios humanos: el botón Build solo aparece en unidades humanas, así que
      el rey Fryhtan no tiene menú de construcción. Build Lair se agrega en la fase 2.

#### Revisión de la raza del reino

El reino Fryhtan guarda internamente la raza elegida en la pantalla (oculta), para que
el código que indexa la raza del reino no falle. La IA no usa esa raza
(`random_unused_race`). Lo que se encontró:

- **Arreglado en la fase 1:**
  - El nombre del reino en las noticias decía "<nombre>'s Kingdom".
  - Los aldeanos de pueblos independientes se podían unir a un reino Fryhtan cuando
    tuviera pueblos en su región (`Town::think_independent_unit_join_nation`).
  - La ayuda del retrato del rey en el reporte de reinos ahora dice la especie.
- **Sin efecto** (necesitan unidades humanas del reino, que no tiene): lealtad de
  generales y soldados según la raza del rey (`Unit::update_loyalty`, honores, espías,
  posada, IA de grandes batallas), pergaminos (los Fryhtans no los recogen).
- **Pendiente para otras fases:**
  - ~~Fase 2: los Fryhtans guardan su especie en `nation_contribution`; al ganar
    experiencia no deben sumar contribución.~~ Hecho.
  - ~~Fase 2: lealtad de los Fryhtans dentro de una guarida tipo fuerte (`Worker`).~~
    Hecho: lealtad fija en 100 y el panel muestra "N/A".
  - ~~Fase 4: lealtad de los pueblos esclavizados según la raza del rey, Train Captain y
    reclutamiento (se ocultan), e influencia del fuerte con bono por raza del rey (la
    guarida usa su propia regla).~~ Hecho: lealtad fija de 50, sin botones, y la
    guarida tiene su propia regla de influencia.
  - ~~Fase 7: la lista de partidas guardadas muestra "King <nombre>" con retrato
    humano~~ (pasó a los ajustes después de la fase 4); fase 7: los mensajes de fin de partida dicen "<nombre>'s Kingdom"; la imagen de fin de
    partida y el Hall of Fame usan la raza interna; sucesión del rey. (Hecho en la fase 7.)
- **Nota:** los Fryhtans del reino tienen las reglas de los Fryhtans invocados de
  gameplay (+25% de daño, regeneración, comida).

### Fase 2: guarida tipo fuerte
- [x] Construir una guarida de su especie (Build Lair del rey o de un general).
- [x] La guarida funciona como fuerte: general + 8 soldados, entrar, salir, patrullar,
      defender.
- [x] Train (soldados) y Promote (generales), con el tope de población.
- [x] Entrenamiento de combate con el general y experiencia al pelear.
- [x] Población Fryhtan en el panel de la guarida y en el reporte de reinos
      ("Fryhtan Population 6/18").

#### Cómo quedó la fase 2

- **La guarida es un fuerte** (`FIRM_CAMP`) de 3x3 que se dibuja con el gráfico de la
  guarida de su especie (4x4), centrado y medio tile más grande, con banderas del color
  del reino. Mientras se construye solo se ve el terreno de la guarida y las banderas
  (las guaridas no tienen gráficos de construcción). Usar el edificio de 4x4 de verdad
  rompería más de 20 usos del tamaño del fuerte. La especie de cada guarida se guarda
  en `ModFryhtan` (en la partida).
- **Nombre:** "<pueblo> Lair" si hay un pueblo cerca, si no "<especie> Lair".
- **Build:** el rey y los generales Fryhtan tienen el botón Build con una sola opción,
  "Lair" (el ícono del fuerte oscurecido con el Fryhtan encima). Cuesta $500 y $100 al
  año (en lugar de lo que cuesta el fuerte); vender o cancelar usa ese costo.
- **Panel de la guarida:** Patrol, Train y Withdraw. No hay Reward ni Recruit. Los
  Fryhtans muestran lealtad "N/A" (internamente 100, no desertan).
- **Train:** cuesta 20 de comida y tarda 20 días; sale un Fryhtan de la especie de la
  guarida con combate 20, que entra a la guarida si hay lugar. Cola de hasta 8 con el
  contador del botón; clic derecho quita uno y devuelve la comida. La ayuda del botón
  muestra la población que usa y la población del reino. No se puede pasar del tope
  (las unidades en cola cuentan).
- **Promote:** un soldado Fryhtan se puede ascender a general (Ordo) con liderazgo 20;
  Demote lo regresa.
- **Combate:** los soldados dentro entrenan con el general como en un fuerte, y los
  Fryhtans del reino ganan experiencia al pelear.
- **Reporte de reinos (Information):** línea "Fryhtan Population X/Y".
- **Probado:** construir con el rey y con un Ordo, entrar como general, cola, cancelar,
  salir, patrullar, Promote, entrenamiento (combate 20 → 73 en 9 años, rey 60 → 100),
  guardar y cargar con la cola a medias, costo anual.
- **Bug corregido:** el rey y los generales no podían volver a entrar a la guarida con
  clic derecho (gameplay solo deja entrar a los soldados Fryhtan al fuerte,
  `UnitArray::assign_to_camp`); solo caminaban hasta ella.
- **Pendiente para otras fases:** la IA le pide tributo al reino Fryhtan y le ofrece
  comprarle comida (revisar en la fase 6).

### Ajustes anotados (después de la fase 2)
- [x] **Barra de progreso de Train:** mostrar en el panel de la guarida el avance del
      Fryhtan que se está entrenando (días hechos de 20), como la barra de entrenamiento
      de un pueblo humano.
- [x] **Inicio con guarida:** el reino empieza con una guarida de su especie ya
      construida, el rey adentro como general y **4 soldados** de la especie (combate
      20), en lugar del rey solo en el mapa.
- [x] **Nombre del rey:** "<nombre del rey> The All High" (por ejemplo "Al The All
      High") en lugar de "All High <especie>", en el panel de la unidad y donde el juego
      muestre el nombre del rey. En el panel de la guarida, en vez de "King / All High
      Wyrm", la primera línea dice "Al The All High" y la segunda "All High Wyrm". El
      reino sigue llamándose "<nombre> <especie> Horde". Los generales siguen como
      "<especie> Ordo" (por ejemplo "Wyrm Ordo"), porque no tienen nombre propio.
- [x] Con el rey fuera, el panel de la guarida dice "Outside of the fort"; cambiarlo a
      "Outside of the lair".

Cómo quedaron: la barra es una línea verde en la parte de abajo del botón Train (bajo
el contador de la cola). La guarida inicial no le cuesta nada al reino. El nombre del
rey usa el nombre completo; con nombres largos se corta en el panel.

### Fase 3: economía
- [x] Mantenimiento de las guaridas en oro; unidades solo con comida (hecho en la fase 2).
- [x] Oro y comida al matar unidades humanas.
- [x] Los edificios humanos que capture se destruyen.

#### Cómo quedó la fase 3

- **Recompensa por matar:** cuando una unidad del reino Fryhtan (o su proyectil) mata a
  una unidad humana de otro reino o independiente, el reino recibe $5 y 5 de comida
  (soldado, aldeano o defensor de un pueblo), $20 y 20 (general) o $100 y 100 (rey).
  Está en `Unit::hit_target`, junto a los contadores de muertes del juego. El oro se
  suma como ingreso "Recovered Treasure", que en el reporte de economía de un reino
  Fryhtan se llama "Spoils of War and Treasure".
- **Edificios capturados:** una vez al día, cualquier edificio de un reino Fryhtan que
  no sea una de sus guaridas se destruye (`FirmArray::process`). El general, los
  trabajadores y el constructor que tenga adentro mueren con él. Cubre todas las
  formas de quedarse con un edificio (pueblos enlazados, etc.) sin tocar cada una. La
  rendición de un reino de la IA ante un reino Fryhtan se quitará en la fase 6.
- **Probado:** matar a los defensores de un pueblo independiente (+$5 y +5 de comida
  cada uno) y pasar los fuertes de la IA al reino Fryhtan con un gancho temporal de
  prueba (se destruyeron con sus soldados).

### Fase 4: esclavizar pueblos
- [x] Enlace guarida–pueblo independiente y baja de resistencia hasta 45.
- [x] Captura por combate: resistencia a 0 → pueblo esclavizado.
- [x] Pueblos de reinos humanos: lealtad a 0 peleando → pueblo esclavizado.
- [x] Panel del pueblo esclavizado: sin Recruit, Train, Collect Tax ni Grant; muestra el
      tributo del mes.
- [x] Tributo mensual; la población esclavizada sube el tope de población.

#### Cómo quedó la fase 4

- **Pueblo esclavizado** = cualquier pueblo de un reino Fryhtan (no hace falta otra
  marca).
- **Influencia de la guarida** (`Town::update_target_resistance`): una guarida con
  general o rey enlazada a un pueblo independiente baja la resistencia de todas sus
  razas hacia el reino hasta 45. La velocidad es la de un fuerte (la diferencia / 30,
  mínimo 1, cada 5 días) por el liderazgo / 100 del mejor general enlazado (mínimo 10):
  con liderazgo 60 baja un 60 % de lo que bajaría un fuerte.
- **Esclavizar:** hace falta una guarida del reino enlazada al pueblo (con o sin
  general). Sin ella, el pueblo no cambia de dueño aunque su resistencia o lealtad
  llegue a 0.
  - Independiente: resistencia a 0 peleando, como cualquier reino.
  - De un reino humano: atacar el pueblo baja la lealtad de su gente; cuando la de todas
    las razas que viven ahí llega a 0, queda esclavizado. (El juego original revisa
    también las razas sin población, que guardan su lealtad vieja, así que para el
    reino Fryhtan solo cuentan las que viven en el pueblo.) Si el pueblo estaba
    entrenando una unidad, se cancela.
- **El pueblo esclavizado:**
  - Lealtad fija de 50: no se rebela, su gente no migra a otros pueblos, no hay
    impuestos anuales, ni impuestos o regalos automáticos.
  - Panel sin botones; muestra "Enslaved town. At the end of the month you will
    receive $X as tribute."
  - Paga $5 por persona el día 1 de cada mes (ingreso "Tribute from Enslaved Towns" en
    el reporte de economía).
  - Produce comida y crece como cualquier pueblo.
  - Si pierde el enlace con todas las guaridas del reino (por ejemplo, si la destruyen o
    la venden), se vuelve independiente con resistencia 100 hacia ese reino.
  - Otro reino lo puede capturar como a cualquier pueblo.
- **Tope de población:** 18 + la gente de todos los pueblos esclavizados del reino.
- **Arreglo de paso:** al destruir o vender una guarida aparecía un fuerte humano (la
  animación de destrucción usa el gráfico del fuerte); ahora las guaridas no la tienen.
- **Probado:** esclavizar un pueblo independiente (resistencia inicial 41, a 0
  peleando), tributo ($70 con 14 personas), tope 10/32, liberación al vender la guarida,
  y esclavizar un pueblo de un reino de la IA (con un gancho temporal que le dio a la
  IA un pueblo enlazado a la guarida, porque los reinos de la IA estaban en otras islas).

### Ajustes anotados (después de la fase 4)
- [x] **Íconos de rango:** mostrar sobre el rey y los generales de un reino Fryhtan el
      ícono de rey y de general que tienen las unidades humanas.
- [x] **Recompensa por matar:** bajar solo el oro: **$2** por soldado, aldeano o defensor
      y **$10** por general. La comida no cambia (5 y 20) y el rey sigue en $100 y 100.
- [x] **Liderazgo y tributo:** el liderazgo del mejor general o rey de una guarida
      enlazada al pueblo esclavizado sube el tributo: tributo = $5 × personas × (1 +
      liderazgo / 100). Con liderazgo 100 se cobra el doble ($10 por persona). El texto
      del panel del pueblo muestra el tributo ya con el bono.
- [x] **Grant a pueblos independientes:** quitar el botón para los reinos Fryhtan (hoy
      aparece y no debería).
- [x] **Nombres de Ordos:** los generales Fryhtan (Ordos) del reino reciben un nombre
      propio al azar de la lista `7kmod/FRYHTAN_NAMES.md` (65 nombres, aprobada), sin
      repetir uno en uso en el reino. Se muestra como el rey: el nombre en la primera
      línea ("Gharvok") y "Wyrm Ordo" en la segunda. El rey sigue con el nombre del
      jugador.
- [x] **Train Ordo en lugar de Promote:** ya no se pueden ascender soldados a general.
      La guarida tiene un botón nuevo para entrenar un Ordo de su especie:
      - tarda el doble que un soldado (**40 días**);
      - sale con **liderazgo 30 y combate 30**;
      - aparece **afuera** de la guarida (no entra como soldado);
      - comparte la cola con Train y cuenta en el tope de población igual que un
        soldado de su tamaño.
      - cuesta **40 de comida** y no hay Demote (aprobado).
- [x] **Partidas guardadas:** en la lista de partidas (guardar y cargar), una partida de
      un reino Fryhtan muestra el retrato del Fryhtan de la especie del reino en lugar del
      rey humano de la raza interna (hoy sale un rey normando), y el nombre "<nombre> The
      All High" (por ejemplo "Al The All High") en lugar de "King Al". Hay que guardar la
      especie en el encabezado de la partida (`GameFileHeader`), porque la lista no
      carga la partida completa; las partidas de antes del cambio siguen mostrando el
      retrato humano.

#### Cómo quedaron

- **Íconos:** el rey y los Ordos de un reino Fryhtan llevan el ícono de rey o de general
  en lugar del ícono del reino; los soldados siguen con el ícono del reino.
- **Recompensa:** $2 / $10 / $100 de oro y 5 / 20 / 100 de comida.
- **Tributo:** $5 × personas × (100 + liderazgo) / 100, con el mejor general o rey de
  las guaridas enlazadas al pueblo (con el enlace activo).
- **Grant:** `Town::can_grant_to_non_own_town()` devuelve 0 para un reino Fryhtan.
- **Train Ordo:** segundo botón de la guarida (ícono de Promote), junto a Train. Cada
  botón muestra cuántos de su tipo hay en la cola y la barra del que se está
  entrenando; clic derecho quita el último de ese tipo. La cola es una sola (hasta 8)
  y se entrena en orden. El Ordo sale junto a la guarida con un nombre de la lista y
  no entra. Sin Promote ni Demote en el menú de las unidades Fryhtan.
- **Nombres:** se guardan en `Unit::name_id` (60000 + el número del nombre; los
  Fryhtans no usan ese campo para mostrar nada). Los Ordos ascendidos antes de este
  cambio no tienen nombre. Al azar entre los que no usa ningún Ordo del reino.
- **Lealtad:** el panel del general de la guarida muestra "Loyalty N/A" para un Ordo
  (antes salía "100 ↓ 0", aunque la lealtad de los Fryhtans no cambia).
- **Lista de partidas:** el encabezado tiene tamaño fijo, así que la especie se guarda
  en los dos últimos bytes del nombre del rey (después de su fin; el juego los ignora):
  una marca y el número de la especie. Si el nombre es muy largo (más de 18 letras) no
  cabe y sale el retrato humano. En el menú principal se carga `monster_res` para los
  retratos (antes no estaba cargado ahí).
- **Partidas guardadas:** las guaridas se guardan con un formato nuevo (con los Ordos
  de la cola), marcado con un -1 antes del número de guaridas; las partidas anteriores
  se cargan con todos los de la cola como soldados.
- **Probado:** Train Ordo (40 días, sale "Kzorr / Wyrm Ordo" con liderazgo y combate 30,
  ícono de general), construir una guarida con el Ordo y verlo como general, la lista de
  partidas con el retrato del Wyrm y "New Player The All High", cargar la partida, y el
  pueblo independiente sin Grant.

### Ajustes anotados (pueblos esclavizados sin general)
- [x] **Sin general no hay tributo:** si ninguna guarida enlazada al pueblo esclavizado
      (con el enlace activo) tiene un Ordo o el rey adentro, el tributo de ese mes no se
      cobra. El panel del pueblo lo dice en lugar del monto: "No Ordo commands a linked
      lair: no tribute, and its loyalty is dropping."
- [x] **La lealtad baja:** en ese caso la lealtad objetivo del pueblo es 0 (con general,
      50), y la lealtad se acerca a ella a la velocidad normal de un pueblo.
- [x] **Rebelión contra la guarida:** al llegar al punto de rebelión (29 o menos, como en
      el juego original: dos tercios de la gente descontenta, no más de una cada 3
      meses), sale del pueblo el 30–60 % de cada raza descontenta, como en una rebelión
      normal, y en lugar de quedarse junto al pueblo van a atacar la guarida del reino
      más cercana (`REBEL_ATTACK_FIRM`). Si la destruyen y el pueblo se queda sin enlace,
      se vuelve independiente con la regla de la fase 4.
- **Probado** (con un gancho temporal que esclavizó el pueblo enlazado a la guarida
  inicial): con la guarida vacía, el panel avisa, la lealtad baja, se rebelaron 12
  personas y fueron contra la guarida.

### Fase 5: capturar guaridas independientes
- [x] Enlace guarida propia–guarida independiente, de cualquier especie.
- [x] Oferta simulada (Ordo y soldados), guardada en la partida.
- [x] Panel de la guarida salvaje con la oferta y Pay Tribute; al pagar, la guarida pasa
      a tu reino con el Ordo y sus soldados (se puede pasar del tope de población).
- [x] Train en cada guarida con su propia especie.

#### Cómo quedó la fase 5

- **Enlace:** una guarida salvaje (`FirmMonster`) queda enlazada si una guarida
  terminada del reino está a 8 casillas o menos (la distancia de enlace de un fuerte con
  un pueblo). El juego no enlaza fuertes con otros edificios, así que no se dibuja una
  línea: el enlace se ve en el panel de la guarida salvaje.
- **Oferta:** la primera vez que se ve el panel de una guarida salvaje enlazada se crea la
  oferta y queda fija (se guarda en la partida). Usa su propio generador de números al
  azar, para no cambiar la secuencia del juego al mirarla.
  - Ordo de la especie de la guarida, con el combate del mejor Ordo de la guarida
    salvaje (si no tiene, el de su rey) y liderazgo 30–60.
  - 4–6 soldados de una especie grande o 6–8 de una pequeña, con combate 20–40.
  - Tributo: la suma de los combates × $2 (gasto "tribute" del reino).
- **Panel:** en lugar de los Ordos de la guarida salvaje se ve "Tribute Offer": el retrato
  y el combate y liderazgo del Ordo, cuántos soldados y su rango de combate, el tributo y
  el botón **Pay Tribute**. Sin dinero suficiente avisa y no paga.
- **Al pagar:** los Fryhtans salvajes de adentro desaparecen (no salen al mapa) y los que
  estaban afuera siguen salvajes; la guarida salvaje no deja monedas de oro. En su lugar
  queda una guarida del reino de esa especie (3x3 en la esquina de la de 4x4), con el
  Ordo (con nombre) como general y los soldados adentro. Se puede pasar del tope de
  población.
- **Train** en una guarida capturada entrena su especie (y cuenta su tamaño en la
  población).
- **Probado** (con un gancho temporal que construyó una guarida junto a una guarida
  salvaje): oferta de una guarida Rattus (Ordo combate 63 y liderazgo 36, 6 soldados de
  22–39, $482), Pay Tribute, la guarida Rattus del reino con "Naggol / Rattus Ordo" y
  sus 6 soldados, población 19/18, y Train de Rattus.
- **Bug corregido:** el juego se cerraba al pagar el tributo si antes estaba seleccionado
  otro edificio con soldados (pasaba con una partida cargada): la lista de soldados del
  panel usa un orden que se calcula solo para el edificio seleccionado
  (`Firm::sort_worker()`), y la guarida nueva se mostraba con el orden del anterior. Se
  recalcula al seleccionarla. También se agrandó el recuadro de la oferta para que se
  lea todo el texto.
- **Pendiente para la fase 6:** los Fryhtans salvajes todavía atacan al reino, así que
  construir junto a una guarida salvaje es peligroso hasta entonces.

### Ajustes anotados (después de la fase 5)
- [x] **Tributo de captura ×5:** el costo de una guarida salvaje pasa de la suma de los
      combates × $2 a la suma × **$10**. Por ejemplo, la oferta Rattus de la prueba
      (Ordo 63 + soldados 178) costaría $2,410 en lugar de $482.
- [x] **Línea de enlace:** dibujar una línea entre cada guarida del jugador y las
      guaridas salvajes enlazadas a ella (a 8 casillas o menos), como la línea punteada
      de un fuerte con un pueblo. Se ve al seleccionar la guarida del jugador o la
      guarida salvaje.
- [x] **Hace falta un general:** solo se puede capturar (Pay Tribute) si alguna guarida
      del jugador enlazada a la guarida salvaje tiene un Ordo o el rey adentro. Sin
      general, la oferta se ve pero el botón está desactivado y el panel lo explica.
- [x] **El liderazgo cambia el costo:** se usa el general (Ordo o rey) con más liderazgo
      de las guaridas enlazadas. Con liderazgo 100 se paga el costo base (suma de los
      combates × $10); con 30, el triple; en medio, gradual:
      costo = base × (1 + 2 × (100 − liderazgo) / 70). Por ejemplo, ×2 con liderazgo 65
      y ×2.14 con el rey al inicio (liderazgo 60). Con liderazgo menor a 30 se queda en ×3. El
      panel muestra el costo ya con el liderazgo.

#### Cómo quedaron

- **Costo:** `ModFryhtan::capture_cost()` = suma de combates × $10 × (70 + 2 × (100 −
  liderazgo)) / 70, con el liderazgo entre 30 y 100. El panel muestra "Tribute: $X
  (leadership Y)". Sin general muestra "Base tribute: $X", el aviso "An Ordo or the All
  High must be in a linked lair to pay the tribute." y no hay botón (el estilo de botón
  desactivado del juego casi no se distingue).
- **Línea:** se dibuja al seleccionar tu guarida (hacia las guaridas salvajes enlazadas)
  o una guarida salvaje (hacia tus guaridas enlazadas). Es animada si tu guarida tiene
  general, y fija si no, como el enlace de un fuerte con un pueblo.
- **Probado:** con el Ordo afuera, aviso y sin botón; con el Ordo adentro (liderazgo 30),
  tributo de $5,940 por una oferta Droog de base $1,980 (×3), pago y captura.

### Ajustes anotados (después de la fase 6)
- [x] **Línea de enlace al colocar la guarida:** al elegir dónde construir una guarida
      (Build → Lair, con el cuadro de colocación siguiendo al mouse), se dibujan las
      líneas hacia las guaridas salvajes que quedarían enlazadas desde ese lugar (a 8
      casillas o menos), igual que al colocar un fuerte se ven las líneas hacia los
      pueblos que enlazaría (`World::draw_link_line`). Así se sabe antes de construir
      qué guaridas salvajes se podrán capturar desde ahí. Se siguen viendo también las
      líneas normales del fuerte hacia los pueblos.
      Hecho en `World::draw_link_line()`; probado con un Ordo junto a una guarida
      salvaje.

### Ajustes anotados (después de la fase 7)
- [x] **Pestañas Humanos / Fryhtans en New Game:** en la página básica, encima de los
      recuadros de las razas, dos pestañas: **Humans** y **Fryhtans**.
      - **Humans** (la de inicio): los recuadros de las 10 razas humanas, como en el
        juego original.
      - **Fryhtans:** en el mismo lugar, recuadros con las 13 especies (retrato de su All
        High y nombre), para elegir una con un clic, igual que una raza.
      - Se quitan el botón "Fryhtan" y el selector con flechas de la fase 1; la línea
        "Fryhtan kingdom: <nombre> <especie> Horde (Large/Small: N population)" se queda
        debajo cuando la pestaña Fryhtans está activa.
      - Aprobado: los 13 recuadros caben haciéndolos un poco más chicos o en dos filas
        (se elige al implementarlo según lo que se vea mejor), y la pestaña elegida se
        recuerda en 7KMOD.DAT, como ya se recuerda la especie.
      - **Cómo quedó:** las pestañas son dos textos encima de la primera fila, "Humans" y
        "Fryhtans", con la letra inclinada de las etiquetas; la elegida va en negrita y
        subrayada (pedido después de una primera versión con recuadros a la derecha).
        Con Humans las 10 razas siguen 5 x 2 como en el original. Con Fryhtans hay 6
        columnas del tamaño de las razas: 5 x 2 especies por página (en orden
        alfabético) y en la 6.ª las flechas ▲ (arriba) y ▼ (abajo), que cambian de
        página (pedido después de una versión de 7 x 2 sin páginas). Página 1: Broosken
        a Rattus; página 2: Rokken, Sauroid y Wyrm, y 7 recuadros vacíos. La flecha sin
        página a dónde ir se ve oscurecida. La especie elegida se mantiene al cambiar de
        página y la pantalla abre en su página. Los nombres usan la misma letra (la más grande en la que caben todos). Son
        recuadros con el nombre, como los de las razas; los retratos no caben en ese
        tamaño. La pestaña es la elección misma (`Config::fryhtan_monster_id`, que ya se
        guarda en 7KMOD.DAT), así que no hizo falta otro dato.
- [x] **Letra suavizada en New Game:** los nombres de las especies, las pestañas
      Humans/Fryhtans y la etiqueta "Seats of Power Unlocked?" se veían pixelados con
      las letras del juego. Ahora usan texto prerenderizado con bordes suavizados en
      **TeX Gyre Pagella** (clon libre de Palatino, licencia GUST): negrita cursiva
      para las especies y la pestaña elegida, cursiva normal para la otra pestaña y la
      etiqueta, que así se parecen a las demás etiquetas de la pantalla.
      - El texto se genera con `7kmod/tools/make_smooth_text.py` (Pillow) en
        `include/ModSmoothText.h`: cada pixel guarda cuánto oscurecer el fondo (0 a 10)
        y el juego lo aplica con la tabla de colores, así se mezcla con la textura de
        piedra. En el repositorio solo va el header generado, no la fuente.
      - Si el texto está traducido y no existe prerenderizado, se usa la letra del
        juego como antes.
- [x] **Entrenamiento más rápido para especies chicas:** las especies de 1 de población
      (Small) entrenan en la mitad del tiempo: soldado **10 días** (en vez de 20) y
      Ordo **20 días** (en vez de 40). Las especies grandes (2 de población) siguen con
      20 y 40 días. La comida no cambia. La barra de progreso y el texto de ayuda del
      botón muestran los días de la especie.
      - **Cómo quedó:** `ModFryhtan::train_days()` recibe la especie y divide entre 2
        cuando `monster_population()` es 1. Probado con Broosken: la ayuda dice 10 y 20
        días y los soldados salen a tiempo; Karrotten (grande) sigue en 20 y 40.

### Fase 6: diplomacia y Fryhtans salvajes
- [x] Sin comercio, amistad ni alianza con un reino Fryhtan (jugador e IA).
- [x] Los reinos de la IA no se pueden rendir a un reino Fryhtan (ni ofrecer la
      rendición ni aceptarla).
- [x] Los Fryhtans salvajes no atacan al reino Fryhtan.

#### Cómo quedó la fase 6

- **Diplomacia** (`TalkRes::can_send_msg`): entre un reino Fryhtan y cualquier otro no
  se pueden enviar (ni el jugador ni la IA) las propuestas de comercio, amistad, alianza,
  embargo comercial, compra de comida ni rendición (pedirla u ofrecerla). Quedan
  declarar la guerra, pedir el cese al fuego, ofrecer o exigir tributo y ayuda, y
  tecnología. En el reporte de reinos, con la IA neutral solo aparecen "Declare war",
  "Offer to pay tribute" y "Demand tribute". También se quitó comprar comida, que en la
  fase 2 se había visto que la IA ofrecía.
- **Rendición:** la IA no elige a un reino Fryhtan al pensar en rendirse
  (`Nation::think_surrender`), y `NationBase::surrender()` no hace nada si alguno de los
  dos reinos es Fryhtan (cubre también al espía que hereda el trono).
- **Fryhtans salvajes:** las guaridas no eligen como objetivo edificios ni pueblos de
  un reino Fryhtan (`FirmMonster::think_attack_human`, `think_attack_neighbor`), las
  unidades no los atacan al buscar objetivos (`UnitMonster::random_attack`) ni al
  estar paradas (`Unit::independent_nation_can_attack`). Los salvajes sí se defienden
  si el reino los ataca, porque eso marca al reino como hostil para esa guarida.
- **Probado:** el menú de diplomacia y 19 años a velocidad máxima: ningún Fryhtan
  salvaje atacó al reino (comprobado con una traza temporal de los ataques). El reino
  de la prueba, que no hizo nada, fue destruido por un reino humano de la IA con
  soldados y armas de asedio, lo que es normal.

### Fase 7: cierre
- [x] Reportes (F1–F8) con sentido para un reino Fryhtan.
- [x] Condición de derrota y sucesión del rey.
- [x] Guardar y cargar.
- [x] Pruebas en Linux y ejecutable de Windows.
- [x] `CHANGELOG-fryhtan.md` y `CHANGELOG-fryhtan.en.md` (creados en la fase 1; se
      actualizan en cada fase).

#### Cómo quedó la fase 7

- **Sucesión** (`ModFryhtan::think_succeed_king`, cada día sin rey): el nuevo All High es
  el Ordo con más combate (en el mapa o al mando de una guarida); si no hay Ordos, el
  Fryhtan con más combate en el mapa; si tampoco, el soldado con más combate de una
  guarida (sale de ella). Se queda con su nombre de Ordo (o recibe uno de la lista) y
  el reino toma ese nombre: "Zhovrek The All High", "Zhovrek Wyrm Horde". El nombre se
  guarda como el nombre del jugador (`NationArray::human_name_array`), así todo lo que
  muestra el nombre del reino o del rey sigue funcionando.
  - Noticias: "Your All High has been slain." y "<nombre> is your new All High.".
  - Si no queda ningún Fryhtan pero sí guaridas, el reino sigue sin rey y el primer
    Fryhtan que se entrene pasa a ser el All High.
- **Derrota:** el reino pierde cuando no tiene rey, ni guaridas, ni unidades Fryhtan
  (`NationBase::check_lose`). Antes solo contaba humanos y pueblos.
- **Fin de partida:**
  - Textos "Your Horde has …" en lugar de "Your Kingdom has …" (derrota, tiempo
    límite y metas).
  - Imagen: al ganar, la de los Fryhtans sobre el campo de batalla (`LOSEGAME`, la que
    el juego usa cuando pierdes contra ellos); al perder, la de la raza humana interna
    (los humanos celebrando).
  - "Final Population" suma los Fryhtans.
- **Hall of Fame:** "<nombre> The All High" (con la misma marca de especie que la lista
  de partidas) y la población con los Fryhtans.
- **Reportes:**
  - Villages: los fuertes de la lista de edificios se llaman "Lair" con su costo real
    ($100 al año).
  - Military: "In Lair" y sin la fila "Human General".
  - Ranking: el puntaje de población suma los Fryhtans del reino.
  - Kingdoms (fases 2 y 6), Economy (fases 3 y 4), Trade, Technology y Espionage se
    ven bien sin cambios (vacíos para un reino Fryhtan).
- **Guardar y cargar:** probado con una partida del jugador de la versión anterior
  (con una oferta pendiente): se carga, se guarda y se vuelve a cargar con la oferta y
  la lista de partidas con el retrato del Rattus.
- **Probado** (con ganchos temporales que mataron al rey): sucesión con un Ordo
  ("Zhovrek"), sin Ordos (sale el mejor soldado de la guarida, "Vazzrik") y derrota al
  destruir la única guarida con todos adentro.

---

## 5. Riesgos

- **Reino sin raza humana:** es lo más delicado. El juego da por hecho en muchos lugares
  que todo reino tiene una raza; un caso que se escape puede tronar el juego o mostrar
  textos raros. Se revisa en la fase 1, antes de construir lo demás encima.
- **IA de los reinos humanos:** su diplomacia y sus ataques no esperan un reino Fryhtan.
  Hay que probar que no se atore (por ejemplo, proponiendo tratos que no se pueden
  hacer).
- **Guarida tipo fuerte:** usar la lógica del fuerte con el aspecto de la guarida puede
  chocar con los lugares donde el juego revisa el tipo de edificio (`FIRM_CAMP` o
  `FIRM_MONSTER`). Se decide en la fase 2 si conviene un tipo de edificio nuevo.

---

## 6. Fuera de la versión 1

- Reinos Fryhtan de la IA.
- Multijugador.
- Reinos Fryhtan en el editor de escenarios.
- Greater Beings, espías y tecnologías para un reino Fryhtan.

---

## 7. Actividades después de la versión 1

Ajustes y cosas nuevas para el reino Fryhtan, ya con la versión 1 en `7kmod-gameplay`.
Se prueban aquí en `7kmod-fryhtan` y luego se hace merge en gameplay.

- [x] **Periodo de gracia de 1 mes al esclavizar un pueblo.** Al esclavizar un pueblo,
      su gente queda con lealtad 30 (`Town::set_nation`, como cualquier pueblo que cambia
      de dueño). Pero el general suele estar afuera peleando, así que ninguna guarida
      enlazada tiene un Ordo o el rey adentro y la lealtad objetivo es 0: en unos 5 días
      baja a 29 (punto de rebelión) y, como el juego revisa la rebelión cada 15 días, el
      pueblo se puede rebelar antes de que el general regrese a la guarida. Con el
      cambio, durante el primer mes después de esclavizarlo el pueblo no se puede
      rebelar, para darle tiempo al general de regresar (con general, la lealtad sube
      hacia 50).
      - Idea para implementarlo sin datos nuevos en la partida guardada: al esclavizar,
        poner `last_rebel_date` 2 meses en el pasado; como el juego no deja rebelarse
        dos veces en menos de 3 meses (`REBEL_INTERVAL_MONTH`), eso da exactamente un mes.

- [x] **Empezar con un pueblo esclavizado de 20 personas.** Hoy el reino Fryhtan empieza
      solo con su guarida (el All High y 4 soldados adentro). Con el cambio, también
      empieza con un pueblo esclavizado de 20 personas, ya enlazado a la guarida inicial
      (cerca de ella, a distancia de enlace), como lo haría un reino humano con su pueblo
      inicial.
      - Con el rey en la guarida hay general al mando, así que el pueblo paga tributo
        desde el primer mes ($5 × 20 = $100) y su lealtad va hacia 50.
      - El tope de población sube con la gente esclavizada (base + 20).
      - La gente del pueblo es de **una raza al azar**.

- [x] **Tope de población base de 20** (hoy 18). El tope sigue siendo la base + la gente
      de los pueblos esclavizados; con el pueblo inicial, el reino empezaría con 40.

- [x] **Los Fryhtans cuestan más población:** las especies grandes pasan de 2 a **3** y
      las pequeñas de 1 a **2**.
      - Cambia el texto de las especies en New Game ("Large: 2 population" → 3,
        "Small: 1 population" → 2).
      - Ojo: el entrenamiento a la mitad de tiempo de las especies pequeñas hoy revisa que
        cuesten 1 de población; hay que cambiarlo para que revise que la especie sea
        pequeña (si no, ninguna especie entrenaría más rápido).
      - El rey y los 4 soldados del inicio ocupan 15 (grandes) o 10 (pequeñas) del tope.

#### Cómo quedaron

- **Gracia:** `Town::set_nation` pone `last_rebel_date` 2 meses en el pasado cuando el
  nuevo dueño es un reino Fryhtan.
- **Pueblo inicial:** `ModFryhtan::create_start_king` crea el pueblo (raza al azar, 20
  personas, lealtad 50) donde un reino humano tendría su pueblo inicial, y la guarida 6
  casillas a la derecha, como el fuerte inicial humano; quedan enlazados.
- **Población:** `FRYHTAN_BASE_POPULATION` 20, `FRYHTAN_SMALL_POPULATION` 2 y
  `FRYHTAN_LARGE_POPULATION` 3; el entrenamiento a mitad de tiempo usa
  `is_large_monster()`.
- **Probado** con un Rokken: pueblo maya de 20 enlazado a la guarida, tributo de $160
  (con el liderazgo 60 del rey) y la ayuda de Train dice "uses 3 population. Fryhtan
  population: 15/40". La gracia no se probó en partida.

- [x] **Los Ordos construyen guaridas de su especie.** Hoy toda guarida que construye el
      reino es de la especie principal (`Firm::init` le pone la especie del reino), sin
      importar quién la construye. Un reino puede tener Ordos de otras especies (por
      ejemplo, el de una guarida salvaje capturada); con el cambio, la guarida que
      construye un Ordo es de la especie de ese Ordo (y la del rey, de la especie del
      rey). Hay que tomar la especie del constructor al empezar la construcción.
- [x] **Train entrena la especie de la guarida.** El soldado y el Ordo que se entrenan
      en una guarida deben ser de la especie de esa guarida (su ícono, su nombre en la
      ayuda de Train, su costo de población y su tiempo de entrenamiento).
      - Según el código, Train ya usa la especie guardada en la guarida, y una guarida
        capturada guarda la especie salvaje. El caso que hoy falla es el de una guarida
        construida por un Ordo de otra especie (sale de la especie principal, así que
        entrena la principal); se arregla con el punto anterior. Hay que revisar también
        las guaridas capturadas al probarlo.

- [x] **Sin guaridas salvajes de la especie del jugador** (balance). Si el jugador elige,
      por ejemplo, Rattus, en el mapa no aparecen guaridas salvajes de Rattus.
      - Las guaridas salvajes del inicio salen de las especies "activas" de la partida
        (`MonsterRes::generate`, que escoge de `active_monster_array`); la especie del
        reino Fryhtan del jugador se quita de esa lista al empezar.
      - Con eso tampoco aparecen después: las guaridas salvajes nuevas salen de otras
        guaridas salvajes de su misma especie (`OF_MONS.cpp`).

- [x] **Sonido "ready" al terminar de entrenar** un soldado o un Ordo en la guarida, como
      cuando un pueblo humano termina de entrenar una unidad.
      - Los Fryhtans no tienen sonido "RDY" propio en `RESOURCE/STD.SET` (solo ataque y
        muerte), pero todas las razas humanas usan el mismo archivo `READY`, que también
        suena cuando la fábrica de guerra termina un arma. Se usa ese mismo sonido, solo
        para las guaridas del jugador y si la guarida está a la vista, como en el
        original (`se_res.far_sound`).

#### Cómo quedaron (guaridas por especie, guaridas salvajes y sonido)

- **Guarida del constructor:** `Firm::init` toma la especie del Fryhtan que la construye
  (`ModFryhtan::unit_monster_id`); sin constructor (guarida inicial) usa la del reino. La
  guarida capturada sigue tomando la especie salvaje.
- **Train:** ya usaba la especie de la guarida; con el punto anterior, una guarida
  construida por un Ordo de otra especie entrena esa especie.
- **Guaridas salvajes:** `MonsterRes::init_active_monster` salta la especie del reino
  Fryhtan del jugador.
- **Sonido:** `ModFryhtan::next_day_lair` toca el sonido "FINS" de la fábrica de guerra
  (el archivo `READY`) cuando sale un Fryhtan de una guarida del jugador.
- **Probado** (con cambios temporales para la prueba): con un reino Rokken, un Ordo
  Deezboanz construyó una guarida Deezboanz, cuya ayuda de Train dice "Train a
  Deezboanz ... in 10 days ... uses 2 population" y que entrenó un Deezboanz; en dos
  partidas las especies salvajes activas no incluyeron a los Rokken; el sonido se
  llamó al salir el soldado (en la nube no hay audio para oírlo).

- [x] **Saqueo de pueblos.** Cuando los Fryhtans del reino atacan un pueblo (de otro
      reino o independiente), **cada aldeano que muere da el mismo bono que matar a un
      aldeano**: $2 de oro y 5 de comida (`KILL_GOLD_UNIT` y `KILL_FOOD_UNIT`). Hoy ese
      bono solo se da al matar unidades (`ModFryhtan::reward_kill`); la gente que muere
      dentro del pueblo al atacarlo no da nada.
      - **No afecta a los edificios:** atacarlos no da oro ni comida.
      - El bono sale de la nada, como el de matar unidades: no se le quita al reino
        dueño.
      - Relación con la tecnología de los **Ick** ("Saqueadores", en
        `DISENO-fryhtan-oscuro.md`): podría duplicar este bono para esa especie.

- [x] **Saquear pueblos independientes (botón Raid).** Una forma de bajar la resistencia
      de un pueblo independiente sin asaltarlo, provocando a sus defensores para pelear
      contra ellos en tu guarida.
      - **Botón "Raid"** en el panel de un pueblo independiente, en el lugar de Grant
        (que los Fryhtans no tienen). Aparece si hay una guarida enlazada con general y al
        menos un soldado.
      - **Espera de 1 mes por pueblo**, que empieza al presionarlo: el botón muestra una
        **barra de progreso** (como la de Train en la guarida) y no se puede usar hasta que
        se llena. Varias guaridas enlazadas no permiten saquear más seguido.
      - **Ayuda del botón**, con lo que se espera: "Raid this village: steal about $20
        and 20 food. About 3 defenders (combat ~25) will attack your lair."
      - **Al saquear:**
        1. **Botín:** $1 y 1 de comida por persona del pueblo, con tope de $30 y 30 de
           comida. Los aldeanos que mata no dan además el bono de saqueo de pueblos.
        2. **Aldeanos muertos** según el tamaño: pueblo chico (menos de 20) 25% de
           probabilidad de 1; mediano (20 a 39) 1; grande (40 o más) 1 o 2.
        3. **Contraataque:** salen hasta **1 defensor por cada 10 personas** (con la regla
           normal del juego: resistencia de 30 o más y al menos 5 × los defensores afuera)
           y atacan la guarida enlazada con el mejor general. Cada defensor se lleva su
           parte de la resistencia (resistencia / personas de su raza, `Town::mobilize_defender`):
           si la guarida lo mata, la resistencia baja para siempre; si sobrevive y regresa
           al pueblo, la devuelve. Su combate es el del pueblo ± 10 (depende de la opción
           "Independent Town Resistance" de New Game, no de la resistencia).
        4. **Con resistencia menor de 30** (ya no salen defensores): baja la resistencia
           de 5 a 10 directamente.
        5. **Mensaje:** por ejemplo, "Raided Tikal: $20 and 20 food, 1 villager killed.
           3 defenders are attacking your lair."
      - **No cambia:** la guarida sigue bajando la resistencia sola hasta 45; atacar el
        pueblo sigue siendo la vía rápida; al llegar a 0 queda esclavizado.
      - **Ritmo de ejemplo:** pueblo de 30 con resistencia 80, matando a los 3 defensores
        cada vez: unos 6 meses para llegar a 30 y 8 a 9 meses para esclavizarlo, con
        unas 9 personas al final. Si la guarida no puede con los defensores, no baja y
        se pierden soldados.
      - **Técnico:** guardar en la partida el día del último saqueo de cada pueblo (sube
        la versión de partida de Fryhtan; las partidas anteriores se siguen cargando), un
        mensaje de red para multijugador, y la orden de atacar la guarida para los
        defensores, como la de los rebeldes de los pueblos esclavizados
        (`REBEL_ATTACK_FIRM`), sin quitarles la resistencia que se llevan.

#### Cómo quedaron (saqueo y Raid)

- **Saqueo al atacar:** `Town::being_attacked`, después de `kill_town_people`, llama a
  `ModFryhtan::reward_town_villager` ($2 y 5 de comida). Los muertos del Raid no pasan
  por ahí, así que no dan el bono.
- **Raid:** `ModFryhtan::raid_town`; el botón (`Town::disp_raid_button`, con el ícono de
  Collect Tax) y su ayuda están en `OTOWNIF.cpp`. Los defensores salen con
  `Town::mobilize_defender` (así llevan su parte de la resistencia y, si regresan, la
  devuelven) y reciben la orden `attack_firm` contra la guarida del mejor general. Con
  la resistencia en 0, el pueblo se rinde y queda esclavizado.
- **Noticia nueva** `NEWS_TOWN_RAIDED`: "Raided <pueblo>: $X and X food, N villagers
  killed. N defenders are attacking your lair." o "... Its resistance has dropped."
- **Fechas:** el día del último saqueo de cada pueblo se guarda al final de los datos de
  `ModFryhtan`, como la gracia (las partidas anteriores se cargan sin ellas). Al crear
  un pueblo o al cambiar de dueño se borran sus fechas (`reset_town_dates`).
- La ayuda de los botones admite ahora 400 caracteres (antes 200, cortaba el texto).
- **Multijugador:** sin mensaje de red, como el resto del reino Fryhtan (solo un
  jugador).
- **Probado** (con cambios temporales para la prueba): pueblo de 26 con resistencia 49
  → $26 y 26 de comida, 1 muerto, 2 defensores que fueron a la guarida y los soldados
  Rokken salieron a pelear; el botón quedó deshabilitado con la barra avanzando. Con
  resistencia 8 → sin defensores, bajó a 0 y el pueblo se rindió (esclavizado, tributo
  de $149). Atacando un pueblo con 8 soldados, cada aldeano muerto dio $2 y 5 de comida.
- **Observado en la prueba:** el clic automático no funcionó cuando ya se veía la ayuda
  del botón (sí funcionó al dar clic enseguida). Revisar si pasa también al jugar, y si
  pasa con otros botones.

#### Ajustes al Raid (después de probarlo)

- [x] **All High del inicio con liderazgo 100** (antes 60; combate 60 igual). Un Ordo que
      lo sucede conserva el suyo.
- [x] **Raid cada 2 meses** (`RAID_INTERVAL_DAYS` 60).
- [x] **Más defensores:** hasta 1 por cada 10 personas, más 1, más 1 si la resistencia
      pasa de 50 o más 2 si pasa de 80 (`ModFryhtan::raid_max_defender`), con la regla
      normal del juego.
- [x] **El primero es el líder de la defensa:** combate del pueblo + 20 (con su vida), y
      se lleva el doble de resistencia (si muere baja el doble; si regresa, devuelve el
      doble).
- [x] **Baja base de 5** por Raid (`RAID_RESISTANCE_DEC`), después de que salen los
      defensores, en todas las razas.
- [x] **Solo con resistencia mayor de 30:** en 30 o menos el botón queda oscurecido y su
      ayuda dice "This village is weakened and ready to be attacked directly." (se quitó
      la baja directa de 5 a 10 sin defensores).
- [x] **Por ahora no mata aldeanos** (`RAID_KILL_VILLAGERS` 0; la regla por tamaño queda
      en el código).
- [x] **Botón Raid y vista de Raid** (cambiado del panel fijo): en el panel normal del
      pueblo, el botón pequeño con el ícono de Collect Tax (`Town::disp_raid_button`). Solo
      se puede presionar con resistencia mayor de 30, guarida con soldados y sin espera;
      si no, se ve oscurecido y su ayuda dice por qué ("This village is weakened and ready
      to be attacked directly.", "You can raid this village again in N days." con la
      barra de espera, o "Your lair needs soldiers..."); si se puede, su ayuda explica qué
      hace y que es cada 2 meses. Al presionarlo se abre la **vista de Raid**
      (`TOWN_MENU_RAID`, `Town::disp_raid_menu`) en el lugar de la lista de razas: ícono de
      la raza principal, "Population" y "Resistance"; "Raid <pueblo>?", lo que se obtiene,
      cuántos defensores atacarán la guarida y su combate; y los botones **Raid village**
      (saquea y regresa) y **Cancel**.
- **Ejemplo** de la prueba: pueblo de 16 con resistencia 86 → 4 defensores (1 + 1 + 2),
  resistencia a 55 al salir (el líder se lleva el doble, más la baja base de 5).

- [x] **Ajuste de tributo y Raid.**
      - **Tributo de los pueblos esclavizados:** baja de $5 a **$3 por persona** al mes
        (`TRIBUTE_PER_PERSON`). El liderazgo del general (o el rey) de la guarida enlazada
        lo sigue subiendo como hoy: x2 con liderazgo 100.
      - **Raid:** sube de $1 y 1 de comida a **$2 y 2 de comida por persona**.
      - **El liderazgo mejora el Raid:** lo que se obtiene se multiplica por
        (100 + liderazgo) / 100, con el liderazgo del general de la guarida que hace el
        Raid (`raid_lair`): **x2 con liderazgo 100**, igual que el tributo.
      - Tope (`RAID_MAX_LOOT`, hoy 30): pasa a 60 antes del liderazgo, para que siga
        contando hasta 30 personas (confirmado). Con liderazgo 100, como mucho $120 y 120 de comida.
      - Actualizar el texto de la vista de Raid (lo que se obtiene) y la noticia.
      - Ejemplo: pueblo de 16 y general con liderazgo 60 → 16 × 2 × 1.6 = **$51 y 51 de
        comida** (hoy $16 y 16).
      - **Cómo quedó:** `TRIBUTE_PER_PERSON` 3; `RAID_LOOT_PER_PERSON` 2 y `RAID_MAX_LOOT`
        60; `ModFryhtan::raid_loot` calcula lo robado (lo usan `raid_town` y la vista de
        Raid); la noticia ya mostraba la cantidad robada.

- [x] **Gracia de 3 meses al perder la guarida de un pueblo esclavizado.** Hoy, en cuanto
      un pueblo esclavizado se queda sin guarida enlazada (`ModFryhtan::next_day_town`),
      se vuelve independiente ese mismo día. Con el cambio:
      - Al quedarse sin guarida enlazada, sigue siendo tuyo **3 meses**. Si en ese tiempo
        vuelve a tener una guarida enlazada (construyes otra o recuperas el enlace), la
        cuenta se cancela.
      - Mientras tanto no paga tributo (no hay general al mando) y su lealtad baja, como
        hoy sin general.
      - **Si no hay ninguna guarida cercana, no se rebela** (los rebeldes no tendrían a
        qué guarida atacar); solo espera a que terminen los 3 meses.
      - Al terminar los 3 meses se declara independiente, con una **noticia**: "Your
        village %s has declared independence." (o el texto que se use para los pueblos
        humanos que se independizan).
      - Hay que guardar en la partida desde cuándo se quedó sin guarida (sube la versión
        de partida de Fryhtan).

- [x] **Bug: el pueblo que se independiza se rinde enseguida a otro reino.** En una
      partida, al eliminar la guarida, el pueblo esclavizado se volvió independiente un
      momento y luego se rindió a otro reino.
      - Causa probable (revisar): al independizarse solo se pone en 100 su resistencia
        hacia el reino Fryhtan. Hacia los demás reinos se queda con los valores que tenía
        guardados de antes (por ejemplo 0, si antes era de un reino humano o nunca se
        calcularon), y `Town::update_resistance` hace que un pueblo independiente con
        resistencia 0 hacia un reino se rinda a ese reino.
      - Arreglo propuesto: al independizarse, poner su resistencia hacia todos los reinos
        como la de un pueblo independiente nuevo (60 a 100, `Town::init`), y 100 hacia el
        reino Fryhtan como hoy.
      - Revisar si pasa lo mismo cuando un pueblo esclavizado se rebela.

#### Cómo quedaron (gracia y bug)

- `ModFryhtan::next_day_town` guarda el día en que el pueblo se quedó sin guarida
  (`lost_lair_date_array`, por número de pueblo; se borra al volver a tener guarida y en
  `Town::set_nation`). A los 90 días (`LOST_LAIR_GRACE_DAYS`) se independiza.
- `Town::think_rebel` no rebela a un pueblo esclavizado sin guarida enlazada
  (`ModFryhtan::in_lair_grace`).
- Noticia: `NEWS_TOWN_REBEL` con 0 rebeldes = "Your village of %s has declared
  independence."
- Al independizarse, resistencia de 60 a 100 hacia todos los reinos y 100 hacia el reino
  Fryhtan.
- Partida guardada: los días se guardan al final de los datos de `ModFryhtan` (las
  partidas anteriores se cargan sin ellos), sin subir la versión de partida.
- **Probado** (con cambios temporales para la prueba): sin la guarida y con resistencia 0
  hacia todos los reinos, el pueblo se independizó a los 90 días exactos, salió la
  noticia y no se rindió enseguida (un reino lo tomó meses después, de la forma
  normal). Guardando a los 11 días y cargando, se independizó el mismo día. La parte de
  no rebelarse durante la gracia no se probó en partida. El caso del pueblo que se
  rebela queda por revisar.

- [x] **Entrenar soldados Fryhtan más rápido.** Un soldado de especie grande tarda **15**
      días (antes 20; `LAIR_TRAIN_DAYS`) y uno de especie pequeña sigue en **10**
      (`LAIR_SMALL_TRAIN_DAYS`). El Ordo queda igual (40 / 20 días). La barra de progreso y
      la ayuda de Train muestran los días nuevos (probado: "Train a Rokken ... in 15 days").

- [x] **Prueba: sin límite de población y entrenar cuesta oro.** El tope de población
      Fryhtan queda desactivado (`FRYHTAN_POPULATION_LIMIT` 0; el cálculo sigue en el
      código). Entrenar un soldado o un Ordo cuesta **$150** (especie pequeña) o **$200**
      (grande), además de la comida de siempre (20 / 40); se cobra al ponerlo en la cola y
      se devuelve con clic derecho. El Ordo tarda **20** (pequeño) o **30** días (grande),
      antes 20 / 40. La ayuda de Train dice el oro y la comida; el reporte de reinos
      muestra solo cuántos Fryhtans hay; New Game dice "Large: $200 per unit" / "Small:
      $150 per unit". Probado: Train de Rokken cobró $200 y 20 de comida, y el clic
      derecho los devolvió.

- [x] **Bug: unidades humanas que se unen al reino Fryhtan a mitad de partida.** En
      pruebas se unieron unidades humanas al reino Fryhtan; un reino Fryhtan no debe
      tener unidades humanas (fuera de las del diseño de la economía oscura).
      - **Caso visto:** salió el mensaje de que se unió una unidad, que venía saliendo de
        un pueblo, y era un espía. Es casi seguro **un espía enemigo disfrazado de tu
        reino** (por ejemplo, uno que estaba en un pueblo que esclavizaste: al cambiar de
        dueño, los espías de adentro quedan disfrazados del nuevo dueño). Cuando la
        lealtad de un espía a su reino baja mucho, deja de ser espía y se vuelve una unidad
        normal del reino del que está disfrazado (`Spy::think_betray` →
        `drop_spy_identity`). Con el reino Fryhtan (reputación 0) pasa cuando su lealtad
        llega a 0.
      - Otros caminos a revisar:
        1. **Traición** (`Unit::think_betray`): una unidad humana con lealtad baja se
           pasa al reino con mejor puntaje que tenga un pueblo base en su región; los
           pueblos esclavizados podrían contar como pueblo base del reino Fryhtan.
        2. **Captura de edificios** (`Firm::capture_firm` → `Unit::betray`): los
           trabajadores o soldados de un edificio capturado cambian al reino que lo
           captura.
        3. Otros cambios de reino de unidades (`Unit::change_nation`) que lleguen a un
           reino Fryhtan.
      - **Unidades independientes:** según el código no se pueden unir: la traición solo
        aplica a unidades de un reino, y los rebeldes que se quedan sin líder se unen al
        reino con la mejor reputación positiva (la del reino Fryhtan es 0).
      - Arreglo propuesto: ninguna unidad humana puede pasarse a un reino Fryhtan. Un
        espía disfrazado de un reino Fryhtan que deja de ser espía no se une: se vuelve
        independiente (o, si está dentro de un pueblo esclavizado, se queda como
        aldeano). Al elegir a quién traicionar se saltan los reinos Fryhtan, y si el reino
        Fryhtan captura un edificio humano, la gente de adentro no se une (se va o se
        vuelve independiente).
      - **Causa más probable del caso visto:** un espía de la IA en un pueblo independiente
        sale de ahí y se **disfraza de tu reino** para meterse en un pueblo esclavizado
        (`Nation::think_assign_spy_target_town`); al cambiar de disfraz sale la noticia
        "Independent unit … has joined your force." (`Unit::spy_change_nation`).
      - **Cómo quedó:**
        - Ningún espía puede disfrazarse de un reino Fryhtan que no sea el suyo
          (`Unit::spy_change_nation`); el menú de disfraz del espía no muestra los reinos
          Fryhtan (`Unit::can_spy_cloak_as`), y la IA no manda espías a pueblos
          esclavizados (`think_assign_spy_target_town`, `ai_assign_spy`).
        - Un espía que sale de un pueblo esclavizado disfrazado del reino Fryhtan sale como
          unidad independiente, sin noticia (`Spy::mobilize_town_spy`). Si deja de ser
          espía estando en el pueblo, se queda como aldeano; fuera del pueblo no deja de
          ser espía (`Spy::think_betray`).
        - `Unit::think_betray` y `Rebel::process_leader_quit` saltan los reinos Fryhtan;
          la propaganda no le da aldeanos (`ModSpy::add_villager`), y un reino Fryhtan no
          puede capturar un edificio humano por sus trabajadores
          (`Firm::can_worker_capture`).

- [x] **Bug: texto basura en la noticia cuando muere un general Fryhtan.** La noticia
      "Your general, <nombre>, has been slain." (`News::general_die`) saca el nombre de la
      tabla de nombres de la raza (`race_res[race_id]`), pero los Fryhtans no tienen raza
      (`race_id` 0), así que lee memoria que no es suya y sale basura.
      - Arreglo propuesto: con un general Fryhtan, guardar en la noticia su tipo de unidad
        y usar el mismo nombre que se ve en el juego (`Unit::unit_name`, por ejemplo
        "Rokken 12"): "Your Ordo, Rokken 12, has been slain." (o "Your general", según
        cómo se llame en el resto del juego).
      - Revisar las demás noticias que usan el nombre de la raza con unidades que
        podrían ser Fryhtans: general asesinado por un espía (`News::unit_assassinated`),
        general que te traiciona o se independiza, y el general que pide honores.
      - **Cómo quedó:** las noticias guardan, para un Fryhtan, menos su tipo de unidad en
        lugar de la raza, y `news_unit_name` (`ONEWSENG.cpp`) usa el nombre del Ordo (por
        ejemplo "Feskul") o el de la especie. La noticia dice "Your Ordo, Feskul, has been
        slain."; el asesinato dice "Your Ordo, …" o "Your All High has been
        assassinated by an enemy spy."; también traición y honores. Los nombres de Ordo
        (desde 60000) no caben en un `short`, así que se leen como `uint16_t`. Probado:
        un Ordo muerto da "Your Ordo, Feskul, has been slain."

- [x] **Las construcciones Fryhtan se regeneran solas.** Hoy un edificio solo se repara
      si tiene un constructor adentro, y los Fryhtans no tienen constructores. La idea es
      que las construcciones del reino Fryhtan (guaridas) recuperen vida solas, como si
      tuvieran un constructor de skill 20.
      - Con las reglas del constructor normal (`Firm::process_repair`), skill 20 es
        **1 punto de vida cada 5 días**.
      - Igual que con un constructor: no se repara mientras el edificio está bajo ataque
        (hasta un día después del último golpe), ni si el reino tiene el oro en negativo.
      - Solo para las construcciones de reinos Fryhtan; las guaridas salvajes no cambian.
      - **Cómo quedó:** en `Firm::next_day`, con `FRYHTAN_REPAIR_DAYS` 5, sin constructor
        y fuera de construcción. Probado: una guarida a 180/200 subió 1 punto cada 5 días.
- [x] **Botín por nivel de combate.** Antes matar una unidad humana daba $2 (soldado,
      aldeano o defensor), $10 (general) o $100 (rey), sin importar su combate.
      - **Soldado:** $2 con combate 10 hasta **$45** con combate 100, en línea recta
        (2 + (combate − 10) × 43 / 90, redondeado).
      - **Defensor de pueblo:** la mitad de un soldado de su combate.
      - **Aldeano:** fijo en $2 (también los que mueren al atacar un pueblo).
      - **General:** ×3 ($135 con combate 100).
      - **Rey:** ×22 (**$990** con combate 100, $44 con combate 10). El primer rey que
        matas de cada reino da el 100%; los siguientes de ese reino, el 50%.
      - **Comida:** fija como antes (5 / 20 / 100).
      - **Cómo quedó:** `ModFryhtan::reward_kill` y `ModFryhtan::kill_gold`, con las
        constantes `KILL_GOLD_*` en `ModFryhtan.h`. Los reyes matados de cada reino se
        guardan en `king_kill_array` al final de los datos Fryhtan de la partida (las
        partidas anteriores cargan con la cuenta en 0); se reinicia cuando aparece un reino
        nuevo (`NationBase::init`). Probado: los reyes de combate 100 de los dos reinos de
        la IA dieron $990 la primera vez y $495 la segunda.
- [ ] **Comida de los Fryhtans a la mitad.** Lo que come cada Fryhtan del reino al año
      baja a la mitad (hoy `invoked_fryhtan_food_year` en `OF_BASE.cpp`, igual para
      soldado, Ordo y All High, afuera o dentro de la guarida):

      | Especie | Hoy | Nuevo |
      |---|---|---|
      | Rokken | 50 | 25 |
      | Sauroid | 45 | 23 |
      | Wyrm, Haubudam | 40 | 20 |
      | Holgh | 35 | 18 |
      | Rattus, Karrotten, Pfith, Droog, Ick | 30 | 15 |
      | Deezboanz, Broosken | 25 | 13 |
      | Doink | 20 | 10 |

      - Ejemplo: una guarida de Rokken llena (1 Ordo y 8 soldados) pasa de 450 a 225 al
        año.
      - 💡 Solo para los reinos Fryhtan; los Fryhtans invocados desde la Sede de Poder de
        un reino humano (gameplay) siguen comiendo lo de hoy.
      - La comida de entrenar (20 el soldado, 40 el Ordo) no cambia.
      - La tabla redondea los impares; en el código se usa el valor exacto (por ejemplo
        22.5), porque se come un poco cada día.
- [x] **Ordo dentro de un pueblo esclavizado.** Un Ordo se puede mandar a un pueblo
      esclavizado del reino (clic derecho sobre el pueblo con el Ordo seleccionado) y
      entra como un espía: deja de verse en el mapa. Hay 1 Ordo por pueblo y no corre
      riesgo. Hace una acción a la vez:
      - **Subjugate:** la lealtad del pueblo no baja del **liderazgo del Ordo**, con tope
        de **50** (`SUBJUGATE_MAX_LOYALTY`, para subirlo con una mejora más adelante).
        Sirve sobre todo cuando no hay general en una guarida enlazada o en los 3 meses de
        gracia.
      - **Kidnap:** llega un aldeano nuevo al pueblo, en promedio 1 cada **4 meses** con
        liderazgo 30 y cada **2 meses** con 100 (en línea recta). Cada uno llega en un día
        al azar entre la mitad y una vez y media del promedio (con liderazgo 30, entre 60
        y 180 días, nunca más de 6 meses) y el contador vuelve a empezar. La raza sale al
        azar según la gente del pueblo. Hasta 60 personas. Noticia: "Your Ordo Feskul has
        kidnapped a Greek villager into your village of …".
      - **Taskmaster:** cada aldeano da **+$0.5 al mes con liderazgo 30 hasta +$2 con
        100**, aparte del tributo (y aunque no haya general en una guarida enlazada), y
        **+5 a +20 de comida al año**.
      - **Cómo quedó:**
        - En el panel del pueblo, el **botón de espionaje** (el ojo) abre la vista del
          Ordo; su ayuda dice "Ordo" y "<nombre>: <acción>". La vista (`TOWN_MENU_ORDO`,
          `Town::disp_ordo_menu`), según el diseño del usuario: ícono, nombre,
          "<especie> Ordo" y liderazgo; la fila **Role | <acción>** con el botón **>**
          que pasa a la siguiente (Subjugate → Kidnap → Taskmaster); lo que hace la
          acción con su liderazgo; la tabla Loyalty (N/A), Leadership, Combat y Hit
          Points; y **Leave village** y **Back**.
        - El Ordo se guarda en `ModFryhtan::town_ordo_array` (por pueblo) y la unidad se
          borra del mapa; al salir se crea otra vez con su nombre, liderazgo, combate y
          vida (`ordo_enter_town`, `ordo_leave_town`). Adentro sigue comiendo y costando
          como afuera, y cuenta en la población Fryhtan.
        - Si el pueblo cambia de dueño o desaparece, el Ordo sale a su lado (se pierde si
          no hay lugar).
        - Se guarda al final de los datos Fryhtan de la partida; las partidas anteriores
          cargan sin Ordos en pueblos.
        - Probado: entrar con clic derecho, Kidnap (llegó un aldeano zulú con su noticia),
          Taskmaster (+$42 y la comida extra con 21 aldeanos y liderazgo 100), salir del
          pueblo, y guardar y cargar con el Ordo adentro.
- [x] **Cursor de entrar al mandar un Ordo a un pueblo.** Con un Ordo seleccionado, al
      pasar el mouse sobre un pueblo esclavizado al que puede entrar, debe verse la
      flecha de entrar (`CURSOR_ASSIGN`), como cuando un aldeano va a un pueblo propio.
      Hoy se ve el cursor normal aunque el clic derecho sí lo manda.
      - Dónde: `Power::choose_cursor` (`OPOWER.cpp`), en el caso
        `SCREEN_OBJECT_FRIEND_TOWN` con una unidad propia seleccionada: hoy solo da
        `CURSOR_ASSIGN` a aldeanos humanos sin oficio. Agregar
        `mod_fryhtan.can_ordo_enter_town(unidad, pueblo)`.
      - Solo con un Ordo seleccionado (igual que el clic derecho, que solo lo manda si
        está solo).
      - **Cómo quedó:** `Power::choose_cursor` da `CURSOR_ASSIGN` si
        `mod_fryhtan.can_ordo_enter_town()`. Probado.
- [x] **Rol del Ordo junto al nombre del pueblo.** En la vista general del mapa, el
      nombre de un pueblo esclavizado con un Ordo adentro muestra su rol entre
      paréntesis, por ejemplo **Rouen (Subjugate)**, **Rouen (Kidnap)** o **Rouen
      (Taskmaster)**.
      - Solo en los pueblos del jugador (los demás no ven el rol).
      - También en el reporte de pueblos (Villages).
      - **Cómo quedó:** `ModFryhtan::town_role_name`, usado en el nombre del mapa
        (`OWORLD_Z.cpp`) y en el reporte (`OR_TOWN.cpp`). Probado: "Persepolis
        (Subjugate)" en los dos.
- [x] **Entrenar un Ordo tarda lo mismo que un soldado:** 15 días (especie grande) y
      10 (chica), antes 30 y 20 (`LAIR_ORDO_DAYS`, `LAIR_SMALL_ORDO_DAYS`). El oro y la
      comida no cambian.
- [x] **Ajustes al Ordo, Kidnap, resistencia y Promote / Demote.**
      - **Kidnap** al doble: **2 aldeanos** por vez, cada 240 días en promedio con
        liderazgo 30 y 120 con 100 (entre la mitad y 1.5 veces: con liderazgo 30, de 120 a
        360 días, máximo 12 meses). La noticia ya no dice la raza: "Your Ordo … has
        kidnapped 2 villagers into your village of …" (o "a villager" si solo cabe uno).
      - **Ordo dentro del pueblo:** cada 15 días gana liderazgo como un general con un
        soldado por cada 5 aldeanos (hasta 8), ×1.5, y combate como un general; recupera
        1 punto de vida (`ModFryhtan::train_town_ordo`). Las fracciones y el potencial se
        guardan (la partida marca el formato nuevo con un -1 antes de la cuenta de Ordos;
        las partidas del formato anterior siguen cargando).
      - **Mínimo de resistencia según el liderazgo** del general de la guarida: 60 con
        liderazgo 30 o menos hasta 45 con 100; cuenta la guarida con el mínimo más bajo
        (`ModFryhtan::enslave_min_resistance`, `Town::update_target_resistance`).
      - **Promote / Demote** de vuelta para los Fryhtans del reino, en el panel de la
        unidad (`ModFryhtan::promote_ordo`, `demote_ordo`): al ascender recibe nombre de
        Ordo; al bajar, un número de serie. El All High no se puede bajar.
      - **Soldados con liderazgo 20** (`LAIR_TRAIN_LEADERSHIP`), visible en su panel y en
        la guarida; lo aprenden en la guarida solo con potencial, como los humanos.
      - **El liderazgo del Ordo sube ×1.5** (`ORDO_LEADERSHIP_PERCENT`) en la guarida y
        en el pueblo.
      - **Train Ordo oculto** (`LAIR_SHOW_TRAIN_ORDO` 0); el código se queda.
      - Probado: soldado de la guarida con Loyalty N/A, Leadership 20; Demote y Promote de
        un Ordo; el Ordo en un pueblo de 20 subió 0.30 de liderazgo y 0.20 de combate cada
        15 días; Kidnap esperando 342 días con liderazgo 30. No se probó en partida el
        mínimo de resistencia ni la noticia nueva de Kidnap.
- [x] **Precios por especie, Pfith chica y Train Ordo de vuelta.**
      - **Ick, Broosken y Doink cuestan $100** (soldado y Ordo); antes $150.
      - **Pfith pasa a especie chica** (hoy es grande por su nivel 6): $150, población 2,
        10 días de entrenamiento (soldado y Ordo). Va como excepción en
        `ModFryhtan::is_large_monster` (y en la tabla de New Game,
        `menu_species_level_array` / `menu_species_size_text`); sus datos de combate,
        vida y ataque no cambian.
      - Así quedan:
        - Chicas a **$100**: Ick, Broosken, Doink.
        - Chicas a **$150**: Deezboanz, Rattus, Pfith.
        - Grandes a **$200**: Haubudam, Rokken, Wyrm, Droog, Sauroid, Karrotten, Holgh.
      - **New Game sin precio ni tamaño:** el texto queda solo "Fryhtan kingdom: New Rokken
        Horde" (hoy "… (Large: $200 per unit)"), así que ya no hace falta cambiar la tabla
        de New Game por Pfith.
      - **Cómo quedó:** `ModFryhtan::train_cost` (`LAIR_CHEAP_TRAIN_COST` 100) y la
        excepción de Pfith en `is_large_monster` (`FRYHTAN_PFITH` y los demás ids. en
        `ModFryhtan.h`); `LAIR_SHOW_TRAIN_ORDO` 1. Probado: los precios, tamaños y días de
        las 13 especies, el texto de New Game y el botón Train Ordo en la guarida.
      - **Train Ordo vuelve a verse** en la guarida (`LAIR_SHOW_TRAIN_ORDO` 1), junto a
        Promote / Demote.
- [x] **Sobrepoblación, Cull Population y muertes por Taskmaster.**
      - **Sobrepoblación:** un pueblo esclavizado con **30 o más** habitantes pierde
        lealtad **poco a poco: −1 cada 5 días** (desde 50, unos 3.5 meses hasta 29, y de
        ahí se puede rebelar con la rebelión normal, `REBEL_LOYALTY`). Mientras tanto la
        lealtad no sube de vuelta. Si baja de 30 habitantes, la baja se detiene y la
        lealtad se recupera hacia su objetivo.
        - ❓ Pendiente: Subjugate contra la sobrepoblación (hoy gana la sobrepoblación).
      - **Botón Cull Population** (icono de espada, el de Demote) en el panel del pueblo
        esclavizado, al lado del ojo del Ordo: mata a **3 o 4** aldeanos al azar (de las
        razas del pueblo, según su gente) y da **$2 y 5 de comida** por cada uno. No se
        puede usar con menos de 5 habitantes (el botón se oscurece y su ayuda lo dice).
        Por ahora sin tiempo de espera y sin efecto en la lealtad de los demás.
      - **Taskmaster mata aldeanos:** 1 cada 60 a 120 días (90 en promedio), sin noticia
        y sin depender del liderazgo; nunca el último.
      - **Cómo quedó:** `ModFryhtan::next_day_town_overpopulation` (y el objetivo en
        `Town::update_target_loyalty`), `cull_town`, y `TownOrdo::next_death_date` (la
        partida marca este formato con -2; las de -1 y las primeras siguen cargando).
        Probado: con 32 habitantes la lealtad bajó de 50 a 45 en 25 días; Cull Population
        mató a 4 y dio $8 y 20 de comida; el botón con su ayuda. No se probó en partida la
        muerte por Taskmaster.
- [ ] **Cull Population solo con un Ordo en el pueblo.** El botón de Cull Population
      solo aparece (o solo se puede usar) cuando hay un Ordo dentro del pueblo
      esclavizado. Sin Ordo, no hay Cull Population.
      - ❓ ¿El botón desaparece o se ve oscurecido con la ayuda "Needs an Ordo in the
        village"?
      - Dónde: `ModFryhtan::can_cull_town` (agregar `town_ordo(town_recno)`) y el botón
        en `Town::disp_main_menu`.

## 8. Economía oscura (en diseño)

El diseño en borrador está en `DISENO-fryhtan-oscuro.md`: el Enforcer de los pueblos
esclavizados, el mercado negro, la torre oscura de ciencia, la fábrica de guerra oscura,
los soldados oscuros y las tecnologías de la guarida. Cuando se apruebe, sus fases se
pasan aquí.
