# Opciones avanzadas (`config.txt`)

Seven Kingdoms 2.15.7 tiene unas 45 opciones avanzadas que no aparecen en ningún menú.
Se configuran en un archivo de texto `config.txt`. Este documento explica cada una según
el código fuente (`ConfigAdv.cpp` y los lugares del juego que usan cada opción).

---

## 1. Cómo se usa

### Dónde va el archivo
El juego busca `config.txt` en este orden y usa el primero que encuentra:
1. **La carpeta de configuración del ejecutable.** Para los ejecutables del mod es
   `%APPDATA%\7kfans.com\7kaa-mod\` (la misma carpeta de las partidas guardadas y `CONFIG.DAT`).
   Para el juego oficial es `%APPDATA%\7kfans.com\7kaa\`.
2. **La carpeta desde donde se ejecuta el juego** (por ejemplo `Desktop\7kaa`).

El archivo no existe por defecto: hay que crearlo. Solo se lee al iniciar el juego, así que
cualquier cambio requiere reiniciarlo.

### Formato
- Una opción por línea: `nombre=valor`. Se permiten espacios alrededor del `=`.
- Las líneas que empiezan con `#` son comentarios.
- **Valores de sí/no:** `true` o `false` (no `1`/`0`).
- **Números:** enteros, como `6`.
- Si una línea tiene un nombre o valor inválido, el juego muestra "Error in config.txt at line N"
  y **descarta todo el archivo**, volviendo a los valores por defecto.

### Ejemplo
```
# Cada reino empieza con el pergamino de su raza
nation_start_god_level=1
# Minas que nunca se agotan
mine_unlimited_reserve=true
# Jugar en ventana de 1280x960
vga_full_screen=false
vga_window_width=1280
vga_window_height=960
```

### Multijugador, partidas guardadas y escenarios
Cada opción tiene una de estas marcas en las tablas:

- **[Igual]**: cambia el juego, así que **todos los jugadores de una partida multijugador deben tener el mismo valor**. Si no, el juego no deja unirse (compara una suma de verificación). También se registra en las repeticiones (replays).
- **[Anfitrión]**: en multijugador se usa el valor del anfitrión, que se envía a los demás. También **se guarda en la partida**: al cargar una partida se usan los valores con los que se guardó, no los de tu `config.txt`.
- **[Local]**: solo afecta a tu computadora (pantalla, teclas, idioma, etc.).
- **[Sin sincronizar]**: cambia el juego pero **no se comprueba ni se envía** en multijugador. Si los jugadores tienen valores distintos, la partida puede desincronizarse; úsala solo en partidas de un jugador o asegúrate de que todos tengan el mismo valor.

**Escenarios y tutoriales:** si un escenario trae su propio `config.txt` en su carpeta, el juego lo
aplica mientras se juega ese escenario y después restaura el tuyo (ver `scenario_config`). Por ejemplo,
la carpeta `TUTORIAL` trae uno con `monster_attack_divisor=6` y `town_loyalty_qol=false`.

---

## 2. Opciones de juego

### Inicio de partida

| Opción | Por defecto | Valores | Qué hace | Marca |
|---|---|---|---|---|
| `nation_start_god_level` | `0` | `0`, `1`, `2` | Pergaminos al empezar. `0`: ninguno. `1`: cada reino tiene el pergamino de su raza (puede construir su Sede de Poder). `2`: todos los reinos tienen todos los pergaminos. Aplica a todos los reinos, incluida la IA. | Sin sincronizar |
| `nation_start_tech_inc_all_level` | `0` | `0`, `1`, `2` | Sube ese número de niveles a **todas** las tecnologías de todos los reinos al empezar (catapultas, balistas, etc. ya disponibles). | Sin sincronizar |
| `race_random_list` | todas | `original` | Con `original`, las razas aleatorias (reinos de la IA, pueblos independientes, mercenarios de la posada) solo salen de las 7 razas originales: normandos, mayas, griegos, vikingos, persas, chinos y japoneses. Sin la opción salen también egipcios, indios y zulúes. | Anfitrión |

### Fryhtans (monstruos)

| Opción | Por defecto | Valores | Qué hace | Marca |
|---|---|---|---|---|
| `monster_attack_divisor` | `4` | `1` a `6` | Qué tan seguido los Fryhtans atacan pueblos y edificios. Con la curva original, en promedio hay **un ataque cada N meses**, sin importar cuántas guaridas haya. Más alto = menos ataques. Los ataques empiezan unos 3 años después del inicio. | Anfitrión |
| `monster_alternate_attack_curve` | `false` | `true`/`false` | Curva alternativa: los ataques aumentan con el número de guaridas. Por cada 6 meses: 1 ataque con 0 guaridas, 2 con 5, 3 con 10, 5 con 20, 7 con 40, 11 con 80. Ignora `monster_attack_divisor`. | Anfitrión |

### Reinos de la IA

| Opción | Por defecto | Valores | Qué hace | Marca |
|---|---|---|---|---|
| `nation_ai_defeat_when_no_towns` | `false` | `true`/`false` | Un reino de la IA sin pueblos (población 0) se rinde o queda derrotado, aunque le queden unidades. Pensado para partidas de simulación. | Igual |
| `nation_ai_no_treaty_with_biggest` | `true` | `true`/`false` | Los reinos grandes de la IA no hacen tratados de amistad con su rival más fuerte. Con `false` pueden hacerlo. | Igual |
| `nation_ai_unite_min_relation_level` | `neutral` | `hostile`, `tense`, `neutral`, `friendly`, `alliance`, `off` | Cuando la IA es agresiva y todos los reinos de la IA van muy atrás de los humanos, se rinden al reino de la IA más fuerte para unirse contra el jugador. Esta opción es la relación mínima necesaria para rendirse a él. `off` desactiva la unión. | Anfitrión |
| `fix_ai_consider_trade_treaty` | `true` | `true`/`false` | Corrección: la IA evalúa de verdad los tratados de comercio. Con `false` vuelve el error original: la IA los acepta siempre. | Igual |
| `firm_ai_enable_think_spy_capture` | `false` | `true`/`false` | Los reinos de la IA toman un edificio (de cualquiera, incluido el jugador) cuando sus espías dentro de él son suficientes para capturarlo, y luego mandan tropas contra los fuertes cercanos del dueño anterior. | Igual |

### Pueblos

| Opción | Por defecto | Valores | Qué hace | Marca |
|---|---|---|---|---|
| `town_migration` | `true` | `true`/`false` | La gente de los pueblos se muda a otros pueblos más atractivos. Con `false` no hay migración entre pueblos. | Igual |
| `town_loyalty_qol` | `true` | `true`/`false` | La calidad de vida del pueblo afecta la lealtad: de −17 a +17 (calidad de vida 0 a 100). Con `false` la calidad de vida no influye en la lealtad. | Anfitrión |
| `town_ai_emerge_nation_pop_limit` | `420` | número | Un pueblo independiente solo puede convertirse en un reino nuevo si la población total de todos los reinos es menor que este número. | Anfitrión |
| `town_ai_emerge_town_pop_limit` | `1000` | número | Solo aparecen pueblos independientes nuevos si la población total es menor que este número (y hay menos de 10 independientes). | Anfitrión |
| `firm_migrate_stricter_rules` | `true` | `true`/`false` | Reglas más estrictas para que los trabajadores se muden a otro pueblo: no se mudan a pueblos donde su raza tiene lealtad menor a 40 y el nuevo pueblo debe ser claramente más atractivo. Con `false` se mudan con más facilidad. | Igual |
| `rebel_think_town_action` | `false` | `true`/`false` | Los rebeldes que se asientan en un pueblo pueden, con el tiempo, volverse un pueblo independiente normal o formar un reino nuevo (si el pueblo tiene 20 habitantes o más). | Igual |

### Unidades

| Opción | Por defecto | Valores | Qué hace | Marca |
|---|---|---|---|---|
| `unit_loyalty_require_local_leader` | `true` | `true`/`false` | Un soldado solo recibe el bono de lealtad de su general si el general está **cerca**. Con `false` basta con que tenga general asignado, esté donde esté. | Igual |
| `unit_ai_team_help` | `true` | `true`/`false` | Las unidades de la IA y las independientes que están quietas o yendo a un edificio ayudan a un compañero atacado (si tienen más de 15 de vida). | Igual |
| `unit_finish_attack_move` | `true` | `true`/`false` | Si el objetivo muere o desaparece mientras la unidad camina hacia él, la unidad termina su paso antes de detenerse, en lugar de quedarse a medio camino. | Igual |
| `unit_spy_fixed_target_loyalty` | `false` | `true`/`false` | Comportamiento original: la lealtad de los espías siempre está entre 70 y 90, lo que los delataba. Con `false` (por defecto) su lealtad se comporta como la de una unidad normal. | Igual |
| `unit_target_move_range_cycle` | `false` | `true`/`false` | Las unidades con varios ataques a distancia equivalentes los van alternando al llegar al alcance. | Igual |
| `unit_allow_path_power_mode` | `false` | `true`/`false` | Activa un modo de búsqueda de caminos que tiene en cuenta los territorios de los reinos. Está desactivado por defecto. | Igual |
| `firm_mobilize_civilian_aggressive` | `false` | `true`/`false` | Por defecto, los constructores y los trabajadores civiles que salen de un edificio quedan en modo no agresivo (no atacan solos). Con `true` salen en modo agresivo. Las unidades de la IA y los soldados con liderazgo no cambian. | Igual |

### Correcciones de errores

| Opción | Por defecto | Valores | Qué hace | Marca |
|---|---|---|---|---|
| `fix_path_blocked_by_team` | `true` | `true`/`false` | Mejor manejo cuando una unidad se bloquea con un compañero quieto del mismo grupo. Con `false` intercambian sus destinos, como en el original. | Igual |
| `fix_recruit_dec_loyalty` | `true` | `true`/`false` | Corrige el cálculo de la lealtad que se pierde al reclutar sin entrenar: la penalización acumulada se cuenta de forma consistente, y la estimación que usa la IA se redondea hacia arriba. | Igual |
| `fix_sea_travel_final_move` | `true` | `true`/`false` | Las unidades de la IA que viajan en barco siguen hacia su destino final después de desembarcar, en lugar de quedarse en la costa. | Igual |
| `fix_town_unjob_worker` | `true` | `true`/`false` | Cuando un pueblo necesita que un trabajador deje su empleo, no saca a espías por error y no falla si el trabajador no puede renunciar. | Igual |
| `game_file_patching` | `true` | `true`/`false` | Corrige partidas guardadas muy antiguas (versión menor a 200): reinicia el tipo de reabastecimiento de los mercados del jugador, que en esas versiones no se inicializaba. | Igual |

### Recursos y construcción

| Opción | Por defecto | Valores | Qué hace | Marca |
|---|---|---|---|---|
| `mine_unlimited_reserve` | `false` | `true`/`false` | Las minas nunca se agotan: su reserva nunca baja de 1,500. | Igual |
| `wall_building_allowed` | `false` | `true`/`false` | Activa el procesamiento de murallas en el mapa (las murallas existentes crecen y se deterioran con el tiempo). **No agrega un botón para construirlas**: el juego no tiene ningún botón que dé la orden de construir murallas, así que en la práctica solo afecta a mapas o escenarios que ya las traen. | Igual |

### Velocidad y avisos

| Opción | Por defecto | Valores | Qué hace | Marca |
|---|---|---|---|---|
| `game_new_default_frame_speed` | `12` | número | Velocidad del juego (cuadros por segundo) al empezar una partida nueva. | Igual |
| `game_load_default_frame_speed` | `9` | número | Velocidad del juego al cargar una partida o un escenario. | Igual |
| `news_notify_complete` | `false` | `true`/`false` | Muestra una noticia cuando termina la construcción de un edificio, el entrenamiento de una unidad o la fabricación de un barco o arma. | Local |

---

## 3. Opciones de pantalla, teclas y red

| Opción | Por defecto | Valores | Qué hace |
|---|---|---|---|
| `vga_full_screen` | `true` | `true`/`false` | Pantalla completa. Con `false` el juego abre en ventana. (El parámetro de línea de comandos `-win` hace lo mismo.) |
| `vga_full_screen_desktop` | `true` | `true`/`false` | Pantalla completa "de escritorio": usa la resolución de tu monitor y escala la imagen. Con `false` cambia el monitor a 800×600, y la ventana también queda fija en 800×600. |
| `vga_keep_aspect_ratio` | `true` | `true`/`false` | Al escalar, mantiene la proporción 4:3 con bandas negras. Con `false` estira la imagen para llenar la pantalla. |
| `vga_window_width` / `vga_window_height` | automático | número | Tamaño de la ventana en modo ventana. Si no se indican, se elige automáticamente (1024×768 si cabe). Hay que indicar los dos. |
| `vga_allow_highdpi` | `false` | `true`/`false` | Permite el modo de alta densidad de pantalla (monitores HiDPI / escalado de Windows). |
| `vga_pause_on_focus_loss` | `false` | `true`/`false` | Pausa el juego cuando la ventana pierde el foco (solo en partidas de un jugador). |
| `fix_world_warp_slop` | `2` | número | Margen en píxeles desde el borde de la pantalla para desplazar el mapa con el ratón cuando el ratón está capturado por la ventana. |
| `bindkey` | — | `EVENTO,tecla` | Cambia la tecla de una acción. Se puede repetir para varias acciones. Ejemplo: `bindkey=KEYEVENT_TOWN_TRAIN,shift+t`. La tecla usa los nombres de SDL (`a`, `F5`, `Space`…) y admite `shift+`, `ctrl+` o `alt+`. Eventos disponibles: ver lista abajo. |
| `locale` | (sistema) | código de idioma | Idioma del juego, como `es`. **En los ejecutables del mod no tiene efecto en los textos**, porque se compilan sin soporte de traducciones. |
| `mp_service_addr` | `battle.7kfans.com` | dirección | Servidor para buscar partidas multijugador por internet. |
| `remote_compare_random_seed` | `true` | `true`/`false` | En multijugador, compara cada cuadro el generador de números aleatorios de todos los jugadores para detectar desincronización. |
| `remote_compare_object_crc` | `true` | `true`/`false` | En multijugador, compara también una suma de verificación de los objetos del juego. |
| `scenario_config` | `true` | `true`/`false` | Permite que los escenarios y tutoriales apliquen su propio `config.txt`. |

**Eventos para `bindkey`:**
`KEYEVENT_FIRM_BUILD`, `KEYEVENT_FIRM_PATROL`, `KEYEVENT_TOWN_RECRUIT`, `KEYEVENT_TOWN_TRAIN`,
`KEYEVENT_UNIT_BUILD`, `KEYEVENT_UNIT_RETURN`, `KEYEVENT_UNIT_SETTLE`, `KEYEVENT_UNIT_UNLOAD`,
`KEYEVENT_UNIT_CANCEL`, `KEYEVENT_BUILD_BASE`, `KEYEVENT_BUILD_CAMP`, `KEYEVENT_BUILD_FACTORY`,
`KEYEVENT_BUILD_HARBOR`, `KEYEVENT_BUILD_INN`, `KEYEVENT_BUILD_MARKET`, `KEYEVENT_BUILD_MINE`,
`KEYEVENT_BUILD_MONSTER`, `KEYEVENT_BUILD_RESEARCH`, `KEYEVENT_BUILD_WAR_FACTORY`,
`KEYEVENT_MAP_MODE_CYCLE`, `KEYEVENT_MAP_MODE0`, `KEYEVENT_MAP_MODE1`, `KEYEVENT_MAP_MODE2`,
`KEYEVENT_REPORT_OPAQUE_TOGGLE`, `KEYEVENT_CLEAR_NEWS`, `KEYEVENT_OPEN_DIPLOMATIC_MSG`,
`KEYEVENT_OPEN_OPTION_MENU`, `KEYEVENT_TUTOR_PREV`, `KEYEVENT_TUTOR_NEXT`, `KEYEVENT_SAVE_GAME`,
`KEYEVENT_LOAD_GAME`, `KEYEVENT_OBJECT_PREV`, `KEYEVENT_OBJECT_NEXT`,
`KEYEVENT_NATION_OBJECT_PREV`, `KEYEVENT_NATION_OBJECT_NEXT`, `KEYEVENT_GOTO_RAW`,
`KEYEVENT_GOTO_KING`, `KEYEVENT_GOTO_GENERAL`, `KEYEVENT_GOTO_SPY`, `KEYEVENT_GOTO_SHIP`,
`KEYEVENT_GOTO_CAMP`, `KEYEVENT_GOTO_SELECTED`, `KEYEVENT_MANUF_QUEUE_UP`,
`KEYEVENT_MANUF_QUEUE_DOWN`, `KEYEVENT_MANUF_QUEUE_ADD`, `KEYEVENT_MANUF_QUEUE_ADD_BATCH`,
`KEYEVENT_MANUF_QUEUE_REMOVE`, `KEYEVENT_MANUF_QUEUE_REMOVE_BATCH`,
`KEYEVENT_SEL_GROUP_1` … `KEYEVENT_SEL_GROUP_6`.

---

## 4. Archivos especiales

Además de `config.txt`, el código revisa si existen ciertos archivos (vacíos sirven) en la carpeta
desde donde se ejecuta el juego:

- `SYNC1.SYS` / `SYNC2.SYS`: activan las comprobaciones de sincronización de multijugador, igual que `remote_compare_random_seed` y `remote_compare_object_crc`.
- `TECHGOD.SYS`: solo en multijugador, los reinos humanos empiezan con 3 niveles extra en todas las tecnologías y con todos los pergaminos. Es una herramienta de prueba de los desarrolladores originales; no se comprueba entre jugadores.
