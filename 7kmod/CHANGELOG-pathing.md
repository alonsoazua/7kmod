# Changelog de 7kmod-pathing (experimental)

Rama experimental con cambios en la búsqueda de caminos (pathfinding). Está basada
en `7kmod-gameplay` e incluye todo lo de esa rama (ver `CHANGELOG-gameplay.md` y
`CHANGELOG.md`). Su ejecutable es `7kaa-pathing.exe`.

Si los cambios funcionan bien en partidas, se pasarán a `7kmod-gameplay` (o a
`7kmod`).

---

## [Sin publicar]

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
