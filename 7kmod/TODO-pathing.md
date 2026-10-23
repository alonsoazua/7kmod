# 7kmod-pathing: pendientes

Rama experimental para el pathfinding y el movimiento de las unidades. Los
pendientes de gameplay están en `TODO.md`.

---

## Checklist del próximo parche

Cosas anotadas para agregar en un parche. `[x]` = implementado, falta probarlo en partida.

- [ ] **Formación nueva para grupos desordenados.** Hoy la caja mágica copia la
  forma actual del grupo. Con soldados que acaban de salir de un fuerte (repartidos
  alrededor del edificio), eso hace que lleguen en anillo, con un hueco en medio.
  - Medir qué tan apretado está el grupo: si las unidades ocupan menos de la mitad
    de las casillas de su cuadro, se arma una formación nueva. Si está apretado, se
    conserva su forma como ahora.
  - La formación nueva va en filas perpendiculares a la dirección del movimiento,
    con el frente hacia el destino. Número de filas según el tamaño del grupo:
    1 fila hasta 4 unidades, 2 filas hasta 12, 3 filas a partir de 13.
  - Unidades pegadas, sin casilla libre entre ellas.
  - Si el movimiento es en diagonal, la formación también se gira en diagonal (en
    escalera).
  - Pasos estrechos: no se controla la forma en el camino. Cada unidad va por su
    propio camino y el grupo se vuelve a formar en el destino.

- [ ] **Orden por tipo de unidad en la formación nueva.**
  - Cuerpo a cuerpo en la fila del frente.
  - Unidades a distancia (Persas, Egipcios, etc.) en las filas de atrás.
  - Armas de asedio al fondo.
  - El general o el rey en la fila de atrás. Esto cubre en parte el pendiente del
    general que llega primero al fuerte enemigo.

- [ ] **Las unidades rápidas esperan a las lentas.** Sin cambiar la velocidad (es un
  dato compartido por tipo de unidad): mientras el grupo va en camino, una unidad que
  va adelantada respecto a la más lenta se detiene un instante de vez en cuando, para
  que el grupo avance junto. Por ejemplo, Zulúes (5) con armas de asedio (3–4).

- [ ] **Intercambios de lugar más frecuentes.** Cuando una unidad choca con otra
  unidad propia que va hacia otro lado o está quieta en su camino, usar más seguido
  el intercambio de lugar que el juego ya tiene (`swapping`), para que los grupos
  grandes fluyan mejor sin traspasarse.

---

## Descartado por ahora

- **Atravesar a las unidades propias** (colisiones suaves, como en StarCraft 2 o Age
  of Mythology): el mapa guarda una sola unidad por casilla y el movimiento, los
  ataques, la selección y el guardado dependen de eso. Sería rehacer el motor de
  movimiento.
- **Cambiar la forma del grupo en pasos estrechos** (pasar a 2 columnas y volver a
  abrirse): necesita un camino común para el grupo y reacomodo en tiempo real.
  Complicado y frágil.
- **Acelerar a las unidades rezagadas:** riesgoso por la misma razón que cambiar la
  velocidad por unidad.
