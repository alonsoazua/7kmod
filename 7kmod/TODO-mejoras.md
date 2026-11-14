# Mejoras generales de 7kmod

Ideas y mejoras de interfaz que sirven para todos los reinos (humanos y, en
`7kmod-gameplay`, también Fryhtan). Se hacen en `7kmod` (New Adversaries Classic) y
pasan a las demás ramas con merge. Lo que solo tiene sentido con las reglas de gameplay
se marca como tal.

---

## Checklist

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
