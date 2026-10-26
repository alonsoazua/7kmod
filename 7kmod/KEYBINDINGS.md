# Teclas y atajos de Seven Kingdoms: Ancient Adversaries

Lista completa de las teclas y combinaciones con el ratón del juego 2.15.7 y de los
ejecutables del mod (`7kaa-mod.exe` y `7kaa-gameplay.exe`), sacada del código. Los
atajos que agrega el mod están marcados como **(mod)**.

## Antes de empezar

- **Las letras solas funcionan sin Shift, Ctrl ni Alt.** Con un modificador apretado,
  el juego busca la combinación con ese modificador, no la letra sola.
- **Las teclas de letras no funcionan mientras escribes** en el chat del informe de
  reinos (F1) o en un cuadro de texto.
- **Muchas teclas cambian según lo que tengas seleccionado.** Por ejemplo, `R` recluta
  en un pueblo, saca a los soldados de un fuerte y regresa una unidad a su fuerte.
- **Teclas configurables:** las acciones que tienen un nombre `KEYEVENT_…` en estas
  tablas se pueden cambiar con la opción `bindkey` de `config.txt`
  (ver `ADVANCED_CONFIG.md`), por ejemplo `bindkey=KEYEVENT_TOWN_TRAIN,shift+t`.
  Las demás están fijas en el código.
- **Distribución del teclado:** las teclas de símbolos (`=`, `-`, `,`, `.`, `\`, `;`,
  `[`, `]`, `'`, `/`) se leen según la tecla que produce ese símbolo en tu teclado. En
  el teclado latinoamericano `=` no tiene tecla propia (es Shift+0), así que los
  atajos con `=` no se pueden usar tal cual. En ese caso, cámbialos con `bindkey`
  a otra tecla. El formato usa `+` para separar el modificador, así que no se puede
  asignar la tecla `+`.

---

## Generales

| Tecla | Acción | Nombre para `bindkey` |
|---|---|---|
| `Esc` | Cierra el informe abierto y regresa al mapa. | (fija) |
| `F1` | Informe de reinos (diplomacia, chat). Otra vez para cerrarlo. | (fija) |
| `F2` | Informe de pueblos. | (fija) |
| `F3` | Informe de economía. | (fija) |
| `F4` | Informe de comercio. | (fija) |
| `F5` | Informe militar. | (fija) |
| `F6` | Informe de tecnología. | (fija) |
| `F7` | Informe de espionaje. | (fija) |
| `F8` | Clasificación de los reinos. | (fija) |
| `F9` | Registro de noticias. | (fija) |
| `F10` | Menú del juego. | (fija) |
| `F11` | Captura de pantalla (`7K000000.BMP`, `7K000001.BMP`…, en la carpeta de configuración del juego). | (fija) |
| `F12` | Solo con el editor de escenarios (rama `7kmod-editor`): en el editor, prueba el escenario; durante la prueba, regresa al editor. | (fija) |
| `S` | Guardar partida. | `KEYEVENT_SAVE_GAME` |
| `L` | Cargar partida. | `KEYEVENT_LOAD_GAME` |
| `O` | Menú de opciones. | `KEYEVENT_OPEN_OPTION_MENU` |
| `P` | Alterna los informes entre opacos y transparentes. | `KEYEVENT_REPORT_OPAQUE_TOGGLE` |
| `X` | Borra las noticias que están en pantalla. | `KEYEVENT_CLEAR_NEWS` |
| `D` | Abre el mensaje diplomático pendiente más antiguo. | `KEYEVENT_OPEN_DIPLOMATIC_MSG` |
| `,` / `.` | Tutorial: texto anterior / siguiente. | `KEYEVENT_TUTOR_PREV` / `KEYEVENT_TUTOR_NEXT` |

### Ventana y ratón

| Tecla | Acción |
|---|---|
| `Alt+Enter` | Pantalla completa / ventana. |
| `Alt+F4` | Salir del juego. |
| `Alt+Tab` | Minimiza el juego (solo en pantalla completa). |
| `Ctrl+G` | Captura o libera el ratón dentro de la ventana. |
| `Ctrl+M` | Cambia el modo del ratón (absoluto / relativo). |

### Velocidad del juego

| Tecla | Acción |
|---|---|
| `1` a `8` | Velocidad de 3 a 24 cuadros por segundo (la tecla × 3). `4` es la velocidad normal (12). |
| `9` | Velocidad máxima. |
| `0` o `Espacio` | Pausa / continuar. |

### Mapa pequeño

| Tecla | Acción | Nombre para `bindkey` |
|---|---|---|
| `Q` | Mapa de terreno. | `KEYEVENT_MAP_MODE0` |
| `W` | Mapa de territorios. Otra vez para mostrar u ocultar los colores de los reinos. | `KEYEVENT_MAP_MODE1` |
| `E` | Mapa de puntos (solo muestra las unidades y edificios). | `KEYEVENT_MAP_MODE2` |
| (sin tecla) | Pasa al siguiente modo de mapa. | `KEYEVENT_MAP_MODE_CYCLE` |

### Ir a…

Cada vez que presionas la tecla, pasa al siguiente de ese tipo, lo selecciona y centra
la vista en él.

| Tecla | Acción | Nombre para `bindkey` |
|---|---|---|
| `K` | Tu rey. | `KEYEVENT_GOTO_KING` |
| `G` | Tus generales. | `KEYEVENT_GOTO_GENERAL` |
| `F` | Tus fuertes. | `KEYEVENT_GOTO_CAMP` |
| `Y` | Tus espías que están en el mapa (no los que están dentro de edificios o pueblos). | `KEYEVENT_GOTO_SPY` |
| `H` | Tus barcos, incluidos los que están en puertos. | `KEYEVENT_GOTO_SHIP` |
| `J` | Un yacimiento de materia prima. | `KEYEVENT_GOTO_RAW` |
| (sin tecla) | Centra la vista en la unidad seleccionada (también se hace con clic en su nombre en el panel). | `KEYEVENT_GOTO_SELECTED` |

### Pasar entre objetos del mismo tipo

Funciona con una unidad, un edificio, un pueblo o un yacimiento seleccionado.

| Tecla | Acción | Nombre para `bindkey` |
|---|---|---|
| `↑` / `↓` | Anterior / siguiente del mismo tipo, de cualquier reino. | `KEYEVENT_OBJECT_PREV` / `KEYEVENT_OBJECT_NEXT` |
| `←` / `→` | Anterior / siguiente del mismo tipo, del mismo reino. | `KEYEVENT_NATION_OBJECT_PREV` / `KEYEVENT_NATION_OBJECT_NEXT` |

En los menús de entrenamiento y producción, `↑` y `↓` mueven la selección de la cola
(ver más abajo).

### Grupos de unidades

| Tecla | Acción | Nombre para `bindkey` |
|---|---|---|
| `Ctrl+1` a `Ctrl+9` | Asigna las unidades seleccionadas al grupo 1–9. | `KEYEVENT_SET_GROUP_1` … `_9` |
| `Alt+1` a `Alt+9` | Selecciona el grupo 1–9. | `KEYEVENT_SEL_GROUP_1` … `_9` |

---

## Ratón en el mapa

| Acción | Resultado |
|---|---|
| Clic izquierdo | Selecciona una unidad, edificio o pueblo. |
| Arrastrar con clic izquierdo | Selecciona tus unidades dentro del recuadro. **(mod)** Si hay al menos una unidad militar (soldados, generales, rey, armas, barcos de guerra, Fryhtans), solo se seleccionan las militares; si solo hay civiles, se seleccionan todas. |
| `Alt` + arrastrar **(mod)** | Selecciona todas tus unidades dentro del recuadro, militares y civiles, como en el juego original. |
| Recuadro sin unidades tuyas **(mod)** | Si dentro hay unidades de otros reinos (o Fryhtans salvajes, rebeldes…), selecciona la más importante (rey, general, luego la de más vida) y el botón `>` del panel muestra la lista de todas las del recuadro. Si hay unidades militares, solo toma las militares (con `Alt`, todas). |
| `Shift` + clic o recuadro | Agrega o quita unidades de la selección actual. |
| `Ctrl` + clic izquierdo | Selección precisa: solo lo que está justo bajo el cursor. Sin Ctrl, el juego también toma unidades grandes o en movimiento cercanas. |
| Clic derecho en el terreno | Mueve las unidades seleccionadas. |
| Clic derecho en un enemigo | Ataca. |
| Clic derecho en un edificio o pueblo propio | Asigna las unidades (trabajar, guarnición, vivir, subir a un barco, etc.). |
| Clic derecho en una unidad propia | Selecciona su grupo, por ejemplo todos los soldados que salieron del mismo fuerte. |
| `Shift` + clic derecho en un edificio propio | Ataca tu propio edificio. |
| `Shift` + clic derecho en tu carreta explosiva | Ataca la carreta para detonarla. |
| `Alt` + clic derecho | Agrega un punto de paso a la ruta de las unidades seleccionadas. |
| Clic derecho durante una orden | Cancela la orden en curso, por ejemplo mientras eliges dónde construir. |

En los informes (comercio, militar, espionaje), **doble clic** en una fila lleva a esa
caravana, barco, edificio, tropa o espía.

---

## Paneles de selección

### Unidades

| Tecla | Acción | Nombre para `bindkey` |
|---|---|---|
| `B` | Menú de construcción. | `KEYEVENT_UNIT_BUILD` |
| `T` | Fundar un pueblo. | `KEYEVENT_UNIT_SETTLE` |
| `R` | Regresar a su fuerte. | `KEYEVENT_UNIT_RETURN` |
| `Supr` | Despedir a la unidad (no funciona con el rey). | (fija) |
| (sin tecla) | Botón Cancelar del menú de construcción. | `KEYEVENT_UNIT_CANCEL` |

**Edificio a construir dentro del menú de construcción:** en este ejecutable no tienen
tecla, pero se pueden asignar con `bindkey`:
`KEYEVENT_BUILD_CAMP` (fuerte), `KEYEVENT_BUILD_MINE`, `KEYEVENT_BUILD_FACTORY`,
`KEYEVENT_BUILD_MARKET`, `KEYEVENT_BUILD_RESEARCH`, `KEYEVENT_BUILD_WAR_FACTORY`,
`KEYEVENT_BUILD_INN`, `KEYEVENT_BUILD_HARBOR`, `KEYEVENT_BUILD_BASE` (Sede de Poder),
`KEYEVENT_BUILD_MONSTER`.

### Barcos

| Tecla | Acción | Nombre para `bindkey` |
|---|---|---|
| `R` | Desembarcar a todos. | (fija) |

`KEYEVENT_UNIT_UNLOAD` existe en la lista de `bindkey` (con `R` por defecto), pero el
botón de desembarcar usa la `R` fija, así que cambiar ese evento no tiene efecto.

### Edificios

| Tecla | Acción | Nombre para `bindkey` |
|---|---|---|
| `Supr` | Demoler el edificio, o cancelar la construcción si no está terminado. En los edificios que se venden, venderlo. | (fija) |
| `R` en el fuerte | Sacar a los soldados a patrullar. | `KEYEVENT_FIRM_PATROL` |
| `R` en el mercado | Contratar una caravana. | `KEYEVENT_FIRM_PATROL` |
| `R` en la posada | Contratar a la unidad seleccionada. | `KEYEVENT_FIRM_PATROL` |
| `R` en el puerto | Zarpar con el barco seleccionado. | `KEYEVENT_FIRM_PATROL` |
| `B` en la torre de ciencia | Elegir qué investigar. | `KEYEVENT_FIRM_BUILD` |
| `B` en la fábrica de guerra | Elegir qué arma fabricar. | `KEYEVENT_FIRM_BUILD` |
| `B` en el puerto | Elegir qué barco construir. | `KEYEVENT_FIRM_BUILD` |

Clic derecho en el botón del constructor también retira al constructor del edificio.

### Pueblos

| Tecla | Acción | Nombre para `bindkey` |
|---|---|---|
| `R` | Reclutar. | `KEYEVENT_TOWN_RECRUIT` |
| `B` | Abrir el menú de entrenamiento. | `KEYEVENT_TOWN_TRAIN` |
| Clic derecho en **Collect Tax** | Configurar el cobro automático de impuestos. | — |
| Clic derecho en **Grant** | Configurar la donación automática. | — |

### Colas de entrenamiento y producción

Funcionan en el menú de entrenamiento del pueblo, en la fábrica de guerra y en el
puerto.

| Tecla / ratón | Acción | Nombre para `bindkey` |
|---|---|---|
| `↑` / `↓` | Cambia la profesión, arma o barco seleccionado. | `KEYEVENT_MANUF_QUEUE_UP` / `KEYEVENT_MANUF_QUEUE_DOWN` |
| `=` | Agrega uno a la cola. | `KEYEVENT_MANUF_QUEUE_ADD` |
| `Shift+=` | Agrega un lote. | `KEYEVENT_MANUF_QUEUE_ADD_BATCH` |
| `-` | Quita uno de la cola. | `KEYEVENT_MANUF_QUEUE_REMOVE` |
| `Shift+-` | Quita un lote. | `KEYEVENT_MANUF_QUEUE_REMOVE_BATCH` |
| Clic izquierdo / derecho en el botón | Agrega / quita uno. | — |
| `Shift` + clic | Agrega o quita un lote. | — |

Tamaño del lote: **8** en los pueblos, **10** en la fábrica de guerra y **5** en el
puerto.

### Agregados por el mod

| Dónde | Tecla / ratón | Acción |
|---|---|---|
| Pueblo en modo fuerte, botón Train **(mod, ambas ramas)** | `Shift` + clic | Pone 8 soldados en cola para el fuerte. |
| Lista de pueblos del botón Recruit del fuerte **(mod, ambas ramas)** | Clic derecho en el panel | Cierra la lista, igual que Cancel. |
| Pueblo, botón Train Captain **(mod, gameplay)** | Clic izquierdo / derecho | Agrega / quita un capitán de la cola. |
| Panel de la unidad con varias seleccionadas **(mod, ambas ramas)** | Botón `>` / `<` a la derecha de la barra del nombre | Muestra / oculta la lista de las unidades seleccionadas. Con varias páginas, `<` y `>` abajo cambian de página. |
| Lista de unidades seleccionadas **(mod, ambas ramas)** | Clic izquierdo / derecho en una unidad | Selecciona solo esa unidad (abre su panel) / la quita de la selección. |
| Panel del fuerte, unidades oscurecidas (fuera del fuerte) **(mod, ambas ramas)** | Clic izquierdo | Mueve la cámara a esa unidad y la selecciona. |
| Fuerte, botón Withdraw **(mod, ambas ramas)** | Clic | Todas las unidades que salieron del fuerte regresan a él. Alert / At ease está en la página 2 de los botones del fuerte. |

Los demás botones del mod (Recruit del fuerte, comercio del puerto, reclutamiento en
edificios de producción, tecnologías, Invocar Fryhtan) no tienen tecla.

---

## Cuadros de mensaje y menús

| Tecla | Acción |
|---|---|
| `Enter` | Aceptar (cuadros de mensaje, menú de opciones). En el chat, envía el mensaje. |
| `Esc` | Cancelar o cerrar el cuadro o menú. |
| Primera letra del botón | En los cuadros de confirmación, equivale a ese botón. |
| Clic derecho | En las ventanas de cargar o guardar partida y de escenarios, equivale a Cancelar. |
| `O` / `S` / `L` / `Esc` | En el menú del juego (F10): opciones / guardar / cargar / volver al juego. |
| `R` en el menú principal | Reproduce la última partida multijugador grabada (`NONAME.RPL` en la carpeta de configuración). |

---

## Trucos (solo un jugador)

**Para activarlos**, presiona `Shift+1` tres veces, `Shift+2` tres veces y `Shift+3`
tres veces, en ese orden (`KEYEVENT_CHEAT_ENABLE1` a `3`). Aparece el mensaje "Cheat
Mode Enabled.". No funcionan en multijugador.

| Tecla | Efecto |
|---|---|
| `C` | +$1000. |
| `\` | +1000 de comida. |
| `N` | Todas las tecnologías y todos los Greater Beings disponibles. |
| `/` | Revela todo el mapa. |
| `;` | Agrega habitantes al pueblo seleccionado. |
| `U` | Rey inmortal (activa / desactiva). |
| `Z` | Construcción rápida (activa / desactiva). |
| `=` | Llena los puntos de oración de la Sede de Poder seleccionada. |
| `-` | Termina la construcción del edificio seleccionado o le restaura toda la vida. |
| `[` | +20 de combate a la unidad seleccionada. |
| `]` | +20 de habilidad a la unidad seleccionada. |
| `'` | +20 de habilidad de espionaje al espía seleccionado. |

Con los trucos activos, `=` y `-` hacen el truco y además su acción normal en las colas
de producción.
