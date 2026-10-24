# 7kmod-gameplay: mejoras al espionaje

Checklist para mejorar el espionaje de forma sustancial. Cómo funciona hoy: ver
`SPYING.md`. Los pendientes generales de gameplay están en `TODO.md`.

---

## Checklist

Cosas anotadas para agregar en un parche. `[x]` = implementado, falta probarlo en partida.

- [ ] **Propaganda en pueblos independientes.** En un pueblo independiente, el modo
  "Sow Dissent" se llama **"Propaganda"** (en el juego, en inglés). La idea es que
  los espías **ablanden la resistencia**: una forma de hacerse de un pueblo más
  rápido y sin gastar en donaciones, pero sin poder conquistarlo solos.
  - Hoy: baja la resistencia hacia tu reino de la gente de la raza del espía en
    habilidad ÷ 5 ÷ población de esa raza, cada 30 días (0.1 a 2 puntos al mes).
  - **Efecto directo:** cada 30 días, habilidad ÷ 10 por espía, sin dividir entre la
    población (2 puntos con habilidad 20, 10 con habilidad 100). Las otras razas del
    pueblo bajan a la mitad, para que un pueblo mixto no necesite un espía de cada
    raza. **Máximo 10 puntos al mes por pueblo** entre todos los espías, para que
    llenar un pueblo de espías no lo haga instantáneo.
  - **Sin un fuerte tuyo enlazado** (con general), la propaganda no baja la
    resistencia de **30**: solo ablanda. Sin fuerte, el último empujón tiene que
    venir de atacar el pueblo (las donaciones ya requieren un fuerte enlazado).
  - **Con un fuerte enlazado**, la propaganda puede bajarla hasta 0, y además
    **sube la influencia del fuerte**: habilidad ÷ 5 por espía, **máximo +20**. La
    meta del fuerte (100 − influencia) baja, así que un general de liderazgo medio
    ya no se queda atorado.
  - Caso de uso: un general de liderazgo 60 a mitad de partida (misma raza que el
    rey, reputación 60) tiene influencia 83, así que el fuerte deja la resistencia
    trabada en 17 y sin donaciones nunca se une. Con liderazgo 80 llega a 100 de
    influencia y se une solo. Simulación con resistencia inicial 80–100 (meses de
    juego hasta que el pueblo se une):

    | Caso | General 60 | General 80 |
    |---|---|---|
    | Solo el fuerte (hoy) | nunca | 10–11 |
    | + 1 espía de habilidad 30 | 10–11 | 7–8 |
    | + 1 espía de habilidad 50 | 7–8 | 6–7 |
    | + 2 espías de habilidad 50 | 4–5 | 4–5 |

    Comparado con las donaciones: cada una cuesta $30 por habitante ($900 en un
    pueblo de 30) y quita 10 puntos, cada vez menos. Un espía cuesta $100 al año:
    la propaganda es la opción lenta y barata, las donaciones la rápida y cara.
  - Ojo: el fuerte solo baja la resistencia de la raza de su general (las otras
    razas del pueblo no tienen meta). La propaganda es la que baja las demás.
  - Por confirmar: si la IA debe usar la propaganda (hoy la IA ya pone espías en
    pueblos).

---

## Ideas por confirmar

- **Corregir la exposición al ver secretos** (error del original, ver `SPYING.md`,
  sección 9): pasar el tipo de reporte en vez del número del espía, y usar una
  probabilidad que baje de forma continua con la habilidad de sobra.
