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
