# Changelog de 7kmod-pathing (experimental)

Rama experimental con cambios en la búsqueda de caminos (pathfinding). Está basada
en `7kmod-gameplay` e incluye todo lo de esa rama (ver `CHANGELOG-gameplay.md` y
`CHANGELOG.md`). Su ejecutable es `7kaa-pathing.exe`.

Si los cambios funcionan bien en partidas, se pasarán a `7kmod-gameplay` (o a
`7kmod`).

---

## [Sin publicar]

Incluye **7kmod 0.2.0** (editor de escenarios), a través de `7kmod-gameplay`.

### Agregado
- **Apurarse al reunirse (0.7.0):** mientras el grupo se reúne antes de marchar, las
  unidades que van a su lugar de la reunión se mueven un 50% más rápido (un paso de
  movimiento extra cada 2 cuadros, con las mismas revisiones que un paso normal; la
  velocidad de las unidades no se cambia). Al marchar vuelven a su velocidad normal.
- **Destino visible durante la reunión (0.7.1):** mientras un grupo seleccionado se
  reúne, en el mapa se muestra una marca en el destino (el icono de los puntos de ruta)
  y en el minimapa una línea animada desde el centro de la reunión hasta el destino. Al
  marchar se ven los caminos reales, como siempre. En 0.7.0 eran líneas rectas por
  unidad en el mapa, que en distancias medianas y largas quedaban lejos del camino real.
- **Juntarse antes de marchar (0.6.0)**, como en Age of Empires II: si un grupo disperso
  (el caso de la formación nueva) se manda a un destino a más de 15 casillas, primero se
  reúne en formación **donde está** (centrada en el centro del grupo, con el frente hacia
  el destino) y luego marcha al destino conservando la formación.
  - Desde 0.6.1 la reunión es más suelta: una unidad cuenta como lista si está a 2
    casillas o menos de su lugar, y el grupo marcha cuando el 80% está listo, o a los
    45 cuadros (unos 2 segundos; 150 en 0.6.1 y 65 en 0.7.0) aunque no lo esté. Las que faltan reciben la orden de
    marcha igual. En 0.6.0 se esperaba a que todas llegaran exactamente a su lugar
    (hasta 400 cuadros).
  - Las unidades que reciben otra orden, mueren o se van no cuentan.
  - Los grupos juntos o con destino cercano se mueven como antes.
  - No se guarda en la partida: si guardas mientras un grupo se está reuniendo, al
    cargar se queda reunido donde estaba y hay que volver a mandarlo.

### Cambiado
- **Desactivada la espera a las unidades lentas (0.7.0):** en las pruebas se comportaba con
  fallas. Cada unidad del grupo vuelve a moverse a su propia velocidad, como en el juego
  original. El código se queda desactivado por si se retoma.
- **Velocidad del grupo: esperar solo cuando van muy adelante (0.5.0).** Antes (0.4.0)
  las unidades rápidas de un grupo hacían pausas todo el tiempo para ir a la velocidad
  del más lento, y una sola unidad lenta lejana frenaba a toda la selección. Ahora cada
  unidad se mueve a su propia velocidad, y solo espera a la unidad más lenta que estaba
  **a 10 casillas o menos** de ella al dar la orden:
  - Compara lo que ha avanzado desde la orden con lo que ha avanzado la lenta. Si avanzó
    más de 3 casillas más, se detiene hasta que la diferencia baje a 1 casilla.
  - Deja de esperarla si la lenta llegó, se detuvo, recibió otra orden o murió, o si
    lleva esperando unos 60 cuadros (la lenta puede estar bloqueada); en ese caso ya no
    la vuelve a esperar en ese movimiento.
  - Como antes, deja de aplicarse al llegar, al recibir otra orden o al entrar en
    combate, y no se guarda en la partida.

### Agregado
- **Formación nueva para grupos dispersos (0.4.0):** si el grupo no está junto (no
  cabe en un cuadro de 10×10 o sus unidades ocupan menos de la mitad del cuadro, como
  los soldados que acaban de salir de un fuerte y quedan repartidos alrededor), ya
  no se copia su forma: se arma una formación nueva en el destino, centrada en el
  clic.
  - Filas perpendiculares a la dirección del movimiento, con el frente hacia el
    destino: 1 fila hasta 4 unidades, 2 filas hasta 12 y a partir de 13, al menos 3
    filas de 10 unidades como máximo (desde 0.4.1; en 0.4.0 eran siempre 3 filas,
    lo que hacía muy anchos los grupos grandes). Por ejemplo, 30 unidades van en 3
    filas de 10, 50 en 5 filas y 100 en 10 filas.
    Unidades pegadas. Si el movimiento es en diagonal, la formación también va en
    diagonal (en escalera).
  - Orden por tipo, del frente hacia atrás: cuerpo a cuerpo, unidades a distancia,
    armas de asedio y caravanas, y al final el general o el rey.
  - Dentro de cada fila las unidades se acomodan según su posición actual, para que
    no se crucen en el camino.
  - Si das clic muy cerca del centro del grupo disperso (a menos de 3 casillas), se
    juntan alrededor del clic como en el original.
  - En pasos estrechos no se controla la forma: cada unidad va por su camino y el
    grupo se vuelve a formar en el destino.
- **El grupo avanza a la velocidad del más lento (0.4.0):** al mover un grupo de
  unidades terrestres, las más rápidas hacen una pausa de vez en cuando para avanzar
  al ritmo de la más lenta (por ejemplo, Zulúes con armas de asedio). La velocidad
  de las unidades no se cambia (es un dato compartido por tipo de unidad). Deja de
  aplicarse cuando la unidad llega, recibe otra orden o entra en combate. No se
  guarda en la partida: al cargar, las unidades que iban en camino siguen a su
  velocidad normal.
- **Intercambio de lugar en la formación (0.4.0):** cuando una unidad choca con un
  compañero del mismo grupo que ya llegó a su lugar y es del mismo tipo y rango
  (soldados), intercambian sus lugares en vez de empujarlo fuera de la formación: el
  compañero sigue hacia el lugar de la unidad y ella ocupa el suyo. Es el
  comportamiento del juego original para ese caso. Con unidades de otro tipo se
  sigue empujando al compañero a un lado, como antes.
- **Movimiento en formación, "caja mágica" (0.3.0):** al mover un grupo de unidades
  terrestres, si el grupo está junto (cabe en un cuadro de 10×10 casillas) y das
  clic fuera del área que ocupa, cada unidad va al punto del clic más su posición
  relativa al centro del grupo, así que el grupo llega con la misma forma en que
  estaba. Cada unidad busca su propio camino a su lugar (no comparten camino), para
  que no se crucen. Si el lugar de una unidad no es caminable, está ocupado o queda
  en otra región, se usa la casilla libre más cercana. Si das clic dentro del área
  del grupo, si el grupo está disperso, si hay más de 100 unidades o si alguna no es
  de tamaño 1×1, se juntan alrededor del clic como en el juego original.
  - Las unidades se mueven cada una a su velocidad (no a la del más lento): la
    velocidad es un dato compartido por tipo de unidad y el juego la usa en muchas
    comprobaciones de llegada y ataque, así que cambiarla por unidad es arriesgado.
    Como la mayoría de los soldados tienen velocidad 4, en la práctica la formación
    se mantiene; las que sí se separan son las mezclas con armas de asedio, Zulúes
    (5) o Fryhtans.

### Corregido
- **Rodeos en movimientos cortos (0.2.1):** con la heurística nueva, al mover una
  unidad unas pocas casillas en línea recta a veces daba un rodeo. Como un paso en
  diagonal cuesta lo mismo que uno recto, un camino que se desvía a un lado y
  regresa empataba con el recto, y la búsqueda podía elegirlo. Ahora la estimación
  sube un poco cuando el camino se aparta de la línea hacia el destino (la menor de
  las dos distancias suma (menor+1)/2), así que en un empate gana el camino recto.

### Agregado
- **Paso lateral al chocar (0.2.0):** cuando una unidad terrestre choca con otra que
  avanza o espera (en una multitud o una fila), antes solo esperaba su turno. Ahora,
  después de esperar un momento, intenta pasar por una de las dos casillas en
  diagonal hacia adelante (45° a cada lado de su dirección) y sigue hacia el
  siguiente punto de su camino. Elige el lado que la deja más cerca de su camino, y
  con la misma distancia la mitad de las unidades prueba primero un lado y la otra
  mitad el otro, para que la multitud se abra hacia los dos lados en vez de formar
  una fila india. Solo lo hace si las casillas hasta el siguiente punto del camino
  se pueden caminar, y no a 2 casillas o menos del destino, para que las unidades
  se acomoden normalmente al llegar (`Unit::try_sidestep` en `src/OUNITHB.cpp`).
- **Buscar otro carril al quedar atascada (0.2.0):** cuando una unidad lleva un rato
  bloqueada y vuelve a buscar camino (`Unit::search_or_wait`), la búsqueda ahora
  cuenta las casillas con unidades como más caras: +2 por cada unidad quieta,
  esperando o peleando y +1 por cada unidad que avanza, hasta +4 por bloque. No las
  prohíbe. Antes, las unidades de su mismo grupo que estaban a más de 3 casillas
  contaban como libres, así que el nuevo camino volvía a pasar por la misma fila;
  ahora busca un carril paralelo si lo hay.

### Cambiado
- **Heurística de la búsqueda de caminos:** la búsqueda (A* sobre bloques de 2×2
  casillas, `src/OSPATH.cpp`) estimaba la distancia al destino como la distancia
  **al cuadrado**, mucho mayor que el costo real del camino (1 o 2 por bloque). Eso
  la volvía una búsqueda "voraz" que iba derecho al destino: ante obstáculos
  cóncavos (un lago en U, un bosque con entrada lateral, una bahía) se metía al
  fondo y gastaba todos sus nodos ahí, y los caminos no eran los más cortos. Ahora
  usa el número de bloques hasta el destino contando las diagonales (distancia de
  Chebyshev) con un peso de 3/2, para que rodee los obstáculos sin volverse lenta.
- **Presupuesto de la búsqueda, 8 veces mayor:** el juego original permitía 2,400
  nodos por cuadro compartidos entre todas las unidades y 1,600 por búsqueda, y no
  empezaba búsquedas nuevas con menos de 400 disponibles. Ahora son 19,200, 12,800
  y 3,200. Hay menos retrasos al dar órdenes a muchas unidades y menos búsquedas que
  fallan por quedarse sin nodos en mapas grandes.

### Corregido
- **Punto más cercano cuando no hay camino:** si una búsqueda no llega al destino,
  la unidad debe caminar al punto explorado más cercano a él. El código original
  solo comparaba dos nodos (el primero de la lista abierta y el primero de la
  cerrada, ordenadas por costo total y no por distancia), así que a veces la unidad
  iba a un punto lejano o a un callejón. Ahora se busca entre todos los nodos de la
  búsqueda el más cercano al destino (y, entre los igual de cercanos, el más
  cercano al punto de partida).

### Compatibilidad
- **Partidas guardadas:** mismo formato que `7kmod-gameplay`; se pueden cargar en
  las dos. Los caminos ya calculados se guardan como listas de puntos.
- **Multijugador:** la búsqueda de caminos es parte de la lógica del juego, así que
  **todos deben usar `7kaa-pathing.exe`**. Mezclarlo con `7kaa-gameplay.exe` en una
  partida haría que el juego se desincronice.
- **Repeticiones:** las grabadas con otro ejecutable no coincidirán.
