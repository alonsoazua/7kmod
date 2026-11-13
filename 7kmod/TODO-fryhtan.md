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
      - **Cómo quedó:** las pestañas "Humans" (arriba) y "Fryhtans" (abajo) están en la
        columna de la derecha, donde estaban el botón Fryhtan y el selector. Con Fryhtans,
        la cuadrícula de las razas se cambia por 14 recuadros más angostos (7 x 2, 55
        píxeles) hechos con los de las razas: las 13 especies en orden alfabético y uno
        vacío. Los nombres usan la misma letra (la más grande en la que caben todos). Son
        recuadros con el nombre, como los de las razas; los retratos no caben en ese
        tamaño. La pestaña es la elección misma (`Config::fryhtan_monster_id`, que ya se
        guarda en 7KMOD.DAT), así que no hizo falta otro dato.

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
