# 7kmod-gameplay: mejoras al espionaje

Checklist para mejorar el espionaje de forma sustancial. Cómo funciona hoy: ver
`SPYING.md`. Los pendientes generales de gameplay están en `TODO.md`.

---

## Checklist

Cosas anotadas para agregar en un parche. `[x]` = implementado, falta probarlo en partida.

- [x] **Propaganda en pueblos independientes.** En un pueblo independiente, el modo
  "Sow Dissent" se llama **"Propaganda"** (en el juego, en inglés). La idea es que
  los espías **ablanden la resistencia**: una forma de hacerse de un pueblo más
  rápido y sin gastar en donaciones, pero sin poder conquistarlo solos.
  - Hoy: baja la resistencia hacia tu reino de la gente de la raza del espía en
    habilidad ÷ 5 ÷ población de esa raza, cada 30 días (0.1 a 2 puntos al mes).
  - **Efecto directo:** cada 30 días, habilidad ÷ 10 por espía, sin dividir entre la
    población (2 puntos con habilidad 20, 10 con habilidad 100). **Máximo 10 puntos
    al mes por pueblo** entre todos los espías, para que llenar un pueblo de espías
    no lo haga instantáneo.
  - **La raza importa mucho:** un espía tiene el efecto completo sobre la gente de
    su misma raza y solo **una cuarta parte** sobre las otras razas del pueblo (hoy
    no tiene ningún efecto sobre ellas). Un espía chino en un pueblo maya rinde
    poco: lo pensado es contratar un espía maya en una Inn y mandarlo al pueblo
    maya. Aplica a todos los efectos de la propaganda (directo, influencia del
    fuerte y aldeanos que se unen).
  - **Sin un fuerte tuyo enlazado con general**, la propaganda no baja la
    resistencia de **30**: solo ablanda. Un fuerte vacío (sin general) o con el
    enlace desactivado no cuenta, para que no baste con construir un fuerte vacío y
    mandar espías. Es la misma regla que el juego ya usa para la influencia del
    fuerte y para las donaciones. Sin fuerte, el último empujón tiene que
    venir de atacar el pueblo (las donaciones ya requieren un fuerte enlazado).
  - **Con un fuerte enlazado que tenga general**, la propaganda puede bajarla hasta
    0, y además
    **sube la influencia del fuerte**: habilidad ÷ 5 por espía de la raza del
    general del fuerte (÷ 20 si es de otra raza), **máximo +20**. La
    meta del fuerte (100 − influencia) baja, así que un general de liderazgo medio
    ya no se queda atorado.
  - Caso de uso: un general de liderazgo 60 a mitad de partida (misma raza que el
    rey, reputación 60) tiene influencia 83, así que el fuerte deja la resistencia
    trabada en 17 y sin donaciones nunca se une. Con liderazgo 80 llega a 100 de
    influencia y se une solo. Simulación con resistencia inicial 80–100 (meses de
    juego hasta que el pueblo se une):

    | Caso | General 60 | General 80 |
    |---|---|---|
    | Solo el fuerte (hoy) | nunca | 10–11 |
    | + 1 espía de habilidad 30 | 10–11 | 7–8 |
    | + 1 espía de habilidad 50 | 7–8 | 6–7 |
    | + 2 espías de habilidad 50 | 4–5 | 4–5 |

    Comparado con las donaciones: cada una cuesta $30 por habitante ($900 en un
    pueblo de 30) y quita 10 puntos, cada vez menos. Un espía cuesta $100 al año:
    la propaganda es la opción lenta y barata, las donaciones la rápida y cara.
  - Ojo: el fuerte solo baja la resistencia de la raza de su general (las otras
    razas del pueblo no tienen meta). La propaganda es la que baja las demás.
  - **La IA:** ya usa este modo en pueblos independientes (`Spy::think_town_spy`), así
    que aprovecha la propaganda sin cambios. Ajuste: hoy decide sacar al espía según
    la lealtad del pueblo, pero un pueblo independiente tiene resistencia, no
    lealtad (descuido del original). Cambiarlo para que el espía salga cuando la
    resistencia de su raza llegue a 30 y la IA no tenga un fuerte enlazado con
    general.

- [x] **Aldeanos que se unen por la propaganda.** Cada 30 días, un pueblo
  independiente con espías tuyos en Propaganda tiene una probabilidad de que **un**
  aldeano lo deje y aparezca a su lado como unidad tuya. **Máximo 1 por pueblo al
  mes** (por reino).
  - La idea es **conseguir aldeanos de razas que no tienes** (con un espía de esa
    raza, contratado en una Inn), así que no depende de la resistencia.
  - Probabilidad al mes, por cada raza del pueblo = suma de la habilidad de tus
    espías en Propaganda de esa raza (los de otras razas cuentan una cuarta parte)
    ÷ 10 × población de esa raza ÷ 30 %. Se suman las razas, **máximo 30%** en
    total. Más espías y más habilidad lo hacen más probable, y un pueblo grande más
    que uno chico.
  - El aldeano es de una de las razas del pueblo, al azar según la probabilidad de
    cada una (casi siempre de la raza de tus espías). El pueblo nunca se queda sin
    habitantes (los espías no cuentan).
  - Calidad: combate de los defensores del pueblo ±10 (10 a 70 según el pueblo).
    Habilidad de trabajo o liderazgo (al azar entre construcción, minería,
    fabricación, investigación o liderazgo): 10 + al azar de 0 a la suma de la
    habilidad de los espías ÷ 2, **máximo 70**. Lealtad inicial: 60.
  - Estimaciones con espías de la raza del pueblo (con espías de otra raza, una
    cuarta parte), probabilidad al mes (entre paréntesis, aldeanos al año en
    promedio):

    | Espías | Calidad hasta | Pueblo 10 | Pueblo 30 | Pueblo 60 |
    |---|---|---|---|---|
    | 1 de habilidad 50 | 35 | 2% (0.2) | 5% (0.6) | 10% (1.2) |
    | 2 de habilidad 50 | 60 | 3% (0.4) | 10% (1.2) | 20% (2.4) |
    | 3 de habilidad 50 | 70 | 5% (0.6) | 15% (1.8) | 30% (3.6) |
    | 2 de habilidad 100 | 70 | 7% (0.8) | 20% (2.4) | 30% (3.6) |

  - Por confirmar: los números.

- [x] **Accidentes en los edificios con trabajadores.** En cualquier edificio con
  trabajadores (minas, fábricas, War Factory, Tower of Science, etc.; no los
  fuertes), de cualquier reino, puede ocurrir un accidente en el que mueren **1 o
  2 trabajadores**.
  - Probabilidad normal: 1 entre 730 al día por edificio (1 cada 2 años en
    promedio).
  - **Espías enemigos en modo Sabotaje** en el edificio la aumentan y se acumulan:
    1 espía ×2, 2 espías ×3, 3 o más ×4.
  - **Límites por reino** (el reino dueño de los edificios), en los últimos 365 días:
    - Accidentes normales: **máximo 1 al año**.
    - Accidentes causados por espías: **máximo 3 al año**.
    - Cuando ocurre un accidente en un edificio con espías, se decide si fue normal
      o causado por los espías según su parte de la probabilidad (con 1 espía, la
      mitad de las veces cada uno). Si ese tipo ya llegó a su límite, no pasa nada.
  - **Mensajes:**
    - Si el edificio es tuyo: "An accident occurred at the Mine near <pueblo>. 2
      workers died."
    - Si lo causó tu espía en un edificio enemigo: "Your spy caused an accident at
      the <reino> Mine near <pueblo>. 1 worker died."
  - Los trabajadores que mueren son de los normales, nunca un espía. Si viven en un
    pueblo, el pueblo pierde esa población.
  - Hay que guardar en la partida la fecha de los últimos accidentes de cada reino
    (cambia la versión de las partidas guardadas, como con las tecnologías del mod).
  - Por confirmar: el máximo de ×4 con 3 espías.

- [x] **Porcentaje de éxito del soborno.** En el menú de soborno, cada botón de monto
  muestra la probabilidad de éxito, por ejemplo "Offer $1,000 (35%)" (montos: $500,
  $1,000, $1,500, $2,000, $3,000 y $4,000).
  - Se calcula con la misma fórmula del juego (ver `SPYING.md`, sección 5), entre 0%
    y 100%.
  - Si el objetivo es un espía de otro reino, el soborno siempre falla, pero el
    porcentaje se muestra como si fuera una unidad normal, para no revelar que es
    espía.


- [x] **Reportes de inteligencia (reemplaza "Steal secrets").** En vez de abrir las
  pantallas de reportes del enemigo en vivo, el espía genera un **reporte resumido
  con fecha**, que se guarda y se consulta después.
  - **Obtenerlo:** un espía dentro de un pueblo o edificio enemigo pasa al modo
    **"Gather Intelligence"**. Después de **30 días** en ese modo se genera el
    reporte y el espía sigue en su lugar. Mientras recolecta lo pueden atrapar, como
    en los otros modos activos (Sabotaje, Sembrar discordia). Ya no hay exposición
    al cerrar la pantalla (así desaparece el error del original, ver `SPYING.md`,
    sección 9).
  - **Límite:** **1 reporte por reino cada 30 días**, sin importar cuántos espías
    tengas en él. Si varios espías terminan, el reporte lo hace el de más habilidad;
    los demás siguen recolectando hasta que pasen los 30 días.
  - **Contenido**, según la habilidad del espía (igual que hoy):

    | Sección | Habilidad | Qué incluye |
    |---|---|---|
    | Pueblos | 20 | número de pueblos, población total, lealtad promedio |
    | Economía | 30 | tesoro, comida, ingresos y gastos anuales |
    | Comercio | 30 | número de mercados y caravanas |
    | Reino | 40 | reputación, relaciones con otros reinos |
    | Tecnología | 40 | lista de tecnologías y su nivel |
    | Militar | 50 | soldados, generales, armas de asedio, barcos, fuertes |
    | Espías | 90 | número de espías y dónde están |

  - **Precisión:** los números salen como un rango aproximado según la habilidad, con
    un error de ±(100 − habilidad) ÷ 2 % (±40% con habilidad 20, ±25% con 50, ±5% con
    90, exacto con 100). Por ejemplo, "80–120 soldiers". El valor real queda en un
    punto al azar dentro del rango, para que no se pueda deducir tomando el centro.
    Las tecnologías y las relaciones se muestran tal cual.
  - **Dónde se consulta:** en el reporte de Espías, una pestaña nueva
    **"Intelligence"** con la lista de reportes (reino, fecha, espía) y, al hacer clic
    en uno, su detalle.
  - **Se guardan** los últimos 30 reportes en la partida (cambia la versión de las
    partidas guardadas).
  - Por confirmar: si la IA debe usarlo (hoy la IA no usa "Steal secrets").

- [x] **Balance de la propaganda (resistencia), −33%.** Efecto directo de habilidad ÷
  10 a **habilidad ÷ 15** por espía cada 30 días, y máximo al mes de 10 a **7** puntos
  por raza. Con habilidad 50: de 5 a 3.3 puntos al mes.
  - No cambian: el mínimo de 30 sin fuerte, la regla de la raza (una cuarta parte
    para otras razas) ni el bono a la influencia del fuerte (habilidad ÷ 5, máximo
    +20, confirmado que se queda así).

- [x] **Balance de los aldeanos que se unen, −33%.** Probabilidad al mes de
  habilidad ÷ 10 a **habilidad ÷ 15** × población de la raza ÷ 30 %, y máximo de 30% a
  **20%**. Con 1 espía de habilidad 50 en un pueblo de 30: de 5% a 3.3% al mes.

- [x] **Gather Intel: de 2 a 6 meses según la habilidad acumulada.** El tiempo para un
  reporte depende de la **suma de la habilidad** de tus espías en "Gather Intel" en ese
  reino (antes, 30 días fijos):
  - Suma 20 o menos: **180 días** (6 meses). Suma 200 o más: **60 días** (2 meses).
    En medio, proporcional: días = 180 − (suma − 20) × 120 ÷ 180.

    | Espías en Gather Intel | Suma | Días |
    |---|---|---|
    | 1 de habilidad 20 | 20 | 180 |
    | 1 de habilidad 50 | 50 | 160 |
    | 1 de habilidad 100 | 100 | 127 |
    | 2 de habilidad 50 | 100 | 127 |
    | 3 de habilidad 50 | 150 | 93 |
    | 2 de habilidad 100 | 200 | 60 |

  - Se calcula cada día con los espías que están recolectando en ese momento. El
    reporte lo sigue haciendo el espía de más habilidad (su habilidad decide las
    secciones y la precisión).
  - El límite de 1 reporte por reino cada 30 días ya no hace falta: como mínimo pasan
    60 días.

- [x] **Balance de los accidentes.** Probabilidad normal de 1 cada 2 años a **1 cada 5
  años** por edificio (1 entre 1,825 al día). **La Sede de Poder (Seat of Power) queda
  fuera.** No cambian los multiplicadores de los espías (×2, ×3, ×4) ni los límites por
  reino (1 normal y 3 por espías en 365 días).
  - Por reino, sin espías (con el límite de 1 cada 365 días):

    | Edificios | Accidentes en promedio | Al menos uno en el primer año |
    |---|---|---|
    | 1 | 1 cada 6 años | 18% |
    | 4 | 1 cada 2.25 años | 55% |
    | 10 | 1 cada 1.5 años | 86% |
    | 20 | 1 cada 1.25 años | 98% |

- [x] **Fondo de los reportes de inteligencia.** En la pestaña "Intelligence", el
  detalle del reporte se dibuja sobre un panel sólido. Cambiarlo a fondo transparente,
  como en las demás ventanas de reportes. La barra de títulos de las columnas (Date,
  Kingdom, Spy, Skill) y las pestañas se quedan en gris sólido.

- [ ] **Muertes en los accidentes normales.** Hoy mueren 1 o 2 trabajadores (50% cada
  uno). En los accidentes normales (no causados por espías), **el 90% de las veces
  muere 1 trabajador** y el 10%, 2. Los causados por espías se quedan en 1 o 2 al 50%
  (confirmado, para probar).

- [ ] **El sabotaje provoca muchos más accidentes, según la habilidad.** Hoy solo
  cuenta el número de espías (×2, ×3, ×4) y un espía de habilidad 100 provoca en
  promedio 1 accidente cada 5 años en el edificio. Nuevo: el multiplicador depende de la
  **suma de la habilidad** de los espías enemigos en Sabotaje en el edificio:
  **1 + suma ÷ 10, máximo ×16** (confirmado, para probar). La probabilidad normal
  sigue siendo 1 entre 1,825 al día.

  | Espías en Sabotaje | Multiplicador | Accidentes por espías al año en ese edificio |
  |---|---|---|
  | 1 de habilidad 20 | ×3 | 0.4 |
  | 1 de habilidad 50 | ×6 | 1 |
  | 1 de habilidad 100 | ×11 | 2 |
  | 3 de habilidad 50 o 2 de 100 | ×16 (máximo) | 3 |

  - Sigue el límite por reino: 3 accidentes causados por espías en 365 días.
  - Cuando ocurre un accidente en un edificio con espías, se sigue decidiendo si fue
    normal o de los espías según su parte de la probabilidad (con ×11, 10 de cada 11
    son de los espías).

- [ ] **Pestaña "Sabotage" en el reporte de Espías.** Una tercera pestaña, junto a
  "Spies" e "Intelligence", con la lista de los accidentes que han causado tus espías,
  del más reciente al más antiguo.
  - Columnas: fecha, reino (con su color), edificio, pueblo cercano, trabajadores
    muertos y el espía que lo causó (el de más habilidad en Sabotaje en el edificio).
  - Se guardan en la partida los últimos 30 accidentes de cada reino.
  - Al final de la lista, el total de accidentes y de trabajadores muertos.

---

## Ideas por confirmar

- ~~**Corregir la exposición al ver secretos**~~ (error del original, ver `SPYING.md`,
  sección 9): ya no hace falta si se hacen los reportes de inteligencia, que
  reemplazan esa pantalla.
