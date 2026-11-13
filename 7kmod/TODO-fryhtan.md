# 7kmod-fryhtan: reinos Fryhtan

Rama `7kmod-fryhtan`, basada en `7kmod-gameplay`. Permite jugar como un reino Fryhtan.
En la versión 1 **solo el jugador** puede ser un reino Fryhtan; los reinos de la IA siguen
siendo humanos. Solo partidas de un jugador (sin multijugador).

`[ ]` = pendiente, `[x]` = implementado, falta probarlo en partida.

---

## 1. Reglas confirmadas

- **Inicio:** el reino empieza solo con su rey, un **All High** de la especie elegida.
  No hay pueblo, guarida ni otras unidades al inicio.
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
  tributo y la ayuda.

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
  - Fase 4: lealtad de los pueblos esclavizados según la raza del rey, Train Captain y
    reclutamiento (se ocultan), e influencia del fuerte con bono por raza del rey (la
    guarida usa su propia regla).
  - Fase 7: la lista de partidas guardadas muestra "King <nombre>" con retrato humano;
    los mensajes de fin de partida dicen "<nombre>'s Kingdom"; la imagen de fin de
    partida y el Hall of Fame usan la raza interna; sucesión del rey.
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
- **Pendiente para otras fases:** la IA le pide tributo al reino Fryhtan y le ofrece
  comprarle comida (revisar en la fase 6).

### Fase 3: economía
- [x] Mantenimiento de las guaridas en oro; unidades solo con comida (hecho en la fase 2).
- [ ] Oro y comida al matar unidades humanas.
- [ ] Los edificios humanos que capture se destruyen.

### Fase 4: esclavizar pueblos
- [ ] Enlace guarida–pueblo independiente y baja de resistencia hasta 45.
- [ ] Captura por combate: resistencia a 0 → pueblo esclavizado.
- [ ] Pueblos de reinos humanos: lealtad a 0 peleando → pueblo esclavizado.
- [ ] Panel del pueblo esclavizado: sin Recruit, Train, Collect Tax ni Grant; muestra el
      tributo del mes.
- [ ] Tributo mensual; la población esclavizada sube el tope de población.

### Fase 5: capturar guaridas independientes
- [ ] Enlace guarida propia–guarida independiente, de cualquier especie.
- [ ] Oferta simulada (Ordo y soldados), guardada en la partida.
- [ ] Panel de la guarida salvaje con la oferta y Pay Tribute; al pagar, la guarida pasa
      a tu reino con el Ordo y sus soldados (se puede pasar del tope de población).
- [ ] Train en cada guarida con su propia especie.

### Fase 6: diplomacia y Fryhtans salvajes
- [ ] Sin comercio, amistad ni alianza con un reino Fryhtan (jugador e IA).
- [ ] Los Fryhtans salvajes no atacan al reino Fryhtan.

### Fase 7: cierre
- [ ] Reportes (F1–F8) con sentido para un reino Fryhtan.
- [ ] Condición de derrota y sucesión del rey.
- [ ] Guardar y cargar.
- [ ] Pruebas en Linux y ejecutable de Windows.
- [ ] `CHANGELOG-fryhtan.md` y `CHANGELOG-fryhtan.en.md` (creados en la fase 1; se
      actualizan en cada fase).

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
