# 7kmod-gameplay: pendientes

Rama para los cambios que modifican el estilo de juego. La rama `7kmod`
queda solo con mejoras de comodidad (interfaz) sobre la 2.15.7.

---

## Checklist del próximo parche

Cosas anotadas para agregar en un parche. `[x]` = implementado, falta probarlo en partida.

- [ ] **Tooltip con la raza en el retrato del rey (F1)** (comodidad: va en
  7kmod y pasa a gameplay con el rebase).
  - Al dejar el ratón sobre el retrato del rey en la lista de reinos
    (`put_nation_rec` en `src/OR_NAT.cpp`) aparece la raza del rey, por
    ejemplo "Japanese".
  - Se toma la misma raza que decide el retrato: la del rey actual, o la del
    reino si en ese momento no hay rey.
  - Usar `help.set_custom_help` sobre el área del retrato (24×20), como los
    tooltips de lealtad. Igual que toda la ayuda del juego, solo aparece en
    partidas de un jugador y con la ayuda activada.
  - Actualizar el changelog de 7kmod.
- [ ] **Los Fryhtans invocados comen** (gameplay).
  - Consumo por año, en proporción al costo de invocación (de 20 a 50):

    | Fryhtan | Costo | Comida / año |
    |---|---|---|
    | Doink | 30 | 20 |
    | Broosken | 40 | 25 |
    | Deezboanz | 45 | 25 |
    | Karrotten | 50 | 30 |
    | Rattus | 55 | 30 |
    | Holgh | 60 | 35 |
    | Haubudam | 75 | 40 |
    | Wyrm | 75 | 40 |
    | Sauroid | 80 | 45 |
    | Rokken | 90 | 50 |

  - Solo los Fryhtans de un reino (invocados); los salvajes no comen.
  - Comen en cualquier lugar: en el mapa (`Unit::pay_expense`, que hoy solo
    cobra comida a los humanos) y dentro del fuerte (`Firm::consume_food`, que
    hoy solo cuenta a los trabajadores con raza).
  - Sin comida pierden 1 punto de vida por día, en el mapa y dentro del fuerte.
    Se suma a la pérdida por falta de dinero (2 por día si faltan los dos).
    Igual que los humanos, dejan de comer cuando la reserva llega a 0.
  - Por ahora no se muestra el consumo en la interfaz.
  - Actualizar `CHANGELOG-gameplay.md`, `COMBAT_ANALYSIS.md` (sección de los
    Fryhtans invocados) y `SIEGE_WEAPONS.md` (la nota de los Fryhtans sin
    dinero).
- [ ] **Tecnología contra Fryhtans** (gameplay, `ModTech`).
  - Los soldados del reino que la investigó hacen 30% más daño a los
    Fryhtans: tanto a los salvajes como a los invocados por otros reinos.
  - El bono se aplica al daño antes de la armadura, como el bono del general y
    el +25% de los Fryhtans invocados (al recibir el golpe, en
    `Unit::hit_target`, que es donde se conoce al objetivo). Para los
    proyectiles se usa el reino del que disparó, porque el tirador puede haber
    muerto antes de que el proyectil llegue.
  - Solo aplica a los soldados: unidades humanas del reino (soldados,
    generales y rey). No aplica a las armas de asedio, los barcos ni a sus
    propios Fryhtans invocados.
  - Investigación rápida por ahora, para probarla. La IA no la investiga.
  - Partidas guardadas: una tecnología más en `ModTechRes`, subir
    `GAME_VERSION_7KMOD` (296 → 297 o la que siga).
  - Por definir: si también aplica a las guaridas Fryhtan (Fryhtan Lair).
  - Actualizar `CHANGELOG-gameplay.md` y `COMBAT_ANALYSIS.md`.
- [ ] **El bono del general en batalla también aplica al rey** (gameplay).
  - Hoy el general en el mapa con al menos 4 de sus soldados a 10 casillas o
    menos recibe su propio liderazgo como bono de daño y +1 de armadura, pero
    el rey no (`Unit::general_buff_active` en `src/OUNITIF.cpp` exige
    `RANK_GENERAL`). Cambiar para que acepte también `RANK_KING`.
  - Mismas condiciones: solo cuentan las unidades que lo tienen como líder
    (las que salen del fuerte con él con el botón de patrulla o en la defensa
    automática).
  - El rey suele tener liderazgo alto (hasta 100), así que con sus tropas
    llegaría a +100% de daño y +1 de armadura.
  - El panel del rey muestra el bono igual que el del general (flecha hacia
    arriba).
  - Actualizar `CHANGELOG-gameplay.md` (quitar "No aplica al rey") y la sección
    8 de `COMBAT_ANALYSIS.md`.
- [ ] **Bono de lealtad para los pueblos que se rinden** (gameplay).
  - Problema: un pueblo enemigo que se rinde a veces se vuelve a rebelar casi
    al instante.
  - Por qué pasa: al cambiar de reino (`Town::set_nation`), la lealtad del
    pueblo queda en 40 para la raza del rey nuevo y en 30 para las demás. La
    rebelión ocurre con 29 o menos. Si su lealtad objetivo es baja (razas
    mezcladas, sin empleo porque perdió sus edificios, sin mercado, o un fuerte
    enemigo todavía enlazado), en unos 10 días baja de 30 a 29. Las rebeliones
    se revisan cada 15 días, así que puede rebelarse casi de inmediato. Si
    queda un fuerte del reino anterior enlazado y ninguno propio, también puede
    volver a rendirse a ese reino.
  - Propuesta: al rendirse (`Town::surrender`), la lealtad sube 15 puntos (55
    para la raza del rey, 45 para las demás) y el pueblo no puede rebelarse
    durante 90 días (como si acabara de rebelarse: `last_rebel_date`). Da tiempo
    de poner un fuerte con general y darle empleo.
  - Aplica tanto a pueblos de otros reinos como a pueblos independientes
    capturados, y a la IA igual que al jugador. No aplica cuando un reino
    completo se rinde y entrega todos sus pueblos (`NationBase::surrender`).
  - Por definir: los valores (+15 y 90 días).
  - Actualizar `CHANGELOG-gameplay.md`.

---

## Tecnologías del mod (`ModTech`)

Las tecnologías del mod están en `include/ModTech.h` y `src/ModTech.cpp`,
separadas de las armas de `TechRes`. Se investigan en "Technologies" de la
Torre de Investigación y no tocan la diplomacia, los reportes ni las demás
partes que asumen que una tecnología es un arma.

- **Hechas:** Elite Warriors, Elite Workers y Building Repair.
- **Agregar una tecnología:** una entrada en el enum y en
  `mod_tech_info_array` (nombre, descripción, costo, niveles), su efecto con
  `mod_tech_res.get_level(...)`, y subir `GAME_VERSION_7KMOD` con su
  `MAX_MOD_TECH_BEFORE_...` para leer las partidas anteriores
  (`GameFile::read_file_3` en `src/OGFILE2.cpp`).
- **Pendientes:**
  - Costo real de Building Repair (hoy es muy barato, solo para pruebas).
  - Que la IA investigue las tecnologías del mod.
  - Íconos propios (hoy reutilizan imágenes del juego).
  - Más tecnologías. Ideas: entrenar más rápido o más barato, minas o
    fábricas más productivas, bonos de lealtad.

El análisis original de las mejoras permanentes quedó en el historial de git
de este archivo.
