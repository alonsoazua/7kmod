# Changelog de 7kmod-pathing (experimental)

Rama experimental con cambios en la búsqueda de caminos (pathfinding). Está basada
en `7kmod-gameplay` e incluye todo lo de esa rama (ver `CHANGELOG-gameplay.md` y
`CHANGELOG.md`). Su ejecutable es `7kaa-pathing.exe`.

Si los cambios funcionan bien en partidas, se pasarán a `7kmod-gameplay` (o a
`7kmod`).

---

## [Sin publicar]

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
