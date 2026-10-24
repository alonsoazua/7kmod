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
  - **La IA:** ya usa este modo en pueblos independientes (`Spy::think_town_spy`), así
    que aprovecha la propaganda sin cambios. Ajuste: hoy decide sacar al espía según
    la lealtad del pueblo, pero un pueblo independiente tiene resistencia, no
    lealtad (descuido del original). Cambiarlo para que el espía salga cuando la
    resistencia de su raza llegue a 30 y la IA no tenga un fuerte enlazado.

- [ ] **Aldeanos que se unen por la propaganda.** Cada 30 días, un pueblo
  independiente con espías tuyos en Propaganda tiene una probabilidad de que **un**
  aldeano lo deje y aparezca a su lado como unidad tuya. **Máximo 1 por pueblo al
  mes** (por reino).
  - La idea es **conseguir aldeanos de razas que no tienes**, así que no depende de
    la resistencia ni de la raza del espía.
  - Probabilidad al mes = suma de la habilidad de tus espías en Propaganda ÷ 10 ×
    población del pueblo ÷ 30 %, **máximo 30%**. Más espías y más habilidad lo hacen
    más probable, y un pueblo grande más que uno chico.
  - El aldeano es de cualquier raza del pueblo, al azar según cuántos habitantes
    tiene cada una. El pueblo nunca se queda sin habitantes (los espías no
    cuentan).
  - Calidad: combate de los defensores del pueblo ±10 (10 a 70 según el pueblo).
    Habilidad de trabajo o liderazgo (al azar entre construcción, minería,
    fabricación, investigación o liderazgo): 10 + al azar de 0 a la suma de la
    habilidad de los espías ÷ 2, **máximo 70**. Lealtad inicial: 60.
  - Estimaciones, probabilidad al mes (entre paréntesis, aldeanos al año en
    promedio):

    | Espías | Calidad hasta | Pueblo 10 | Pueblo 30 | Pueblo 60 |
    |---|---|---|---|---|
    | 1 de habilidad 50 | 35 | 2% (0.2) | 5% (0.6) | 10% (1.2) |
    | 2 de habilidad 50 | 60 | 3% (0.4) | 10% (1.2) | 20% (2.4) |
    | 3 de habilidad 50 | 70 | 5% (0.6) | 15% (1.8) | 30% (3.6) |
    | 2 de habilidad 100 | 70 | 7% (0.8) | 20% (2.4) | 30% (3.6) |

  - Por confirmar: los números.

---

## Ideas por confirmar

- **Corregir la exposición al ver secretos** (error del original, ver `SPYING.md`,
  sección 9): pasar el tipo de reporte en vez del número del espía, y usar una
  probabilidad que baje de forma continua con la habilidad de sobra.
