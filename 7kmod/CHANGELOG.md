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
  - **Petición de honores:** cuando uno de tus generales llega al penalty máximo (−50), aparece la noticia "Your general, <nombre>, asks to be honored for his service: $<costo>". Al darle clic, la cámara se mueve al general y lo selecciona (o a su fuerte, si está dentro), y se abre el reporte de reinos (F1), igual que con una propuesta diplomática, con tu reino seleccionado y ninguna pestaña seleccionada. Abajo aparece la petición, por ejemplo "General Itzcoatl asks $150 to be honored for his service to the kingdom.", con las opciones **Accept.** (paga los honores) y **Reject.** (no paga). Si no te alcanza el dinero, una segunda línea lo indica y solo aparece Reject. Al elegir, la noticia se quita y regresas a la vista anterior. El juego sigue mientras está abierto. Si el general muere, cambia de reino o deja de ser general, la noticia se elimina y la petición se cierra. Pagar sus honores por otro medio también quita la noticia.
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
- **Documentación:** `SIEGE_WEAPONS.md` explica las armas de asedio: costos de construcción y mantenimiento, qué pasa sin dinero, estadísticas por versión, daño contra unidades y edificios, daño en área, escudos contra proyectiles y detalles de la Spitfire, la Unicorn y la Porcupine.
- **Aviso "Game Paused":** mientras el juego está en pausa aparece "Game Paused" centrado en la parte de arriba del mapa, tanto si pausaste tú (`Espacio` o `0`) como si el juego se pausó solo al perder el foco de la ventana. Desaparece al continuar. En multijugador lo ven todos, porque la pausa es compartida. Antes no había ningún aviso.
- **Unidades fuera del fuerte:** el panel del fuerte sigue mostrando a las unidades que salieron de él, oscurecidas, para que se vea a quién pertenece el fuerte y dónde están sus tropas:
  - **General (o rey):** si el fuerte se queda sin general, en su lugar aparece el retrato oscurecido del general que salió, con su nombre y "Outside of the fort". Si otro general entra al fuerte, se muestra el nuevo como siempre.
  - **Soldados:** los que salieron aparecen oscurecidos, con su barra de vida, en los lugares vacíos de la lista de soldados (hasta completar 8). No cuentan como soldados del fuerte.
  - Al dar clic en uno, la cámara se mueve a esa unidad y la selecciona.
  - Solo se muestran mientras están en el mapa: si mueren, entran a otro edificio o cambian de reino, ya no aparecen. Solo en tus fuertes.
- **Botón Withdraw en el fuerte:** en la página 1 de los botones del fuerte, en el lugar de Alert / At ease. Hace que todas las unidades que salieron del fuerte (general o rey, soldados, armas y Fryhtans) regresen a él, con la misma orden de "regresar al fuerte" del juego. Se desactiva si no hay nadie afuera. El botón **Alert / At ease pasa a la página 2**, en el primer lugar; por eso tus fuertes ahora siempre tienen las flechas de página. Cuando hay espías, su botón va en el segundo lugar de la página 2.
- **Lista de las unidades seleccionadas:** con varias unidades seleccionadas, junto a su número (a la derecha de la barra del nombre del panel) aparece un botón **">"**. Reemplaza el resto del panel con una lista de las unidades, como la de los soldados del fuerte: retrato, barra de vida y nivel de combate (o de habilidad para los civiles). El botón cambia a **"<"** para volver. El orden es rey y generales primero, luego por tipo de unidad y combate. Clic izquierdo en una unidad la selecciona sola y abre su panel; clic derecho la quita de la selección. Al pasar el ratón aparece su nombre. Con más de 32 unidades hay páginas con flechas. La lista se cierra sola si queda una unidad seleccionada.
- **Recuadro sobre unidades de otros reinos:** antes, un recuadro sin unidades tuyas no seleccionaba nada. Ahora, si dentro hay unidades de otros reinos, Fryhtans salvajes, rebeldes o independientes, se selecciona la más importante (rey, luego general, luego la de más vida; no se usa el combate para no revelarlo) y se ve su panel. Igual que con tus unidades, si hay alguna unidad militar en el recuadro solo se toman las militares (con `Alt`, todas). Como la profesión de las unidades de otros reinos no se ve (y así no se delatan los espías), todas sus unidades humanas cuentan como militares; solo las caravanas y los barcos Trader y Transport cuentan como civiles. El número y el botón ">" muestran la lista de todas las del recuadro (hasta 100), agrupadas por reino (primero el de la unidad elegida, al final las que no tienen reino), cada una con el color de su reino en una esquina y **sin nivel de combate**. No reciben órdenes: clic izquierdo abre el panel de una (la lista se conserva y ">" la vuelve a mostrar) y clic derecho la quita de la lista. Mientras el panel muestra una de ellas, todas las de la lista muestran su barra de vida sobre ellas en el mapa, como las unidades seleccionadas.
- **Selección con prioridad militar:** al seleccionar con el recuadro, si dentro hay al menos una unidad militar (soldados, generales, rey, armas de asedio, barcos de guerra Caravel y Galleon, Fryhtans y Greater Beings), solo se seleccionan las militares y se ignoran los civiles (constructores, mineros, trabajadores, investigadores, sacerdotes, caravanas y barcos Trader y Transport). Tus espías cuentan según la habilidad que aparentan: los que parecen soldados (sin habilidad o con liderazgo) se seleccionan con el ejército, y los que aparentan un oficio civil quedan fuera. Si solo hay civiles, se seleccionan como antes. Con `Alt` presionado el recuadro selecciona todas las unidades, como en el juego original. También aplica con `Shift` para agregar a la selección.
- **Retrato del rey en el informe de reinos (F1):** en la lista de reinos, después del color de cada reino aparece el retrato de su rey (por ejemplo, el del rey japonés si el rey es japonés). Se usa la raza del rey actual, porque el sucesor puede ser de otra raza; si el reino se queda un momento sin rey, se muestra el rey de la raza del reino. Al dejar el ratón sobre el retrato aparece la raza del rey (como toda la ayuda del juego, en partidas de un jugador con la ayuda activada).

### Cambiado
- **Versión en el menú principal:** en lugar de "7kmod-test" se muestra la versión del mod, por ejemplo "7kmod 0.1.0". La versión está en `include/ModVersion.h`.

### Corregido
- **Unidades fuera del fuerte:** al dar clic en un soldado oscurecido del fuerte ya no quedan restos de las barras de vida de la lista del fuerte encima del panel del soldado.
- **Fondo de los paneles nuevos:** el fondo de la lista de unidades se aclaraba un poco más cada vez que la lista cambiaba (por ejemplo, al morir una unidad), y el del general fuera del fuerte cada vez que cambiaba. El panel se volvía a pintar encima de sí mismo, y como no es opaco aclaraba lo que ya estaba aclarado. Ahora se pinta de nuevo toda la interfaz, como hace el juego.
- **Lista de unidades de otros reinos:** a veces aparecían unidades que no estaban en el recuadro, como un soldado saliendo de un fuerte al otro lado del mapa. Cuando una unidad muere o entra a un edificio, el juego le da su número interno a la siguiente unidad que se crea; la lista ahora comprueba que cada número siga siendo la misma unidad (tipo, nombre y reino).
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
