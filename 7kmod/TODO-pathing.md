# 7kmod-pathing: pendientes

Rama experimental para el pathfinding y el movimiento de las unidades. Los
pendientes de gameplay están en `TODO.md`.

---

## Checklist del próximo parche

Cosas anotadas para agregar en un parche. `[x]` = implementado, falta probarlo en partida.

- [ ] **Velocidad del grupo: esperar solo cuando van muy adelante.** Hoy (0.4.0) las
  unidades rápidas de un grupo se saltan cuadros todo el tiempo para ir a la velocidad
  del más lento (un Zulú con soldados de velocidad 4 se salta 1 de cada 5 cuadros), y se
  ve como una pausa constante. Nuevo:
  - Las unidades se mueven a **su propia velocidad**.
  - Solo si en el grupo hay una unidad **más lenta**, cada unidad compara lo que le falta
    para llegar con lo que le falta a la más lenta. Si va **más de 3 casillas adelante**,
    se detiene y **espera** hasta que la diferencia baje a 1 casilla; luego sigue.
  - Si la más lenta llegó, se detuvo, recibió otra orden o murió, ya no se espera.
  - Deja de aplicarse igual que hoy: al llegar, al recibir otra orden o al entrar en
    combate.
  - Por confirmar: las 3 casillas para detenerse y 1 para seguir.

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
