# 7kmod-gameplay: mejoras al espionaje

Checklist para mejorar el espionaje de forma sustancial. Cómo funciona hoy: ver
`SPYING.md`. Los pendientes generales de gameplay están en `TODO.md`.

---

## Checklist

Cosas anotadas para agregar en un parche. `[x]` = implementado, falta probarlo en partida.

- [ ] **Propaganda en pueblos independientes.** En un pueblo independiente, el modo
  "Sow Dissent" se llama **"Propaganda"** (en el juego, en inglés) y es mucho más
  potente.
  - Hoy: baja la resistencia hacia tu reino de la gente de la raza del espía en
    habilidad ÷ 5 ÷ población de esa raza, cada 30 días (0.1 a 2 puntos al mes).
  - Propuesta: **habilidad ÷ 10 por mes, sin dividir entre la población** (2 puntos
    al mes con habilidad 20, 10 con habilidad 100). Igual que las donaciones, que
    quitan una cantidad fija sin importar el tamaño del pueblo.
  - **Con un fuerte tuyo enlazado al pueblo** (con general): el efecto se duplica
    (4 a 20 puntos al mes).
  - **Las otras razas del pueblo** también bajan su resistencia, a la mitad del
    efecto. Así un pueblo mixto no necesita un espía de cada raza.
  - Por confirmar: los números; si el bono del fuerte debe ser ×2 o depender del
    liderazgo del general; si la IA debe usar la propaganda (hoy la IA ya pone
    espías en pueblos).

---

## Ideas por confirmar

- **Corregir la exposición al ver secretos** (error del original, ver `SPYING.md`,
  sección 9): pasar el tipo de reporte en vez del número del espía, y usar una
  probabilidad que baje de forma continua con la habilidad de sobra.
