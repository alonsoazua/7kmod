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

### Compatibilidad
- **Partidas guardadas:** versión 301 (`GAME_VERSION_7KMOD_AI_TYPE`), que guarda
  `ai_type`. Carga las partidas de gameplay y pathing (versión 300 o anterior) con la
  IA original. Los demás ejecutables del mod no cargan las partidas de esta rama.
