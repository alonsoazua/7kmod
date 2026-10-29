# Greater Beings (Seres Mayores)

Cómo funcionan los Greater Beings de Seven Kingdoms: Ancient Adversaries 2.15.7, según el
código fuente (`OU_GOD.cpp`, `OGODRES.cpp`, `OF_BASE.cpp`, `OU_MONS.cpp`, `OSITE.cpp`) y los
datos del juego (tablas `GOD`, `UNIT` y `UNITATTK` de `RESOURCE/STD.SET`).

**Cambios del mod:**
- **7kmod:** la opción "Seats of Power Unlocked?" (cómo se obtiene el pergamino al
  inicio) y la ayuda de los botones de poder del Djini, Isis y uNkulunkulu, que no la
  tenían.
- **7kmod-gameplay:** el Dragon normando lanza una bola de fuego con un botón en vez de
  atacar; el Jing Nung chino ataca a la mitad de costo; el Djini mughul además convierte
  a un soldado; el Phoenix griego cuesta menos y tiene un poder para exponer espías. Están
  marcados con *(gameplay)* abajo. En `7kmod` los cuatro funcionan como el original.

---

## 1. Cómo se obtienen

### El pergamino (Scroll of Power)
- Cada raza tiene un Greater Being. Para invocarlo, tu reino necesita el **pergamino de esa raza**, que permite construir su **Seat of Power** (Sede de Poder).
- **Al matar a un rey Fryhtan** (el monstruo de rango rey) queda un pergamino en el suelo cerca de donde murió.
  - La raza del pergamino es la de las unidades más numerosas en un radio de 10 casillas alrededor del rey. Si no hay unidades con raza cerca (por ejemplo, solo armas de asedio), la raza es aleatoria.
  - Solo puede recogerlo una unidad **de la misma raza que el pergamino**. Al recogerlo, su reino aprende a construir esa Sede de Poder y sale una noticia.
  - Matar a un general Fryhtan deja oro, no pergaminos.
- **Al inicio de la partida:**
  - Con la opción de 7kmod **Seats of Power Unlocked?** = Yes, el reino del jugador empieza con el pergamino de su raza (solo en partidas de un jugador).
  - El juego original también tiene el ajuste avanzado `nation_start_god_level` en `config.txt`: 0 = ninguno (por defecto), 1 = cada reino empieza con el pergamino de su raza, 2 = todos los pergaminos. Este ajuste aplica a **todos** los reinos, incluida la IA. No se sincroniza en multijugador: todos los jugadores deben tener el mismo valor o la partida puede desincronizarse. Ver `ADVANCED_CONFIG.md`.

### La Sede de Poder (Seat of Power)
- Solo acepta un supervisor y trabajadores (sacerdotes) **de la raza de la Sede**.
- **Puntos de oración:**
  - Se acumulan cada día, solo si hay supervisor: **productividad / 100** por día. Con "fast build" es productividad / 10.
  - La productividad es la suma de la habilidad de los trabajadores, ajustada por su salud, dividida entre 8. Con 8 sacerdotes de habilidad 100 al máximo de salud es 100, o sea **1 punto por día**.
  - El supervisor sube poco a poco la habilidad de los trabajadores, como en el fuerte.
  - Máximo: **400 puntos**.
- **Invocar (Invoke):**
  - Requiere al menos **40 puntos** (10% del máximo) y que la Sede no tenga ya un ser invocado. **Cada Sede mantiene a un solo ser a la vez**; para tener varios necesitas varias Sedes.

### Reglas comunes de todos los seres
- **Su vida son los puntos de oración de su Sede.** Si la Sede pierde puntos (por existir, usar poderes o atacar), el ser pierde vida.
- **El daño no se acumula.** Recibir daño no resta puntos a la Sede: en cada cuadro del juego la vida del ser vuelve a igualarse a los puntos de la Sede. Solo muere por daño si en un mismo instante recibe tanto daño como puntos tiene. En la práctica es casi invulnerable mientras tiene muchos puntos y frágil cuando le quedan pocos (por ejemplo, justo después de usar su poder).
- **Existir cuesta puntos cada día.** El costo depende del ser (ver tabla). Si los puntos llegan a 0, el ser muere.
- **Si la Sede es destruida, el ser muere al instante.** Si la Sede cambia de dueño, el ser también.
- **Todos vuelan** y tienen armadura 8.
- **Poderes:** cada uso cuesta los puntos indicados en la tabla, y el botón solo se activa si el ser tiene al menos esa cantidad de puntos (vida).
- **Ataque:** solo los dos dragones atacan. Los demás no pueden atacar y solo se mueven o usan su poder. *(gameplay)*: solo el Jing Nung chino ataca; el Dragon normando usa su bola de fuego con un botón.

---

## 2. Resumen

| Raza | Ser | Tipo | Costo por día | Costo por uso | Alcance del poder | Visión |
|---|---|---|---|---|---|---|
| Normanda | Dragon | Ataque (fuego). *(gameplay)*: bola de fuego con botón | 1 | 10 por ataque. *(gameplay)*: 100 por bola | — *(gameplay: área de radio 4)* | 9 |
| China | Jing Nung | Ataque (3 ataques) | 1.5 | 10 por ataque. *(gameplay)*: 5 | — | 9 |
| Griega | Phoenix | Explorador, revela información. *(gameplay)*: poder de exponer espías | 2.5. *(gameplay)*: 1.5 | — *(gameplay: 100)* | — *(gameplay: un pueblo propio)* | 10 |
| Maya | Kukulcan | Poder: más combate | 1.5 | 100 | Área (radio 5) | 9 |
| Persa | Lord of Healing | Poder: curar | 1.5 | 100 | Área (radio 5) | 9 |
| Japonesa | Mind Turner | Poder: bajar lealtad | 1.5 | 100 | Área (radio 5) | 9 |
| India | Djini | Poder: bajar lealtad. *(gameplay)*: y convertir a un soldado | 1.5 | 100 | Área (radio 5) | 10 |
| Vikinga | Thor | Poderes: lluvia y tornado | 1.5 | 100 | Mapa / frente a Thor | 9 |
| Egipcia | Isis | Poder: más población | 1.5 | 100 | Una casilla (pueblo) | 9 |
| Zulú | uNkulunkulu | Poder: más liderazgo | 1.5 | 100 | Una casilla | 9 |

**Notas sobre la tabla:**
- El "costo por día" sale de la tabla `GOD` (puntos de existencia ÷ 20).
- Con 400 puntos y sin oración, un ser de costo 1.5 dura unos 266 días; el Phoenix, unos 160. Una Sede con buena productividad (hasta 1 punto por día) compensa parte del gasto.
- **Área de radio 5:** una zona de unas 9×9 casillas alrededor del punto elegido, recortada en forma de rombo achatado (las filas de arriba y abajo son más cortas).
- **Efecto sobre edificios:** el efecto llega al supervisor y a los trabajadores que viven dentro (no a los de edificios cuyos trabajadores viven en pueblos). Se reparte entre las casillas que ocupa el edificio, así que un edificio completo dentro del área recibe aproximadamente el efecto entero; uno parcialmente dentro, solo una parte.
- **Barcos:** las unidades dentro de tus barcos reciben el efecto de los poderes amistosos, dividido por el tamaño del barco.

---

## 3. Detalle por ser

### Normandos — Dragon
- **Qué hace:** ataca con fuego a distancia.
- **Ataque:** daño base 15, alcance 6 casillas, retraso entre ataques 6.
- **Costo:** cada ataque gasta 10 puntos de oración, que también son su vida; atacar mucho lo debilita.
- **Uso:** la unidad de combate directa más fuerte al inicio, pero gasta rápido sus puntos.

- *(gameplay)* **Bola de fuego con botón:** ya no ataca. Su botón de poder (el ícono es su
  bola de fuego) deja elegir un punto en el mapa; vuela hasta él y lanza la bola.
  - **Costo:** 100 puntos por bola.
  - **Daño base 150** (su ataque tenía 15), en un radio de 109 pixeles (unas 3.4 casillas;
    antes 45) que baja del 100% al centro al 50% en la orilla: contra armadura 4, unos 37
    de vida al centro, 31 a una casilla, 26 a dos y 20 a tres. Solo daña a reinos con los
    que estás en guerra. Los edificios reciben el daño ÷3, una vez cada uno.
  - **Fuego** en las casillas a 5 o menos del centro (antes 3).
  - La IA la lanza cuando hay 5 o más unidades enemigas juntas.

### Chinos — Jing Nung (dragón chino)
- **Qué hace:** ataca a distancia con una secuencia de 3 ataques.
- **Ataque:** tres ataques de daño base 18 cada uno, alcance 6.
- **Costo:** 10 puntos por cada ataque.
- **Uso:** más daño por secuencia que el Dragon normando, al mismo costo por ataque.

- *(gameplay)* **Costo:** 5 puntos por ataque: con los mismos puntos ataca el doble.

### Griegos — Phoenix (Fénix)
- **Qué hace:** no ataca ni tiene poderes. Es un explorador volador con la **mayor visión (10)** de todos los seres.
- **Revela información:** todo lo que está a 10 casillas o menos del Fénix muestra su información completa al seleccionarlo, como si tuvieras un espía dentro:
  - **Unidades enemigas:** su panel de información (combate, habilidad, lealtad) y su profesión real.
  - **Edificios enemigos:** el panel interior, con la lista de trabajadores y sus niveles, la producción, la investigación, etc.
- **No detecta espías.** El código original tenía una función para marcar a los espías enemigos con habilidad menor a 40 dentro de tus edificios cuando el Fénix volaba sobre ellos, pero está desactivada (comentada) en el código.
- **Costo:** es el que más cuesta mantener (2.5 por día). *(gameplay)*: 1.5 por día.
- *(gameplay)* **Poder: exponer espías.** Botón de poder (el ícono del ojo de los espías):
  se elige un **pueblo de tu reino** y el Fénix vuela hasta él. Si hay espías de otros
  reinos, **expone y ejecuta** al de menor habilidad de espionaje, con la noticia normal.
  - **Costo:** 100 puntos, aunque no haya espías. Sobre algo que no es un pueblo tuyo no
    hace nada y no gasta puntos.
  - **Efecto:** un anillo de fuego sobre el pueblo (el del Mind Turner, en naranja), con
    el sonido del poder de Isis.
  - La IA lo usa sobre sus pueblos con espías ajenos (el juego sabe dónde están).
- **Uso:** explorar el mapa y espiar el interior de los edificios enemigos sin arriesgar espías; la IA lo manda a volar a puntos aleatorios del mapa.

### Mayas — Kukulcan
- **Poder (botón de aumento de combate):** las unidades **mayas de tu reino** en el área ganan de **+15 a +24 de combate** al instante, con un máximo de 100.
  - Funciona en unidades en el mapa y en trabajadores mayas de tus edificios.
  - La vida actual de las unidades no cambia, aunque suba su combate.
- **No afecta** a unidades de otras razas, aunque sean tuyas.
- **Uso:** mejorar de forma permanente a un grupo de soldados mayas antes de una batalla.

### Persas — Lord of Healing (Señor de la Curación)
- **Poder (curar):** cura a las **unidades humanas de tu reino** en el área, de cualquier raza.
  - En el mapa: recuperan entre 1/9 y 1/6 de su vida máxima (mínimo 10).
  - Dentro de edificios: los trabajadores recuperan entre 1/7 y 1/4 de su vida máxima.
- **No cura** armas de asedio, barcos ni unidades de otros reinos.
- **Uso:** recuperar un ejército herido sin esperar a que sane en el fuerte.

### Japoneses — Mind Turner (Manipulador de Mentes)
- **Poder:** baja la lealtad de todo lo que **no es de tu reino** en el área:
  - Unidades y trabajadores con raza: **−7 a −15 de lealtad**.
  - Pueblos de otros reinos: baja la lealtad de cada raza unos 7–15 en total si el pueblo está completo dentro del área.
  - Pueblos independientes: baja su resistencia hacia tu reino en la misma medida, lo que ayuda a conquistarlos.
- **Ojo:** el código no distingue entre enemigos y aliados; afecta a cualquier reino que no sea el tuyo, incluidos los aliados.
- **Uso:** debilitar la lealtad de un ejército o de un pueblo enemigo; con lealtad muy baja las unidades pueden desertar o rebelarse, como en el juego normal.

### Indios — Djini
- **Poder:** baja **−20 a −30 de lealtad** a las unidades visibles en el área que pertenezcan a reinos con los que estás **en guerra** o a nadie (independientes).
- **No afecta** a trabajadores dentro de edificios, a pueblos, ni a reinos con los que no estás en guerra.
- **Uso:** más fuerte que el japonés contra ejércitos en el campo, pero solo funciona contra enemigos declarados.

- *(gameplay)* **Además convierte a un soldado:** en cada lanzamiento, el soldado con menos
  combate del área (rango soldado, no general ni rey; solo personas, sin espías) entre las
  unidades a las que baja la lealtad pasa a tu reino, como una unidad que cambia de bando
  (con su noticia y su lealtad). Empates: el de menor lealtad.

### Vikingos — Thor
Tiene **dos poderes**, cada uno de 100 puntos:
- **Lluvia (Rain):** se activa al instante, sin elegir objetivo.
  - Lluvia fuerte durante 10 días y tormenta eléctrica durante 7 días.
  - La lluvia apaga incendios.
  - Los rayos caen en **puntos al azar de todo el mapa** y dañan a quien esté ahí (unidades, edificios y pueblos de cualquier reino, incluido el tuyo), y pueden provocar incendios.
- **Tornado:** aparece un tornado justo enfrente de Thor, en la dirección a la que mira.
  - Dura 60 días (600 cuadros) y el viento sopla 10 días en esa dirección, así que el tornado avanza hacia allá.
  - Daña a las unidades por las que pasa, de cualquier reino; a las unidades voladoras les hace el doble de daño.
- **Uso:** el tornado sirve contra ejércitos agrupados si orientas a Thor hacia ellos; la lluvia es más útil para apagar incendios que como ataque, porque los rayos son aleatorios.

### Egipcios — Isis
- **Poder:** se lanza sobre **un pueblo de tu reino** (alcance de una sola casilla) y le agrega **hasta 5 habitantes** de las razas que ya viven ahí, sin pasar el límite de crecimiento de 60.
  - Los nuevos habitantes llegan con la lealtad actual de su raza en ese pueblo.
- **No hace nada** sobre unidades, edificios ni pueblos ajenos.
- **Uso:** acelerar el crecimiento de un pueblo, por ejemplo para tener más gente que reclutar o que pague impuestos.

### Zulúes — uNkulunkulu
- **Poder:** sobre una sola casilla, da **+30 de habilidad (liderazgo)** a una unidad **zulú de tu reino que sea general o rey**, con un máximo de 100.
  - Si se lanza sobre un edificio, lo recibe el supervisor si es un general zulú.
  - No afecta a soldados ni a trabajadores.
- **Uso:** convertir rápido a un general zulú en un gran líder, lo que mejora el entrenamiento de sus soldados y su lealtad.

---

## 4. Cómo usa la IA los Greater Beings

- La IA construye Sedes de Poder e invoca a sus seres cuando tiene el pergamino.
- **Dragones:** buscan objetivos enemigos para atacar. *(gameplay)*: el Jing Nung; el Dragon lanza su bola de fuego a grupos de 5 o más enemigos.
- **Thor:** lanza el tornado contra objetivos enemigos.
- **Lord of Healing:** busca el fuerte propio con más heridos y lo cura.
- **Phoenix:** vuela a puntos aleatorios. *(gameplay)*: antes expone a los espías de sus pueblos si tiene los puntos.
- **Los demás seres:** buscan objetivos según su poder.
