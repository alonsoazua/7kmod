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

### Agregado
- **Invocar Fryhtan (prueba):** botón nuevo en la Sede de Poder (icono de contratar unidad, último lugar de la fila) que invoca un **Rattus** de tu reino con combate 50.
  - Cuesta el 10% de la barra de oración (40 puntos) y es permanente: no gasta puntos por día ni muere si la Sede es destruida.
  - Si tienes espías propios dentro de la Sede, ese lugar lo ocupa el botón de espías, como en el juego original.
  - Los Fryhtans de un reino no deambulan solos, atacan según su modo agresivo, no entran a guaridas Fryhtan, no dan reputación a quien los ataca y no cuentan para el objetivo de destruir a los Fryhtans.
  - Su gráfico no tiene zonas con el color del reino, así que se ve igual que un Rattus salvaje.
- **Fryhtans en el fuerte:** los Fryhtans de tu reino pueden entrar a tus fuertes como soldados, igual que las armas de asedio, y salir conservando su especie. En el panel del fuerte se muestra su combate. No entrenan con el general. Al salir del fuerte tienen el botón de regresar al fuerte.
- **Retrato pequeño de los Fryhtans:** los datos del juego no tienen retratos pequeños para los Fryhtans (usaban uno persa). Ahora se generan al iniciar el juego reduciendo su retrato grande, y se ven en la lista de soldados del fuerte.

### Cambiado
- **Versión en el menú principal:** se muestra la versión de esta rama y la de `7kmod` en la que está basada, por ejemplo "7kmod-gameplay 0.1.0 (7kmod 0.1.0)". La versión está en `include/ModVersion.h`.

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
