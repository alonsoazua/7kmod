# Changelog de 7kmod

Cambios de la rama `7kmod` (ejecutable `7kaa-mod.exe`) respecto a la versión original
**Seven Kingdoms: Ancient Adversaries 2.15.7** (tag `v2.15.7`).

La rama `7kmod` tiene mejoras de comodidad e interfaz y algunos ajustes de balance.
Los cambios que modifican el estilo de juego están en la rama `7kmod-gameplay`, que
incluye todo lo de `7kmod` y documenta lo suyo en `CHANGELOG-gameplay.md`.

## Cómo se usa este archivo

- Cada cambio nuevo se anota primero en **Sin publicar**.
- Al publicar una versión, esa sección se renombra con el número y la fecha, y se crea
  el tag `7kmod-X.Y.Z` en el commit correspondiente.
- Versiones: el tercer número sube con correcciones, el segundo con funciones nuevas o
  cambios de balance, y el primero queda en 0 mientras el mod siga en pruebas.
- Secciones de cada versión: **Agregado**, **Cambiado**, **Corregido**, **Compatibilidad**.

---

## [Sin publicar]

### Agregado
- **Contribución y honores de los generales:** un general se vuelve infeliz cuando su contribución (1 punto por cada golpe al enemigo) es más del doble de lo que se le ha dado en recompensas; pierde hasta 50 de lealtad. El juego original no lo mostraba en ningún lado. Ahora:
  - **Reporte militar (F5) con pestañas** en el panel de abajo:
    - **Units:** lo que mostraba antes (unidades por tipo).
    - **Contributions:** la lista de tus generales, ordenada por riesgo, con Loyalty (actual y hacia dónde va), Contrib., Status (Content, Unrewarded o At risk! si su lealtad objetivo es menor a 30 y te puede traicionar), Penalty (cuánta lealtad pierde) y Honors (lo que cuesta quitar el penalty, a $30 por recompensa).
    - **Pay selected** paga los honores del general seleccionado y **Pay all** los de todos, en orden de riesgo, hasta que se acaba el dinero. Usan la recompensa normal del juego. Doble clic en un general lleva a él.
  - **Petición de honores:** cuando uno de tus generales llega al penalty máximo (−50), aparece la noticia "Your general, <nombre>, asks to be honored for his service: $<costo>". Al darle clic se abre una ventana con la petición, su lealtad, el penalty y el costo, con **Accept** (paga los honores) y **Reject** (solo cierra la ventana). El juego sigue mientras está abierta. Si el general muere, cambia de reino o deja de ser general, la noticia se elimina y la ventana se cierra. Pagar sus honores por otro medio también quita la noticia.
- **Desglose de la lealtad:** al dejar el ratón sobre la lealtad de un general o un soldado (en el panel de la unidad, del fuerte o de la Sede de Poder) aparece de qué se compone su lealtad objetivo: pueblos enlazados, tropas, raza, reputación, liderazgo del rey y penalty por falta de recompensas para los generales; liderazgo del general, combate y raza para los soldados. Solo para tus unidades. Como toda la ayuda del juego, aparece en partidas de un jugador con la ayuda activada.
- **Unidades seleccionadas:** al seleccionar varias unidades, el número de unidades seleccionadas aparece a la derecha de la barra superior del panel de la unidad.
- **Poder de ataque y armadura en el panel de las unidades:** debajo del combate (y arriba de la contribución) se muestran:
  - **Melee pwr:** el poder del ataque cuerpo a cuerpo: el daño promedio por minuto antes de la armadura del enemigo, a la velocidad normal del juego, con el mismo cálculo del juego. Combina el daño de cada golpe y la velocidad de ataque. Contra un enemigo real, la armadura le quita armadura ÷ 4 a cada golpe (1 con la armadura normal), así que el daño real es menor, sobre todo en unidades de golpes débiles. Se muestra sin armadura para que los bonos se lean exactos: un bono de +100% duplica el poder.
  - **Ranged pwr:** lo mismo para el ataque a distancia, si la unidad lo tiene.
  - **Bono del general:** si el general de la unidad está a 10 casillas o menos, el poder incluye su bono y se muestra con una flecha hacia arriba, como en la lealtad, y el porcentaje: por ejemplo "62 ↑ (+100%)". El porcentaje es el liderazgo del general y es exactamente lo que sube el poder.
  - **Armor:** la armadura de la unidad. En `7kaa-mod.exe` es 4 para todas, como en el juego original.
  - No incluye los ataques especiales que gastan poder. No se muestra en civiles ni en tus propios espías (el menú de espía necesita ese espacio).
  - El panel crece según las líneas que muestra y los botones se recorren hacia abajo.
- **Panel de los Fryhtans:** al seleccionar un Fryhtan se muestra su combate, poder de ataque y armadura. Antes no mostraba ningún dato.
- **Documentación:** `GREATER_BEINGS.md` explica en detalle cómo funciona cada Greater Being (cómo se obtienen, costos, poderes y efectos exactos), según el código y los datos del juego. Incluye lo que revela el Fénix (la información de unidades y edificios enemigos cercanos) y que su detección de espías está desactivada en el código original.
- **Documentación:** `ADVANCED_CONFIG.md` explica las opciones avanzadas ocultas de `config.txt` del juego original: dónde va el archivo, su formato, qué hace cada opción y cómo se comporta en multijugador y en las partidas guardadas.
- **Documentación:** `KEYBINDINGS.md` lista todas las teclas y combinaciones con el ratón del juego y del mod: generales, mapa, grupos, paneles de unidades, edificios y pueblos, colas de producción, menús y trucos, con el nombre de cada acción para cambiarla con `bindkey`.

### Cambiado
- **Versión en el menú principal:** en lugar de "7kmod-test" se muestra la versión del mod, por ejemplo "7kmod 0.1.0". La versión está en `include/ModVersion.h`.

### Corregido
- **Documentación:** en `ADVANCED_CONFIG.md`, la lista de acciones para `bindkey` estaba incompleta; faltaban `KEYEVENT_CHEAT_ENABLE1` a `3`, `KEYEVENT_SEL_GROUP_7` a `9` y `KEYEVENT_SET_GROUP_1` a `9`.
- **Nombres repetidos en las noticias:** en mensajes con dos nombres, como "El espía <nombre> del reino de <rey> ha sido descubierto y ejecutado", el nombre de la unidad salía igual al del rey. La función que arma los nombres usaba una sola memoria compartida, y con el compilador de los ejecutables del mod el segundo nombre sobrescribía al primero. Es un error del juego original que no se nota en la versión oficial por el orden en que su compilador evalúa los nombres.

### Compatibilidad
- Sin cambios en las partidas guardadas ni en el multijugador.

---

## [0.1.0] - 2026-10-18

Primera versión documentada. Reúne todos los cambios hechos hasta ahora sobre la 2.15.7.

### Agregado

#### General
- **Marca en el menú principal:** el texto "7kmod-test" aparece junto a la versión, para distinguir el mod del juego original.
- **Carpeta de configuración y partidas propia (`7kaa-mod`):** el mod no mezcla sus partidas guardadas ni su configuración con las del juego oficial.

#### Fuerte: reclutar y entrenar desde el fuerte
- **Botón Recruit en el fuerte** (icono de Train):
  - Si hay varios pueblos propios conectados al fuerte, muestra la lista para elegir uno; si hay uno solo, va directo.
  - Abre el pueblo en "modo fuerte", con solo tres botones: Recruit, Train y "Back to Fort" en una línea. Tax y Grant se ocultan.
- **Recruit en modo fuerte:** la unidad de la raza elegida entra directamente al fuerte.
- **Train en modo fuerte:** pone en cola un soldado de Leadership sin pasar por el menú de entrenamiento. Shift + clic agrega 8.
- **Los soldados entrenados desde el fuerte entran solos al fuerte** al terminar, si sigue siendo tuyo y tiene espacio. Si no, se quedan junto al pueblo como siempre.
- **Contador en el botón Train:** muestra cuántos soldados se están entrenando para ese fuerte.
- **Páginas de botones para el espía:** si tienes espías en tu propio fuerte, aparecen flechas "<" ">" bajo la fila de botones para cambiar a la página con el botón de espías.

#### Opciones de partida
- **Seats of Power Unlocked?** (página Advanced Options II, partida de un jugador):
  - Con "Yes", tu reino empieza sabiendo construir la Sede de Poder (Seat of Power) de su raza, como si tuviera el pergamino.
  - Solo aplica al reino del jugador, no a la IA, y no funciona en multijugador.
  - La elección se guarda en `7KMOD.DAT`; `CONFIG.DAT` conserva el formato original.

### Cambiado

#### Pueblos independientes
- **Población equilibrada al inicio:** todos los pueblos independientes iniciales tienen de 15 a 29 habitantes, sin importar cuántas razas tengan. Antes, los mixtos eran más pequeños (2 razas: 12–22, 3 razas: 11–19).
- **Menor resistencia en pueblos mixtos:** −8 con 2 razas y −12 con 3 o más, con un mínimo de 10. Aplica a los pueblos iniciales y a los que aparecen después.

#### Interfaz
- El botón Cancel de la lista de pueblos es más alto, para que el texto quepa.

### Compatibilidad
- **Partidas guardadas:** versión de partida **290** (agrega el fuerte destino de las unidades en entrenamiento). El mod carga partidas del juego oficial 2.15.x; el juego oficial no carga las partidas del mod.
- **Multijugador:** el entrenamiento para el fuerte usa el mensaje de red nuevo `MSG_TOWN_TRAIN_FOR_FIRM`. Todos los jugadores necesitan el mismo ejecutable del mod.
