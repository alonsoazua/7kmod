# Mejoras generales de 7kmod

Ideas y mejoras de interfaz que sirven para todos los reinos (humanos y, en
`7kmod-gameplay`, también Fryhtan). Se hacen en `7kmod` (New Adversaries Classic) y
pasan a las demás ramas con merge. Lo que solo tiene sentido con las reglas de gameplay
se marca como tal.

---

## Checklist

- [ ] **Bug: los pueblos rebeldes no se rinden aunque su resistencia llegue a 0.** En
      la práctica son incapturables: al atacarlos, aun con resistencia 0, no se rinden y
      aguantan hasta que matas a todos los aldeanos (lo probó el usuario).
      - **Causa probable:** un pueblo "controlled by rebels" no se rinde mientras su
        grupo tenga rebeldes afuera (`Town::surrender`: `mobile_rebel_count > 0`). Si un
        rebelde se queda atorado lejos, o la cuenta no baja bien (rebeldes que murieron,
        que se unieron a otro grupo, defensores que salieron), el pueblo nunca se rinde.
        Además, las guaridas y los fuertes no le bajan la resistencia
        (`Town::update_target_resistance`).
      - **Revisar:** que `mobile_rebel_count` cuente solo rebeldes vivos y de verdad
        afuera (`Town::mobilize_defender` lo sube, `Town::dec_pop`/regreso lo baja).
      - **Arreglo propuesto:** que el pueblo se rinda con resistencia 0 si los rebeldes
        de afuera están lejos (por ejemplo, ninguno a menos de cierta distancia del
        pueblo) o si no hay ninguno vivo; o simplemente no esperar a los rebeldes de
        afuera.

- [ ] **Bug: Training desde la partida no carga el escenario de la lección.** Al elegir
      otra lección con **Next Training**, o con **Training** del menú de la partida, el
      juego solo cambia el texto de ayuda y deja el mapa de la lección (o partida) en la
      que estabas. Parece que "carga el juego anterior".
      - **Causa:** `Tutor::run(tutorId, inGameCall)` (`OTUTOR.cpp`) solo carga el
        archivo `TUTORIAL\<código>.TUT` cuando se llama desde el menú principal; desde la
        partida (`inGameCall` 1, llamado desde `Tutor::select_run_tutor(1)` en
        `OTUTOR.cpp` y `OINGMENU.cpp`) se lo salta a propósito ("don't load pre-saved
        game when selecting a new tutorial in the game"). Viene así del juego original
        de 7kfans; el mod no lo cambió.
      - **Probado** (Linux, rama Fryhtan): desde el menú principal, Military y Trade
        cargan cada uno su mapa; desde Military, Next Training → Trade deja el mapa de
        Military con el texto de Trade (1/45).
      - **Arreglo propuesto:** no se puede cargar otro escenario en medio de la partida
        que corre (`battle.run_loaded()`). Guardar la lección elegida, terminar la
        partida actual y, al volver al menú principal, arrancar esa lección como lo hace
        Training (`OGAMMAIN.cpp`, `tutor.select_run_tutor(0)` → `Tutor::run`).

- [ ] **Pestaña de mantenimiento en el reporte militar.** Una tercera pestaña en el
      panel de abajo del reporte militar, junto a **Units** y **Contributions**, con lo
      que se espera que el ejército actual cueste al año en **oro** y en **comida**,
      desglosado por tipo de unidad, y el total.
      - **Filas:** una por tipo de unidad (por ejemplo, soldados y generales de cada
        raza, cada arma de asedio, cada barco), con cuántas hay, el oro al año y la
        comida al año. Abajo, el total del ejército.
      - **Qué cuenta:** las unidades afuera y las que están en fuertes (soldados y
        generales). Los fuertes en sí (su costo anual) pueden ir en una fila aparte,
        para que el total sea lo que el ejército le cuesta al reino.
      - **Oro**, según lo que paga hoy el juego: el sueldo de soldados y generales
        afuera (`SOLDIER_YEAR_SALARY`, `GENERAL_YEAR_SALARY`), el costo anual de las armas
        y barcos (`year_cost` de cada unidad) y el costo anual de los fuertes. Hay que
        revisar en el código qué se paga por unidad y qué va incluido en el costo del
        fuerte, para que la pestaña muestre lo mismo que se cobra.
      - **Comida:** cada soldado o general humano come 10 al año
        (`PERSON_FOOD_YEAR_CONSUMPTION`); las armas y los barcos no comen.
      - **En gameplay:** los Fryhtans del reino (invocados, o la horda de un reino
        Fryhtan) comen según su especie, de 20 (Doink) a 50 (Rokken) al año
        (`invoked_fryhtan_food_year`), y salen en sus propias filas por especie.
      - Sirve igual para un reino humano que para uno Fryhtan.
