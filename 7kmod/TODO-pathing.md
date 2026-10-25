# 7kmod-pathing: pendientes

Rama experimental para el pathfinding y el movimiento de las unidades. Los
pendientes de gameplay están en `TODO.md`.

---

## Checklist del próximo parche

Cosas anotadas para agregar en un parche. `[x]` = implementado, falta probarlo en partida.

- [x] **Velocidad del grupo: esperar solo cuando van muy adelante.** Hoy (0.4.0) las
  unidades rápidas de un grupo se saltan cuadros todo el tiempo para ir a la velocidad
  del más lento (un Zulú con soldados de velocidad 4 se salta 1 de cada 5 cuadros), y se
  ve como una pausa constante. Nuevo:
  - Las unidades se mueven a **su propia velocidad**.
  - Solo si en el grupo hay una unidad **más lenta**, cada unidad compara **lo que ha
    avanzado desde la orden** (no lo que le falta) con lo que ha avanzado la más lenta.
    Si avanzó **más de 3 casillas más** que ella, se detiene y **espera** hasta que la
    diferencia baje a 1 casilla; luego sigue. Así, en un grupo disperso, una unidad lenta
    que empezó lejos no deja a las demás paradas: todas avanzan desde donde estaban.
  - Solo se espera a las unidades lentas que empezaron **cerca** (a 10 casillas o menos
    de la unidad al dar la orden). Las que estaban lejos no cuentan.
  - Si la más lenta llegó, recibió otra orden, murió o **lleva un rato sin moverse**
    (bloqueada, más de 2 segundos de juego), ya no se espera, para que una unidad atorada
    no deje parado al grupo.
  - Deja de aplicarse igual que hoy: al llegar, al recibir otra orden o al entrar en
    combate.
  - Por confirmar: las 3 casillas para detenerse y 1 para seguir.

- [ ] **Línea de camino mientras el grupo se reúne.** Al reunirse antes de marchar
  (0.6), el camino de cada unidad solo llega a su lugar de la reunión, así que casi no
  se ve la línea hasta que el grupo marcha. Mientras el grupo se reúne, cada unidad
  seleccionada muestra además una **línea recta** desde su lugar en la reunión hasta su
  lugar en el destino (el destino más su posición en la formación), con el mismo estilo
  animado de siempre, en el mapa (`draw_unit_path_on_zoom_map`, `src/OWORLD_Z.cpp`) y en
  el minimapa (`src/OUNITA.cpp`). Al marchar se reemplaza por el camino real.

- [ ] **Espera máxima de la reunión: 3 segundos.** Bajar el tiempo máximo que el grupo
  espera antes de marchar de 150 cuadros (unos 7 segundos) a **unos 65 cuadros (unos 3
  segundos a la velocidad normal)** (`FORM_UP_MAX_FRAMES`, `src/OUNITAM.cpp`). El tiempo
  real depende de la velocidad del juego, porque se cuenta en cuadros. El resto no
  cambia: marcha antes si el 80% de las unidades ya está a 2 casillas o menos de su
  lugar.

- [ ] **Desactivar la espera a las unidades lentas** (0.5.0). Visto en pruebas: se
  comporta con fallas. Las unidades de un grupo vuelven a moverse cada una a su propia
  velocidad, sin esperar a las más lentas (como en el juego original). El código se
  queda, pero desactivado (`UnitArray::set_group_speed` no asigna unidad lenta), por si
  se retoma después. La reunión antes de marchar (0.6) no cambia.

- [ ] **Apurarse al reunirse (+50%).** Mientras el grupo se reúne antes de marchar, las
  unidades que van hacia su lugar en la reunión se mueven un 50% más rápido, como en Age
  of Mythology y Age of Empires IV. No se cambia la velocidad (es un dato compartido por
  tipo de unidad): se procesa **un paso de movimiento extra cada 2 cuadros**, con las
  mismas revisiones de bloqueo, llegada y giro que un paso normal.
  - Solo durante la reunión y solo hacia el lugar de la reunión. Al marchar, al recibir
    otra orden o al entrar en combate, cada unidad vuelve a su velocidad normal.
  - Con la espera máxima de unos 3 segundos, el efecto dura poco.

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
