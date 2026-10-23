# 7kmod-pathing: pendientes

Rama experimental para el pathfinding y el movimiento de las unidades. Los
pendientes de gameplay están en `TODO.md`.

---

## Checklist del próximo parche

Cosas anotadas para agregar en un parche. `[x]` = implementado, falta probarlo en partida.

(vacío)

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
