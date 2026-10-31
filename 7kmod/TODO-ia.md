# 7k-mod-IA: pendientes

Rama experimental para la IA de los reinos de la computadora (ejecutable
`7kaa-ia.exe`, carpeta `%APPDATA%\7kfans.com\7kaa-ia\`). Incluye `7kmod-pathing`,
`7kmod-gameplay` y `7kmod`; sus pendientes están en `TODO-pathing.md`, `TODO.md` y
`TODO-editor.md`.

- **Cómo se prueba:** `ai_type` en el `config.txt` de la carpeta de la rama: `mixed`
  (por defecto, la mitad de los reinos de la IA con cada IA), `7kmod` (todos con la IA
  experimental) u `original`. El cambio se aplica al reiniciar el juego. En el reporte
  de reinos, los que usan la IA experimental llevan "(7kmod)".
- **Cómo se agrega una regla:** dentro de `config_adv.is_mod_ai_nation(nationRecno)`,
  para que solo la usen los reinos con la IA experimental y la original no cambie.

---

## Checklist

`[x]` = implementado, falta probarlo en partida.

- [x] **Comida: mínimo de aldeanos sin empleo.** La IA original se queda sin comida:
  solo los aldeanos sin empleo producen (30 al año) y todos comen (10), así que hace
  falta que 1 de cada 3 personas del reino sea aldeano sin empleo. La IA original no lo
  cuida (ver el análisis abajo).
  - **Regla:** un pueblo de un reino con la IA experimental no da gente (contratación
    automática de los edificios, reclutamiento de fuertes, órdenes de la IA, entrenar
    unidades) si el reino quedaría con menos de **30%** de aldeanos sin empleo entre
    **toda su gente** (pueblos, soldados en fuertes y Seats, y unidades;
    `Nation::all_population`). Excepciones: los primeros **8** trabajadores de cada
    pueblo, y las primeras **8** personas del reino fuera de los pueblos
    (`Town::mod_ai_keep_peasants`, `src/OTOWN.cpp`).
  - **Primera versión:** contaba solo el 30% dentro de cada pueblo. En la prueba
    (`mixed`, unos 3 años) mantenía aldeanos, pero no frenaba a los soldados: un soldado
    sale del pueblo junto con su lugar en la población, así que el porcentaje no baja.
    Los reinos tenían 30 a 45 personas fuera contra 20 a 30 en sus pueblos, y 2 de los 3
    con la IA nueva seguían perdiendo comida (−280 y −420 al año).
  - **Con la regla del reino** (misma prueba): los 3 reinos con la IA nueva quedaron
    cerca del equilibrio (−120, −20 y +140 al año, con unos 5,000 a 5,800 de comida),
    contra −640, −390 y +50 de los de la IA original (uno se quedó con 0 aldeanos).
    Con 30% todavía pierde un poco: hace falta 33% para no perder comida.
  - Por definir: el porcentaje (30%, 33% o según la comida) después de probar en
    partida.

- [x] **Los soldados se entrenan, ya no se reclutan.** Para todos (jugador y las dos
  IA): solo entran a un fuerte las personas con liderazgo y los espías; los aldeanos y
  los trabajadores con otro oficio tienen que entrenarse en Leadership ($30, 5 días).
  - **Jugador:** mandar una unidad sin liderazgo a un fuerte no hace nada
    (`Unit::firm_can_assign`); el clic en el enlace pueblo-fuerte ya no jala gente
    (`Firm::pull_town_people` regresa 0 para los fuertes); Recruit en el modo de fuerte
    queda desactivado (`Town::fort_recruit_can_pull`), Train sigue. Recruit normal del
    pueblo sigue.
  - **Excepciones:** espías (para meterse en fuertes enemigos), generales y reyes,
    armas, Fryhtans y el editor.
  - **IA:** `FirmCamp::ai_recruit` ya no jala aldeanos, pide la orden de llevar
    soldados al fuerte; `Nation::ai_assign_worker` para un fuerte toma soldados que le
    sobren a otro fuerte o entrena uno (`train_unit`, con `ai_should_spend`), y ya no
    saca aldeanos (`recruit_jobless_worker`). `train_unit` respeta la regla de aldeanos
    de la IA experimental.
  - **Prueba** (`mixed`, mapa de prueba, unos 10 años con el jugador quieto): la IA
    llena sus fuertes entrenando. Al final: reinos con la IA experimental 7 fuertes y 45
    soldados, 5 y 22, 8 y 16; con la original 6 y 23, 10 y 43. Comida de los
    experimentales 9,600, 5,000 y 5,800 contra 2,900 y 1,700 de los originales. El
    jugador quieto ya no fue derrotado en ese tiempo (antes, a los 3 años).
  - Por definir: si la IA entrena lo bastante rápido (uno a la vez por pueblo, 5 días
    cada uno) y si el costo le pesa.

- [x] **Límites de población por reino** (`src/ModLimits.cpp`, opciones de
  `config.txt` guardadas en la partida, 0 = sin límite):
  - `nation_max_town_population=600`: al llegar no hay nacimientos en los pueblos del
    reino (`Town::population_grow`). Conquistas, pueblos que se unen, migración y gente
    que vuelve a los pueblos pueden pasarlo.
  - `nation_max_military_units=300`: cuenta soldados y generales con liderazgo (no el
    rey ni espías), armas de asedio, Caravel, Galleon y Fryhtans, también en fuertes,
    en entrenamiento o en construcción (`mod_military_unit_count`). Al llegar se
    bloquea entrenar Leadership y Promote (`Town::recruit`, `Town::add_queue`,
    `Town::process_queue`), construir armas (`FirmWar`), barcos de guerra
    (`FirmHarbor`), invocar Fryhtans (`FirmBase::can_invoke_fryhtan`) y contratar
    mercenarios con liderazgo (`FirmInn::hire`). Las colas esperan; los botones se
    desactivan con el motivo en su ayuda; al pedirlo aparece un mensaje.
  - La IA respeta los límites (`FirmCamp::ai_recruit`, `FirmWar::think_new_production`).
  - En el reporte de reinos: "Your Population" y "Your Military Units" con su límite.
  - **Prueba** (`mixed`, con límites de prueba de 80 y 12, unos 9 años): la IA se
    detiene en el límite militar, con 1 o 2 de más a veces; los saltos grandes (por
    ejemplo de 16 a 26) y la población por encima de 80 vienen de conquistas, que el
    límite permite.
  - Por definir: si las conquistas y las capturas deben contar para los límites.

- [x] **Impuesto por habitante en `config.txt`** (`town_tax_per_person=5`, como el
  original `TAX_PER_PERSON`; se guarda en la partida). Cambia lo que da cada habitante
  al cobrar impuestos (con la baja de 10 de lealtad, `Town::collect_tax`) y en el
  impuesto anual que no baja la lealtad (`Town::collect_yearly_tax`). Probado con 10:
  un pueblo de 40 da $400.

---

## Análisis: por qué la IA original se queda sin comida

- **Construir:** solo los fuertes nuevos revisan que la comida no vaya en déficit
  (`Nation::think_military`). Las minas, fábricas, torres y fábricas de armas
  adicionales revisan `ai_has_enough_food()`: más de 2,000 a 4,000 de comida **o** sin
  déficit; con las 5,000 iniciales sigue construyendo aunque ya pierda comida.
- **Contratar:** los edificios toman solos a los aldeanos sin empleo de sus pueblos
  enlazados (`Firm::recruit_worker`) y los fuertes reclutan hasta 8 soldados
  (`FirmCamp::think_recruit`), sin revisar la comida.
- **Comprar comida** a otros reinos (`think_request_buy_food`): solo si otro tiene de
  sobra y acepta.
- **No despide ni cierra nada:** `Nation::think_close_camp()` está desactivada en el
  original (regresa 0).
- **El cálculo de comida de la IA** (`yearly_food_consumption`) no cuenta lo que comen
  los Fryhtans de gameplay.
- **Sin comida:** los trabajadores, soldados y unidades pierden 1 de lealtad cada 2 días
  (los aldeanos de los pueblos no) y los Fryhtans 1 de vida al día.
- **Otras ideas:** que en déficit la IA saque trabajadores del edificio menos útil o
  cierre una fábrica de sobra; reactivar el cierre de fuertes; que la regla de construir
  pida reserva para varios años del déficit; contar a los Fryhtans en el cálculo.
