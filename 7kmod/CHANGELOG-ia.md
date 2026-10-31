# Changelog de 7k-mod-IA (experimental)

Rama experimental para probar cambios en la IA de los reinos de la computadora. Está
basada en `7kmod-pathing` e incluye todo lo de esa rama, de `7kmod-gameplay` y de
`7kmod` (ver `CHANGELOG-pathing.md`, `CHANGELOG-gameplay.md` y `CHANGELOG.md`). Su
ejecutable es `7kaa-ia.exe`.

Si los cambios funcionan bien en partidas, se pasarán a `7kmod-gameplay`.

---

## [Sin publicar]

Versión **0.1.0**, basada en **7kmod-pathing 0.7.1** (gameplay 0.1.0, 7kmod 0.2.0).

### Agregado
- **Carpeta propia:** `7kaa-ia.exe` guarda sus partidas, `CONFIG.DAT` y `config.txt` en
  `%APPDATA%\7kfans.com\7kaa-ia\`, aparte de los demás ejecutables del mod
  (`7kaa-mod`), para que su `config.txt` y sus partidas no se mezclen con ellos.
- **Tipo de IA (`ai_type` en `config.txt`):** `mixed` (por defecto en esta rama),
  `7kmod` u `original`.
  - `original`: la IA del juego.
  - `7kmod`: todos los reinos de la IA usan la IA experimental.
  - `mixed`: la usan los reinos de número par (con el jugador como reino 1, la mitad de
    los reinos de la IA), para compararla con la original en la misma partida.
  - En el reporte de reinos, los que la usan llevan "(7kmod)" junto al nombre (solo si
    la opción no es `original`). Con nombres largos se corta.
  - Se guarda en la partida: al cargarla se usa el valor con el que se guardó (y queda
    así hasta reiniciar el juego). En multijugador se compara entre los jugadores (como
    las opciones marcadas Igual en `ADVANCED_CONFIG.md`).
- **IA experimental, regla de aldeanos:** los pueblos de un reino con la IA
  experimental no dan gente (contratación automática de los edificios, reclutamiento de
  fuertes, órdenes de la IA, entrenar unidades) si el reino quedaría con menos de 30% de
  aldeanos sin empleo entre toda su gente: pueblos, soldados en fuertes y Seats, y
  unidades (lo mismo que come). Siempre se permiten los primeros 8 trabajadores de cada
  pueblo y las primeras 8 personas del reino fuera de los pueblos
  (`Town::mod_ai_keep_peasants`). Es para que la IA no se quede sin comida: solo los
  aldeanos sin empleo producen (30 al año) y todos comen (10).

- **Límites por reino** (`config.txt`, se guardan en la partida; 0 = sin límite):
  - `nation_max_town_population` (**600** por defecto): la gente que vive en los pueblos
    del reino. Al llegar, no hay nacimientos en sus pueblos. Los pueblos conquistados,
    los que se unen y la gente que llega o vuelve a los pueblos pueden pasarlo.
  - `nation_max_military_units` (**300** por defecto): soldados y generales con
    liderazgo (no el rey ni los espías), armas de asedio, barcos de guerra (Caravel y
    Galleon) y Fryhtans del reino, también los que se están entrenando o construyendo.
    Al llegar no se puede entrenar Leadership (ni Promote), construir armas o barcos de
    guerra, invocar Fryhtans ni contratar mercenarios con liderazgo: las colas esperan,
    los botones se desactivan con el motivo en su ayuda, y al pedirlo aparece un
    mensaje. Las unidades ganadas por captura, rendición o conversión pueden pasarlo.
  - La IA respeta los mismos límites.
  - En el reporte de reinos (Information) se ven "Your Population" y "Your Military
    Units" con su límite.

- **Impuesto por habitante** (`town_tax_per_person` en `config.txt`, **5** por defecto,
  como el original; se guarda en la partida): lo que da cada habitante al cobrar
  impuestos en un pueblo y en el impuesto anual automático (`Town::collect_tax`,
  `Town::collect_yearly_tax`).

### Cambiado
- **Los soldados se entrenan, ya no se reclutan** (para todos: jugador y todas las IA).
  - Solo entran a un fuerte las personas con oficio de liderazgo (soldados, generales,
    reyes) y los espías. Los aldeanos y los trabajadores con otro oficio no entran: hay
    que entrenarlos en Leadership en un pueblo (Train, $30 y 5 días)
    (`Unit::firm_can_assign`).
  - Los fuertes ya no jalan aldeanos de los pueblos: ni con el clic en el enlace
    pueblo-fuerte ni con la IA (`Firm::pull_town_people`).
  - En el modo de reclutar desde un fuerte, **Recruit** queda desactivado; **Train**
    sigue funcionando y el soldado entrenado entra solo al fuerte. Recruit en el modo
    normal del pueblo sigue sacando aldeanos.
  - Las armas, los Fryhtans y lo que se coloca con el editor no cambian.
  - **La IA** llena sus fuertes entrenando soldados en sus pueblos (la orden de llevar
    trabajadores a un fuerte ya no saca aldeanos: entrena, `Nation::ai_assign_worker`,
    `FirmCamp::ai_recruit`). Entrena uno a la vez por pueblo, y respeta la regla de
    aldeanos de la IA experimental.

### Compatibilidad
- **Partidas guardadas:** versión 302: 301 guarda `ai_type`
  (`GAME_VERSION_7KMOD_AI_TYPE`) y 302 los límites por reino
  (`GAME_VERSION_7KMOD_NATION_LIMITS`) y 303 el impuesto por habitante
  (`GAME_VERSION_7KMOD_TOWN_TAX`). Las partidas anteriores usan los valores de
  `config.txt`. Carga las partidas de gameplay y pathing (versión 300 o anterior) con la
  IA original. Los demás ejecutables del mod no cargan las partidas de esta rama.
