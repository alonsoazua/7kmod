# Guía de New Adversaries

**Español** | [English](GUIDE.en.md)

New Adversaries es un mod de **Seven Kingdoms: Ancient Adversaries 2.15.7**. Esta guía
explica, por tema, qué tiene de distinto respecto al juego original. Para el historial
detallado de cada versión están los changelogs.

El instalador trae dos versiones del juego:

| | **New Adversaries Classic** | **New Adversaries** |
|---|---|---|
| Ejecutable | `7k-newAdversaries-Classic.exe` | `7k-newAdversaries.exe` |
| Qué cambia | Interfaz, comodidad, editor de escenarios y pocos ajustes | Todo lo de Classic, más cambios de reglas y los reinos Fryhtan |
| Partidas guardadas | `%APPDATA%\7kfans.com\7k-newAdversaries-Classic\` | `%APPDATA%\7kfans.com\7k-newAdversaries\` |

En esta guía, lo marcado con **[NA]** solo está en New Adversaries. Lo que no lleva
marca está en las dos.

## Contenido

1. [Interfaz y comodidad](#1-interfaz-y-comodidad)
2. [Controles nuevos](#2-controles-nuevos)
3. [Fuertes, reclutamiento y producción](#3-fuertes-reclutamiento-y-producción)
4. [Cambios de reglas [NA]](#4-cambios-de-reglas-na)
5. [Espionaje [NA]](#5-espionaje-na)
6. [Greater Beings y Fryhtans invocados [NA]](#6-greater-beings-y-fryhtans-invocados-na)
7. [Jugar como reino Fryhtan [NA]](#7-jugar-como-reino-fryhtan-na)
8. [Editor de escenarios](#8-editor-de-escenarios)
9. [Más documentación](#9-más-documentación)

---

## 1. Interfaz y comodidad

### Avisos en pantalla
- **"Game Paused"** arriba del mapa mientras el juego está en pausa.
- **"Game Speed: N"** durante 10 segundos al cambiar la velocidad, quitar la pausa,
  empezar o cargar una partida.
- **Barra de entrenamiento:** una barra verde chica sobre tus pueblos, fábricas de
  guerra, astilleros (y guaridas, en los reinos Fryhtan) muestra el avance de lo que se
  está entrenando o construyendo.

### Selección de unidades
- **El recuadro prefiere a los militares:** si dentro hay soldados, armas o barcos de
  guerra, solo se seleccionan esos y los civiles se ignoran. Con `Alt` se seleccionan
  todas.
- **Número de seleccionadas** a la derecha de la barra del panel.
- **Lista de unidades:** con varias seleccionadas, el botón **">"** muestra la lista con
  su retrato, vida y nivel. Abajo, un panel con la unidad en foco: su título, su nombre,
  su combate y su habilidad.
  - **Clic:** pone la unidad en foco, sin perder la selección.
  - **Doble clic:** te quedas solo con esa unidad.
  - **`Ctrl` + clic:** centra la vista en ella.
  - **Clic derecho:** la quita de la selección.
- **Unidades de otros reinos:** un recuadro sin unidades tuyas ya selecciona algo: la
  más importante de las que hay dentro, y su lista muestra a todas, agrupadas por reino.
  No se revela su combate.

### Grupos
- `Ctrl+1`–`9` guarda la selección como grupo; `Alt+1`–`9` lo selecciona, y dos veces
  seguidas además centra la vista en él.
- **Botones de grupo:** en la orilla derecha del mapa, encima de los botones de
  noticias, aparece un botón por cada grupo que tiene unidades. Clic lo selecciona,
  doble clic además centra la vista.
- El panel de una unidad muestra su número de grupo junto al color del reino, y la lista
  dice "Group N" cuando la selección es un grupo completo.

### Paneles con más información
- **Poder de ataque y armadura** en el panel de las unidades: "Melee pwr", "Ranged pwr"
  y "Armor". Si un general cercano les da bono, se ve con una flecha y el porcentaje.
- **Los Fryhtans** muestran su combate, poder y armadura (antes no mostraban nada).
- **Desglose de la lealtad:** al dejar el ratón sobre la lealtad de un general o
  soldado tuyo, aparece de qué se compone.
- **Retrato del rey** de cada reino en el reporte de reinos (F1).
- **Retratos de los Fryhtans** en las listas de unidades y de soldados del fuerte.

### Generales y honores
- Un general se enoja si su **contribución** en batalla es mucho mayor que lo que le has
  pagado en recompensas. Ahora se ve:
  - **Reporte militar (F5), pestaña Contributions:** tus generales ordenados por riesgo,
    con su estado ("Content", "Unrewarded", "At risk!") y cuánto cuesta honrarlos.
    **Pay selected** y **Pay all** pagan los honores.
  - **Petición de honores:** cuando un general llega al límite, sale una noticia y puedes
    aceptar o rechazar su petición.

---

## 2. Controles nuevos

| Acción | Qué hace |
|---|---|
| `Ctrl+1`–`9` | Guarda la selección como grupo |
| `Alt+1`–`9` | Selecciona el grupo; dos veces seguidas centra la vista |
| `Alt` + recuadro | Selecciona todas las unidades, también los civiles |
| Botones de grupo (orilla derecha del mapa) | Clic selecciona; doble clic centra la vista |
| Lista de unidades: clic / doble clic / `Ctrl`+clic / clic derecho | Foco / solo esa unidad / centrar / quitar |
| `Shift` + clic en Train (fuerte) | Pone 8 soldados en la cola |

La lista completa de teclas del juego y del mod, y cómo cambiarlas, está en
`KEYBINDINGS.md`.

---

## 3. Fuertes, reclutamiento y producción

- **Recruit en el fuerte:** recluta una unidad del pueblo enlazado y la mete directo al
  fuerte. **Train** pone en cola un soldado que entra solo al fuerte al terminar.
- **Recruit en minas, fábricas, torres de ciencia y fábricas de guerra:** igual que en el
  fuerte, pero con la profesión del edificio.
- **Reclutamiento automático** de esos edificios: un botón lo activa o desactiva. En
  Classic está activado por defecto, como en el original; **[NA]** en New Adversaries
  está desactivado.
- **Unidades fuera del fuerte:** el panel del fuerte sigue mostrando, oscurecidos, al
  general y los soldados que salieron. Clic en uno lleva la cámara a él.
- **Withdraw:** botón en la página 1 del fuerte que regresa a todos los que salieron.
  Alert / At ease pasa a la página 2.
- **Pueblos independientes iniciales** de 15 a 29 habitantes, y menos resistencia en los
  pueblos de varias razas.

---

## 4. Cambios de reglas [NA]

### Pueblos
- **Train Captain:** entrena un general de la raza del rey (30 de liderazgo) por $150.
  Solo en pueblos donde la raza del rey es mayoría.
- Los pueblos independientes tienen como máximo **2 razas**.
- Un pueblo de otro reino que se **rinde** empieza con más lealtad y no se puede rebelar
  durante 90 días.

### Puerto: comercio de materias primas
- El puerto puede producir arcilla, cobre o hierro como una mina sin trabajadores.
  **Hire Shipment** ($500) le da 500 de suministro.

### Tecnologías nuevas (Torre de Ciencia → Technologies)

| Tecnología | Efecto |
|---|---|
| Elite Warriors | Los soldados entrenados empiezan con 30 de combate (antes 20) |
| Elite Workers | Los trabajadores entrenados empiezan con 30 de habilidad |
| Building Repair | Todos tus edificios recuperan 1 de vida cada 5 días |
| Fryhtan Slayers | Tus soldados hacen 30% más daño a los Fryhtans |

### Combate y razas
- **Armadura por raza:** algunas razas suben de armadura al ganar combate.
- **Bono del general:** un general con al menos 4 de sus soldados cerca les da su
  liderazgo como bono de daño y +1 de armadura.
- **Ajustes por raza:** chinos, egipcios, japoneses y mughules aprenden sus ataques
  especiales a otro nivel de combate; los zulúes son 25% más rápidos.
- **Los disparos ya no se bloquean** por edificios, murallas ni montañas.

Los números exactos están en `COMBAT_ANALYSIS.md`.

---

## 5. Espionaje [NA]

- **Propaganda** en pueblos independientes: mucho más fuerte que "Sow Dissent", sobre
  todo con un espía de la raza del pueblo y un fuerte tuyo enlazado. A veces un aldeano
  se une a tu reino.
- **Counter-Spy** en pueblos independientes: tu contraespía puede atrapar a los espías de
  otros reinos.
- **Gather Intel:** tus espías recolectan información y cada pocos meses recibes un
  **reporte de inteligencia** del reino espiado (pestaña "Intelligence" del reporte de
  espías). Reemplaza a "Steal secrets".
- **Accidentes:** en los edificios con trabajadores pueden ocurrir accidentes, y los
  espías pueden provocarlos.
- **Soborno:** cada opción muestra su probabilidad de éxito.

Todo el detalle está en `SPYING.md`.

---

## 6. Greater Beings y Fryhtans invocados [NA]

### Greater Beings
- **Dragon (normando):** su poder es una bola de fuego mucho más fuerte, con más área.
- **Djini (mughul):** además de bajar la lealtad, convierte a un soldado enemigo.
- **Phoenix (griego):** expone y ejecuta a un espía enemigo en uno de tus pueblos.
- **Lord of Healing (persa):** cura más dentro de los fuertes, y también a tus Fryhtans.
- El Jing Nung chino gasta menos por ataque y el Phoenix menos por día.

### Invocar Fryhtans
- La **Sede de Poder** tiene un botón para invocar un Fryhtan aliado de tu raza (por
  ejemplo, Rokken para los persas), pagando puntos de oración.
- Son permanentes, comen, regeneran vida, pueden entrar a tus fuertes como soldados y
  llevan un ícono con el color de tu reino.
- La IA también los invoca y los usa en sus ataques.

Detalles en `GREATER_BEINGS.md`.

---

## 7. Jugar como reino Fryhtan [NA]

Solo en partidas de un jugador. En **New Game**, la pestaña **Fryhtans** permite elegir
una de las 13 especies. El reino se llama "<nombre> <especie> Horde" y su rey es el
**All High**.

### Cómo empieza
- El All High dentro de una **guarida** con 4 soldados de su especie, y un **pueblo
  esclavizado** de 20 habitantes enlazado.
- Life Points iniciales según la opción **Treasure**: 150, 300, 500 o 700.
- Todos los reinos empiezan en la masa de tierra más grande, porque los Fryhtans no
  cruzan el mar.

### Diferencias con un reino humano
- Sin edificios humanos, sin reputación y sin lealtad en sus Fryhtans.
- Sin comercio, amistad ni alianzas con los humanos; sí guerra, cese al fuego, tributo y
  ayuda.
- Los **Fryhtans salvajes no te atacan** (salvo que los ataques).
- **Sucesión:** si muere el All High, el Ordo con más combate toma su lugar.
- Pierdes solo cuando te quedas sin guaridas y sin Fryhtans.

### Life Points (LP)
La moneda de los Fryhtans, junto al oro y la comida, en la barra de arriba.

- **Se ganan** matando (más con enemigos de más combate, generales y reyes), con el
  tributo de los pueblos esclavizados (1 al mes por cada 10 aldeanos, y 1 más con 15 o
  más) y con los aldeanos que mueren en tus pueblos.
- **Se gastan** al entrenar Fryhtans, ascender Ordos, construir guaridas e investigar
  tecnologías.

Además, matar humanos da **botín** de oro y comida según su nivel de combate (mucho más
por generales y reyes).

### Guaridas
- Funcionan como fuertes: un líder (Ordo o All High) y soldados. Se construyen con
  **Build → Lair** ($500 y 30 LP).
- **Lugares según la especie:** las de mucha vida caben menos. Rokken 4; Wyrm, Droog,
  Haubudam y Sauroid 5; Holgh, Karrotten y Pfith 6; las demás 8.
- **Train** entrena un soldado de la especie de la guarida y **Train Ordo** un general.
  Si la guarida está llena, el nuevo se queda afuera.
- **Promote / Demote** asciende a un soldado a **Ordo** (con nombre propio) o lo regresa.
- **Capturar guaridas salvajes:** una guarida salvaje cerca de una tuya muestra una
  oferta; con **Pay Tribute** pasa a tu reino con un Ordo y sus soldados, y entrena su
  propia especie.

| | Oro | LP (soldado) | LP (Ordo) |
|---|---|---|---|
| Ick, Doink, Broosken | $100 | 7 | 12 |
| Deezboanz, Rattus, Pfith | $100 | 8 | 13 |
| Karrotten, Holgh, Droog | $150 | 9 | 14 |
| Las demás | $150 | 10 | 15 |
| Ascender a Ordo | — | 5 | — |

### Pueblos esclavizados
- **Cómo se esclaviza:** una guarida con líder enlazada a un pueblo independiente le
  baja la resistencia; para terminar de bajarla hay que pelear. A un pueblo de otro
  reino se le baja la lealtad peleando.
- **Raid:** en un pueblo independiente enlazado a tu guarida, el botón Raid lo asalta
  para bajarle la resistencia y llevarte botín.
- Pagan **tributo** en oro y LP cada mes, producen comida y crecen, pero no reclutan ni
  pagan impuestos.
- Necesitan una guarida enlazada **con líder adentro**; si no, no pagan y su lealtad baja.
- **Sobrepoblación:** con 30 habitantes o más (32 con Dominion), el pueblo pierde lealtad
  y termina rebelándose. El panel del pueblo tiene dos páginas: la **1** con el tributo
  y la **2** que explica la sobrepoblación.

### El Ordo dentro del pueblo
Un Ordo puede entrar a un pueblo esclavizado y darle una orden:

| Orden | Qué hace |
|---|---|
| **Subjugate** | Sube la lealtad; si el pueblo llega a la sobrepoblación, mata aldeanos para evitar rebeliones |
| **Kidnap** | Cada cierto tiempo llegan 2 aldeanos nuevos al pueblo |
| **Taskmaster** | El pueblo produce más comida, pero el trabajo forzado mata un aldeano de vez en cuando |

Con un Ordo adentro también se puede usar **Cull Population** (botón de espada): mata 3
o 4 aldeanos a cambio de un poco de oro, comida y LP. Sin Ordo, el pueblo dice "Send an
Ordo to enable actions."

### Tecnologías de guarida
Se investigan con el botón **Research** (página 2 de la guarida). El costo se paga una
vez al desbloquearla; puedes cambiar de tecnología y cada una guarda su avance. Tardan 6
meses.

| Tecnología | Costo | Requiere | Efecto |
|---|---|---|---|
| **Alchemist Lair** | $500 + 20 LP | — | Puedes construir guaridas sobre yacimientos; con un líder adentro, convierten 50 de material en 5 LP cada 15 días |
| **Brood Mastery** | $1,000 + 30 LP | — | Entrenar y ascender Fryhtans cuesta 20% menos oro y LP |
| **Empowerment** | $1,500 + 50 LP | Brood Mastery | Los Fryhtans entrenados salen con +10 de combate |
| **Dominion** | $1,000 + 30 LP | — | +10 de lealtad en los pueblos esclavizados; la sobrepoblación empieza en 32 |
| **Sacrificial Rites** | $1,000 + 30 LP | Dominion | Un pueblo con un Ordo que llega a la sobrepoblación sacrifica 5 aldeanos a cambio de LP extra |

---

## 8. Editor de escenarios

Se abre en **Single Player → Scenario → New** (o **Edit**, para modificar un escenario
existente). Permite armar un mapa desde cero o desde uno generado:

- Colocar y borrar unidades, reyes, espías, pueblos, edificios, minas y naturaleza.
- Editar lo seleccionado: raza, combate, habilidad, población, lealtad, nombres.
- Pintar terreno: pasto, tierra, agua, colinas.
- Agregar y quitar reinos y cambiar su color.
- Elegir las metas, probar el escenario (**F12**) y guardarlo.

---

## 9. Más documentación

En la carpeta `Docs` del juego (como `.txt`; los `.en.txt` son en inglés) y en el
repositorio:

| Archivo | Tema |
|---|---|
| `KEYBINDINGS.md` | Todas las teclas y cómo cambiarlas |
| `ADVANCED_CONFIG.md` | Opciones avanzadas de `config.txt` |
| `COMBAT_ANALYSIS.md` | Cómo funciona el combate, con tablas por raza y Fryhtan (solo en el repositorio, en español) |
| `SIEGE_WEAPONS.md` | Armas de asedio |
| `GREATER_BEINGS.md` | Greater Beings y sus poderes |
| `SPYING.md` | Espionaje |
| `CHANGELOG*.md` | Historial detallado de cada versión (carpeta `Changelog` del juego) |
