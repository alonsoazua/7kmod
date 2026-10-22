# Análisis de combate: soldados y Fryhtans

Cómo funciona el combate entre unidades, con los números de cada raza y de cada
Fryhtan. Las secciones 1 a 6 describen las reglas y datos del **juego original**
(sin el balance del mod) y sirven como referencia para comparar. La sección 8
describe el **balance de razas de `7kmod-gameplay`** y sus resultados.

Las reglas salen del código (`src/OUNITAT.cpp`, `src/OUNITAT2.cpp`,
`src/OSPRITE2.cpp`, `src/OSPRITEA.cpp`) y los números de los datos del juego
(`RESOURCE/STD.SET`, tablas `UNIT`, `UNITATTK` y `SACTION`). Los resultados de
duelos son simulaciones; el simulador está en `7kmod/tools/combat_sim.py` y se puede
volver a correr después de cualquier cambio (ver la última sección).

---

## 1. Reglas del combate

### Vida

- **Vida máxima = vida base × combate ÷ 100.**
- Todos los humanos tienen vida base 200: un soldado con combate 20 tiene 40 de vida,
  con 50 tiene 100 y con 100 tiene 200.
- Los Fryhtans tienen vida base de 150 a 400 (tabla de la sección 2).
- El nivel de combate es lo que más cambia la vida. El daño cambia mucho menos.

### Daño de cada golpe

```
daño = daño básico + azar(0 a 2)
       + daño perforante × azar(0 a combate − nivel del ataque − 1) ÷ (100 − nivel del ataque)
```

1. **Bono del general:** si el general de la unidad está cerca, el daño sube un
   porcentaje igual a su liderazgo (un general con 60 de liderazgo da +60%). También
   aplica a los Fryhtans que salen del fuerte con su general. En `7kmod-gameplay`,
   el general también recibe su propio liderazgo como bono y +1 de armadura si tiene
   al menos 4 de sus soldados a 10 casillas o menos (no aplica al rey). El bono se
   aplica antes de la reducción de la armadura, así que el daño real que llega al
   objetivo sube más que el porcentaje.
2. **Reducción general:** el daño se divide entre 4 (`ATTACK_SLOW_DOWN`), para que las
   batallas duren más.
3. **Armadura:** al resultado se le resta 1. Si el daño dividido es menor que 1.25, el
   golpe quita como máximo 0.25.

En promedio, el perforante aporta la mitad de su valor con combate 100 y casi nada
con combate bajo. Por eso **el combate sube poco el daño**: un Maya hace 8 + 1 + 1.5 =
10.5 por golpe con combate 50 (1.6 de vida al objetivo) y 8 + 1 + 3 = 12 con combate
100 (2 de vida).

### La armadura de los datos no se usa

Cada unidad tiene un valor `ARMOR` en los datos (de 3 a 8), pero **el código no lo
lee**. Siempre usa una armadura fija de 4 para todos (el "−1" de arriba). Así es
desde el juego original, no es un cambio del mod. Consecuencias:

- El Rokken (armadura 8) y el Sauroid (7) no resisten más golpes que un Rattus (4).
- Los Normandos (7) y Griegos (6), que en los datos parecen diseñados como unidades
  resistentes con poco daño, pierden esa ventaja.
- Los Mayas y Persas (3) no sufren su armadura baja.

La sección 4 compara los resultados si la armadura funcionara. `7kmod-gameplay` usa
una armadura propia para algunas razas (sección 8).

### Velocidad de ataque

- **Ciclo de un ataque = cuadros de la animación + retraso del ataque − 1** (si el
  retraso es 0, solo la animación).
- El juego corre a 12 cuadros por segundo en velocidad normal: un ciclo de 12 cuadros
  es un golpe por segundo.
- Los ataques cuerpo a cuerpo golpean en el último cuadro de la animación. Los de
  alcance disparan en el cuadro `OUT_FRAME` y el proyectil tarda un poco en llegar.

### Ataques especiales y poder

- Cada unidad tiene **poder**: máximo combate + 50, recupera 5 por día de juego (cada
  10 cuadros, menos de 1 segundo).
- Algunos ataques necesitan un nivel de combate mínimo y una cantidad de poder, y
  gastan poder al usarse.
- **Elección del ataque:** si el objetivo está al lado, la unidad está obligada a
  usar un ataque cuerpo a cuerpo, el de mayor daño básico que pueda usar. Si está
  lejos, usa el ataque de mayor daño que lo alcance. Si ninguno lo alcanza, camina
  hacia él.
- Los ataques encadenados (columna `NEXT_ATKID`) se van alternando: después de un
  ataque, la unidad pasa al siguiente de la cadena que pueda usar.

### Niveles de combate de los Fryhtans

| Fryhtan | Combate |
|---|---|
| Invocado desde la Sede de Poder (mod) | 50, fijo; no entrena |
| Rey de una guarida | 100 |
| Generales de una guarida | 40 a 69 |
| Soldados de una guarida | de la mitad del combate de su general a casi el mismo (20 a 68) |

Los Fryhtans invocados además regeneran 1 de vida cada 10 días y los cura el Greater
Being persa (ver `CHANGELOG-gameplay.md`).

---

## 2. Datos de cada unidad

Ataques en formato **básico + perforante**. El ciclo está en cuadros (12 = 1 segundo).

### Soldados

| Raza | Vida base | Armadura (no se usa) | Ataques |
|---|---|---|---|
| Normando | 200 | 7 | 5+5, cuerpo a cuerpo, ciclo 7<br>7+3, alcance 6, ciclo 13, desde combate 50 |
| Maya | 200 | 3 | 8+6, cuerpo a cuerpo, ciclo 8 |
| Griego | 200 | 6 | 5+3, cuerpo a cuerpo, ciclo 6<br>4+4, cuerpo a cuerpo, ciclo 3, desde combate 30, poder 10 (gasta 8)<br>4+4, cuerpo a cuerpo, ciclo 3, desde combate 60, poder 25 (gasta 12) |
| Vikingo | 200 | 4 | 6+6, cuerpo a cuerpo, ciclo 9<br>40+0, cuerpo a cuerpo, ciclo 12, desde combate 50, poder 90 (gasta 60) |
| Persa | 200 | 3 | 3+3, cuerpo a cuerpo, ciclo 8<br>6+6, alcance 6, ciclo 11 |
| Chino | 200 | 5 | 6+10, cuerpo a cuerpo, ciclo 7<br>5+3, alcance 6, ciclo 11, desde combate 30 |
| Japonés | 200 | 5 | 5+3, cuerpo a cuerpo, ciclo 7<br>32+0, cuerpo a cuerpo, ciclo 10, desde combate 55, poder 100 (gasta 45) |
| Egipcio | 200 | 4 | 5+4, cuerpo a cuerpo, ciclo 8<br>6+3, alcance 6, ciclo 14, desde combate 40<br>25+0, alcance 6, ciclo 14, desde combate 70, poder 90 (gasta 60) |
| Mughul | 200 | 4 | 5+6, cuerpo a cuerpo, ciclo 8<br>5+5, cuerpo a cuerpo, ciclo 4, desde combate 30, poder 70 (gasta 10)<br>30+0, cuerpo a cuerpo, ciclo 7, desde combate 60, poder 60 (gasta 50) |
| Zulú | 200 | 4 | 4+3, cuerpo a cuerpo, ciclo 9<br>10+8, alcance 7, ciclo 22, desde combate 30 |

El ataque especial del Vikingo necesita 90 de poder y el del Japonés 100, así que en
la práctica empiezan a usarse con combate 50 y 55, y solo cada cierto tiempo.

### Fryhtans

| Fryhtan | Aliado de (mod) | Vida base | Armadura (no se usa) | Ataques |
|---|---|---|---|---|
| Rattus | Japoneses | 150 | 4 | 7+9, cuerpo a cuerpo, ciclo 7 |
| Broosken | Zulúes | 150 | 3 | 15+5, cuerpo a cuerpo, ciclo 22 |
| Deezboanz | Mayas | 200 | 5 | 8+8, cuerpo a cuerpo, ciclo 13 |
| Pfith | Mughules | 250 | 4 | 5+4, cuerpo a cuerpo, ciclo 8<br>8+4, cuerpo a cuerpo, ciclo 11, poder 50 (gasta 40) |
| Karrotten | Egipcios | 270 | 3 | 7+6, cuerpo a cuerpo, ciclo 11 |
| Holgh | Normandos | 280 | 4 | 7+8, cuerpo a cuerpo, ciclo 10<br>10+5, cuerpo a cuerpo, ciclo 14, desde combate 10, poder 50 (gasta 40) |
| Sauroid | Griegos | 300 | 7 | 7+8, cuerpo a cuerpo, ciclo 9 |
| Haubudam | Vikingos | 300 | 4 | 7+6, cuerpo a cuerpo, ciclo 5<br>7+6, cuerpo a cuerpo, ciclo 10 (alternan) |
| Wyrm | Chinos | 330 | 5 | 6+9, cuerpo a cuerpo, ciclo 8<br>7+12, cuerpo a cuerpo, ciclo 12, desde combate 20, poder 50 (gasta 40) |
| Rokken | Persas | 400 | 8 | 8+4, cuerpo a cuerpo, ciclo 12<br>15+0, cuerpo a cuerpo, ciclo 18, desde combate 10, poder 50 (gasta 40) |
| Droog | — (solo salvaje) | 320 | 6 | 6+6, cuerpo a cuerpo, ciclo 10<br>8+8, cuerpo a cuerpo, ciclo 12, desde combate 10, poder 50 (gasta 40) |
| Ick | — (solo salvaje) | 160 | 3 | 3+5, cuerpo a cuerpo, ciclo 9<br>6+4, cuerpo a cuerpo, ciclo 12, desde combate 20, poder 50 (gasta 40) |
| Doink | — (solo salvaje) | 180 | 3 | 5+5, alcance 5, ciclo 15 |

Los ataques especiales de los Fryhtans necesitan 50 de poder y gastan 40. Con
combate 50 tienen 100 de poder máximo y recuperan 5 por día, así que en una pelea
usan el especial al principio y después cada 8 días de juego aproximadamente.

---

## 3. Daño por segundo y vida

Daño por segundo contra un objetivo, sin general, a velocidad normal. "Alcance" es el
daño disparando desde lejos (0 si la raza todavía no tiene ataque a distancia).

### Soldados

| Raza | Combate 20 (40 de vida) | Combate 50 (100 de vida) | Combate 100 (200 de vida) |
|---|---|---|---|
| Normando | 0.86 | 1.19 · alcance 1.09 | 1.70 · alcance 1.36 |
| Maya | **1.94** | **2.24** | 2.80 |
| Griego | 1.00 | 1.21 | 1.74 |
| Vikingo | 1.05 | **2.20** | 2.66 |
| Persa | 0.38 · alcance 0.86 | 0.42 · alcance 1.09 | 0.54 · alcance 1.48 |
| Chino | 1.49 | 2.13 · alcance 0.53 | **3.19** · alcance 0.82 |
| Japonés | 0.84 | 0.99 | 2.20 |
| Egipcio | 0.75 | 0.93 · alcance 0.75 | 1.30 · alcance 1.50 |
| Mughul | 0.80 | 1.24 | 2.47 |
| Zulú | 0.44 | 0.51 · alcance 1.05 | 0.70 · alcance 1.44 |

### Fryhtans

"Fuerza" es vida × daño por segundo: una medida de cuánto daño total hace antes de
morir, útil para comparar unidades con perfiles distintos.

| Fryhtan | Costo (puntos) | Combate 50: vida / daño/s / fuerza | Combate 100: vida / daño/s / fuerza |
|---|---|---|---|
| Rattus | 40 | 75 / **2.43** / 182 | 150 / 3.42 / 513 |
| Broosken | 40 | 75 / 1.73 / 130 | 150 / 1.88 / 282 |
| Deezboanz | 50 | 100 / 1.50 / 150 | 200 / 1.95 / 390 |
| Pfith | 55 | 125 / **1.01** / 127 | 250 / 1.39 / 348 |
| Karrotten | 60 | 135 / 1.37 / 184 | 270 / 1.74 / 471 |
| Holgh | 60 | 140 / 1.65 / 230 | 280 / 2.17 / 608 |
| Sauroid | 65 | 150 / 1.84 / 276 | 300 / 2.49 / 747 |
| Haubudam | 65 | 150 / 1.99 / 298 | 300 / 2.57 / 772 |
| Wyrm | 70 | 165 / 1.69 / 280 | 330 / 2.58 / 851 |
| Rokken | 80 | **200** / 1.54 / 308 | 400 / 1.72 / 688 |
| Droog | — | 160 / 1.27 / 204 | 320 / 1.74 / 558 |
| Ick | — | 80 / 0.51 / 41 | 160 / 0.82 / 132 |

Para comparar, un soldado con combate 50 tiene una fuerza de 224 (Maya), 220
(Vikingo), 121 (Griego) o 119 (Normando).

---

## 4. Soldados contra soldados

Duelos uno contra uno entre soldados del mismo nivel de combate, empezando a 6
casillas para que los arqueros alcancen a disparar. Porcentaje promedio de victorias
contra las otras 9 razas:

| Raza | Combate 20 | Combate 50 | Combate 100 |
|---|---|---|---|
| Maya | 100% | 95% | 86% |
| Vikingo | 76% | 88% | 81% |
| Mughul | 36% | 58% | 67% |
| Chino | 89% | 41% | 56% |
| Japonés | 48% | 31% | 56% |
| Egipcio | 24% | 13% | 39% |
| Persa | 0% | 37% | 37% |
| Griego | 69% | 50% | 37% |
| Zulú | 11% | 24% | 24% |
| Normando | 49% | 65% | 20% |

Victorias de la raza de la fila contra la de la columna, con combate 100:

| | Nor | May | Gri | Vik | Per | Chi | Jap | Egi | Mug | Zul |
|---|---|---|---|---|---|---|---|---|---|---|
| Normando | — | 0 | 62 | 0 | 0 | 100 | 0 | 0 | 0 | 10 |
| Maya | 100 | — | 100 | 71 | 100 | 0 | 100 | 100 | 100 | 100 |
| Griego | 30 | 0 | — | 0 | 100 | 0 | 0 | 100 | 0 | 100 |
| Vikingo | 100 | 32 | 100 | — | 100 | 0 | 100 | 100 | 97 | 100 |
| Persa | 99 | 0 | 0 | 0 | — | 100 | 0 | 41 | 0 | 86 |
| Chino | 0 | 100 | 100 | 100 | 0 | — | 100 | 0 | 100 | 0 |
| Japonés | 100 | 0 | 100 | 0 | 100 | 0 | — | 100 | 0 | 100 |
| Egipcio | 100 | 0 | 0 | 0 | 57 | 100 | 0 | — | 0 | 94 |
| Mughul | 100 | 0 | 100 | 2 | 100 | 0 | 100 | 100 | — | 100 |
| Zulú | 94 | 0 | 0 | 0 | 10 | 100 | 0 | 4 | 0 | — |

**Cómo leerlo:**

- Los duelos son muy de piedra-papel-tijera: casi todos terminan 100 a 0. Los
  promedios dicen más que cada cruce.
- **Mayas y Vikingos son los más fuertes** en todos los niveles. El Maya tiene el
  mejor ataque básico del juego y el Vikingo un especial de 40 desde combate 50.
- **Los Chinos dependen de la distancia:** cuerpo a cuerpo son los más fuertes con
  combate 100, pero desde combate 30 prefieren disparar flechas débiles cuando el
  enemigo no está al lado, y pierden contra otros arqueros.
- **Los Japoneses** son débiles hasta combate 55, cuando se activa su especial.
- **Los Normandos, Egipcios, Griegos y Zulúes** quedan por debajo. Los Normandos y
  Griegos tienen ataques débiles y su armadura alta no cuenta. Los Egipcios tienen
  flechas hasta combate 40 y su ataque fuerte hasta combate 70.
- **Los Persas y Zulúes** son arqueros: en duelos les va mal porque el enemigo llega
  rápido al cuerpo a cuerpo, pero en batallas reales, disparando detrás de una línea
  de soldados, rinden más de lo que muestra esta tabla.

### Si la armadura funcionara

Mismos duelos, pero usando el valor `ARMOR` de cada unidad en lugar del 4 fijo (cada
punto arriba de 4 quita 0.25 más por golpe y cada punto abajo quita 0.25 menos):

| Raza | Armadura | Combate 20 | Combate 50 | Combate 100 | Cambio con 100 |
|---|---|---|---|---|---|
| Normando | 7 | 89% | 80% | 88% | +68 |
| Vikingo | 4 | 44% | 87% | 84% | +3 |
| Japonés | 5 | 56% | 44% | 71% | +15 |
| Chino | 5 | 88% | 73% | 56% | 0 |
| Mughul | 4 | 32% | 33% | 54% | −13 |
| Griego | 6 | 90% | 77% | 46% | +9 |
| Maya | 3 | 67% | 56% | 35% | −51 |
| Egipcio | 4 | 23% | 0% | 33% | −6 |
| Zulú | 4 | 11% | 33% | 22% | −2 |
| Persa | 3 | 0% | 16% | 12% | −25 |

Con armadura, los Normandos pasan a ser de los más fuertes y los Mayas y Persas caen
mucho. Los datos parecen pensados para la armadura, pero activarla tal cual
desequilibra en otra dirección: dejaría a Persas y Egipcios todavía más débiles.

---

## 5. Fryhtans contra soldados

Nivel de combate que necesita un soldado de cada raza para ganar al menos la mitad de
los duelos cuerpo a cuerpo contra un Fryhtan invocado (combate 50), sin generales:

| Fryhtan (costo) | Nor | May | Gri | Vik | Per | Chi | Jap | Egi | Mug | Zul |
|---|---|---|---|---|---|---|---|---|---|---|
| Rattus (40) | 68 | 43 | 68 | 50 | >100 | 46 | 55 | 80 | 60 | >100 |
| Broosken (40) | 55 | 32 | 53 | 50 | >100 | 37 | 55 | 63 | 52 | 97 |
| Deezboanz (50) | 60 | 37 | 60 | 50 | >100 | 40 | 55 | 69 | 57 | >100 |
| Pfith (55) | 53 | 32 | 52 | 49 | >100 | 35 | 55 | 63 | 52 | 94 |
| Karrotten (60) | 68 | 43 | 68 | 50 | >100 | 46 | 55 | 79 | 60 | >100 |
| Holgh (60) | 79 | 52 | 79 | 52 | >100 | 53 | 61 | 92 | 60 | >100 |
| Sauroid (65) | 88 | 60 | 88 | 61 | >100 | 60 | 69 | >100 | 66 | >100 |
| Haubudam (65) | 92 | 63 | 92 | 66 | >100 | 63 | 74 | >100 | 69 | >100 |
| Wyrm (70) | 89 | 60 | 89 | 61 | >100 | 61 | 71 | >100 | 66 | >100 |
| Rokken (80) | 94 | 64 | 94 | 68 | >100 | 64 | 77 | >100 | 71 | >100 |

Persas, Zulúes y Egipcios aparecen tan altos porque esta tabla es cuerpo a cuerpo;
como arqueros pueden dañar al Fryhtan antes de que llegue.

### Los Fryhtans resisten mucho y hacen poco daño

- **La vida crece con el tamaño del Fryhtan y el daño no.** Su vida base va de 150 a
  400, pero su daño por golpe es de 5 a 8, igual que el de los humanos.
- **Ejemplo:** un Rokken con combate 50 tiene el doble de vida que un soldado con
  combate 50, pero pega menos que un Maya. En duelo, el Rokken tarda unos 65 segundos
  en matar a un Maya con combate 50 y el Maya unos 89 en matar al Rokken.
- **Subir su combate no lo arregla:** el combate multiplica la vida pero apenas sube
  el daño. Un Rokken con combate 100 tiene el doble de vida y solo 12% más de daño.
- **Sus especiales casi no cuentan** en peleas largas, por el poder que necesitan.
- **La regeneración** (1 de vida cada 10 días) los hace todavía más resistentes en
  peleas largas.
- **Excepciones:** el Rattus y el Broosken son al revés, con poca vida (75 con combate
  50). El Rattus es el que más daño por segundo hace de todos los Fryhtans.

### Costo de invocación contra fuerza

Los costos actuales se calcularon solo con la vida base. Con la fuerza (vida × daño
por segundo, combate 50):

| Fryhtan | Costo | Fuerza | Fuerza por punto |
|---|---|---|---|
| Rattus | 40 | 182 | 4.6 |
| Haubudam | 65 | 298 | 4.6 |
| Sauroid | 65 | 276 | 4.2 |
| Wyrm | 70 | 280 | 4.0 |
| Rokken | 80 | 308 | 3.9 |
| Holgh | 60 | 230 | 3.8 |
| Broosken | 40 | 130 | 3.3 |
| Karrotten | 60 | 184 | 3.1 |
| Deezboanz | 50 | 150 | 3.0 |
| Pfith | 55 | 127 | **2.3** |

El Pfith es el peor por punto y de los más débiles en total. El Rattus cuesta 40 y
tiene la misma fuerza que el Karrotten, que cuesta 60.

---

## 6. Límites del análisis

- Son duelos uno contra uno, en terreno plano y con vida completa. No incluyen
  generales (salvo que se indique), grupos, formaciones, edificios ni Greater Beings.
- Se supone que los proyectiles siempre aciertan. En el juego, una flecha puede fallar
  si el objetivo se mueve.
- Cada número viene de cientos de duelos con azar, así que puede variar unos ±3 puntos
  entre corridas.
- En batallas reales, los arqueros (Persas, Zulúes, Egipcios, Normandos y Chinos con
  combate alto) rinden más que en estas tablas si los protege una línea cuerpo a
  cuerpo.

## 7. Cómo volver a generar los números

El simulador lee los datos del juego y aplica las mismas reglas que el código:

```
python3 7kmod/tools/combat_sim.py                       # reglas de 7kmod-gameplay, data/RESOURCE/STD.SET
python3 7kmod/tools/combat_sim.py --original            # reglas del juego original (secciones 3 a 5)
python3 7kmod/tools/combat_sim.py ruta/a/STD.SET        # otro archivo de datos
python3 7kmod/tools/combat_sim.py --armor               # usando la armadura de los datos
python3 7kmod/tools/combat_sim.py --seed 5              # otra semilla de azar
```

Imprime las tablas de daño por segundo, duelos entre razas y Fryhtans contra
soldados. Los cambios de balance del mod están al principio del simulador
(`MOD_ARMOR`, `MOD_ATTACK_LEVEL` y `MOD_SPEED`), con los mismos valores que el
código. Si se cambian las reglas en el código, hay que aplicar el mismo cambio ahí
o en la clase `Fighter`.

---

## 8. Balance de razas de 7kmod-gameplay

### Cambios

| Raza | Cambio | Original |
|---|---|---|
| Normando | Armadura 4; 5 desde combate 50 | 4 (fija para todos) |
| Griego | Armadura 4; 5 desde combate 30; 6 desde combate 70 | 4 |
| Maya | Armadura 3 mientras su combate es menor a 50; 4 desde 50 | 4 |
| Chino | Ataque a distancia desde combate 40 | 30 |
| Egipcio | Ataque a distancia desde combate 30 | 40 |
| Japonés | Ataque especial desde combate 50 | 55 |
| Mughul | Segundo ataque especial (30 de daño) desde combate 50 | 60 |
| Zulú | Velocidad de movimiento 5 (1.9 casillas por segundo) | 4 (1.5) |

- **Armadura:** la armadura aplica a todas las unidades de esa raza (soldados,
  generales, rey, civiles y espías). Las demás razas, los Fryhtans, las armas y los
  barcos siguen con 4. Cada punto arriba de 4 quita 0.25 más a cada golpe recibido, y
  un golpe que no supera la armadura + 0.25 quita como máximo 0.25.
- **Ataques:** los niveles se cambian al cargar los datos (`UnitRes::apply_mod_attack_balance()`),
  sin modificar los archivos del juego.
- **Velocidad:** se cambia al cargar los datos de los sprites. La lluvia y la nieve
  la siguen reduciendo igual.

### Efecto de la armadura en cada golpe

Daño promedio de un golpe cuerpo a cuerpo de un atacante con combate 20, después de la
reducción:

| Atacante | Contra armadura 4 | Contra armadura 5 | Contra armadura 6 |
|---|---|---|---|
| Maya | 1.39 | 1.14 | 0.89 |
| Chino | 0.99 | 0.74 | 0.49 |
| Vikingo | 0.89 | 0.64 | 0.39 |
| Normando | 0.62 | 0.37 | 0.25 |

Con combate bajo, los golpes son débiles y la armadura 6 les quita más de la mitad.
Con combate alto los golpes son más fuertes y la armadura pesa menos.

### Soldados contra soldados con el balance del mod

Porcentaje promedio de victorias contra las otras 9 razas (mismo método de la
sección 4), comparado con el original:

| Raza | Combate 20 | Combate 30 | Combate 40 | Combate 50 | Combate 100 |
|---|---|---|---|---|---|
| Maya | 95 (100) | 87 (100) | 88 (100) | 95 (93) | 86 (86) |
| Vikingo | 75 (75) | 66 (76) | 66 (78) | 87 (88) | 81 (81) |
| Mughul | 36 (35) | 53 (55) | 54 (58) | **67** (57) | 65 (67) |
| Griego | 68 (68) | **80** (64) | **78** (62) | 46 (50) | **57** (37) |
| Chino | 95 (89) | **98** (67) | 67 (56) | 39 (40) | 56 (56) |
| Normando | 48 (48) | 48 (49) | 47 (48) | 44 (64) | **46** (20) |
| Japonés | 47 (48) | 29 (31) | 28 (27) | **56** (31) | 42 (56) |
| Persa | 0 (0) | 15 (15) | 32 (32) | 32 (35) | 26 (37) |
| Egipcio | 24 (24) | 5 (24) | 16 (16) | 11 (14) | 28 (38) |
| Zulú | 11 (11) | 19 (19) | 23 (23) | 24 (25) | 14 (23) |

Entre paréntesis, el resultado original. Victorias con combate 100, fila contra
columna:

| | Nor | May | Gri | Vik | Per | Chi | Jap | Egi | Mug | Zul |
|---|---|---|---|---|---|---|---|---|---|---|
| Normando | — | 0 | 0 | 0 | 100 | 100 | 14 | 100 | 0 | 93 |
| Maya | 100 | — | 100 | 70 | 100 | 0 | 100 | 100 | 100 | 100 |
| Griego | 100 | 0 | — | 0 | 100 | 0 | 100 | 100 | 12 | 100 |
| Vikingo | 100 | 26 | 100 | — | 100 | 0 | 100 | 100 | 96 | 100 |
| Persa | 0 | 0 | 0 | 0 | — | 100 | 0 | 44 | 0 | 88 |
| Chino | 0 | 100 | 100 | 100 | 0 | — | 100 | 0 | 100 | 0 |
| Japonés | 82 | 0 | 0 | 0 | 100 | 0 | — | 100 | 0 | 100 |
| Egipcio | 0 | 0 | 0 | 0 | 53 | 100 | 0 | — | 0 | 92 |
| Mughul | 100 | 1 | 88 | 2 | 100 | 0 | 100 | 100 | — | 100 |
| Zulú | 6 | 0 | 0 | 0 | 14 | 100 | 0 | 8 | 0 | — |

**Lectura:**

- **Normandos y Griegos** mejoran mucho con combate 100 (Normandos de 20% a 46%,
  Griegos de 37% a 57%).
- **La armadura sube con el combate:** con combate bajo los golpes son débiles y una
  armadura alta los anula casi por completo. Con armadura 6 desde el principio, los
  Griegos ganaban el 96% al 100% de los duelos con combate 20 a 40. Con la armadura
  escalonada quedan en 68% a 80%: fuertes, pero no imbatibles. Los Normandos no
  cambian hasta combate 50.
- **Mayas:** la armadura 3 les quita la ventaja total que tenían al principio (de
  100% a 78–83%) sin tocarlos desde combate 50.
- **Japoneses y Mughules** mejoran con combate 50 por sus especiales más tempranos
  (Japoneses de 31% a 55%, Mughules de 57% a 67%).
- **Chinos:** con combate 30 ya no cambian su ataque fuerte cuerpo a cuerpo por
  flechas débiles (de 67% a 87%); desde 40 quedan como antes.
- **Egipcios:** en duelos no mejoran. Con flechas desde combate 30 se quedan
  disparando flechas débiles en vez de acercarse a pelear cuerpo a cuerpo, y pierden
  contra otros arqueros. En batallas con una línea cuerpo a cuerpo delante, disparar
  antes sí les ayuda; los duelos no lo reflejan.
- **Zulúes:** la velocidad casi no influye en los duelos, porque pelean a distancia
  y casi no caminan. Les ayuda a alcanzar, huir y llegar antes a la batalla. Bajan con
  combate 100 porque ahora pierden contra los Normandos.
- **Persas, Egipcios y Zulúes** siguen siendo los más débiles en duelos. Son
  arqueros: su valor está en batallas de grupo, que este simulador no mide.

### Fryhtans contra soldados con el balance del mod

Combate que necesita un soldado para ganarle la mitad de los duelos cuerpo a cuerpo a
un Fryhtan invocado (combate 50):

| Fryhtan (costo) | Nor | May | Gri | Vik | Per | Chi | Jap | Egi | Mug | Zul |
|---|---|---|---|---|---|---|---|---|---|---|
| Rattus (40) | 60 | 49 | 50 | 50 | >100 | 46 | 52 | 79 | 50 | >100 |
| Broosken (40) | 52 | 35 | 47 | 50 | >100 | 37 | 50 | 63 | 50 | 97 |
| Deezboanz (50) | 52 | 41 | 46 | 50 | >100 | 40 | 50 | 69 | 50 | >100 |
| Pfith (55) | 43 | 41 | 32 | 49 | >100 | 37 | 50 | 63 | 50 | 94 |
| Karrotten (60) | 58 | 50 | 47 | 50 | >100 | 46 | 52 | 79 | 50 | >100 |
| Holgh (60) | 69 | 52 | 60 | 52 | >100 | 53 | 60 | 92 | 57 | >100 |
| Sauroid (65) | 77 | 60 | 66 | 61 | >100 | 60 | 69 | >100 | 64 | >100 |
| Haubudam (65) | 80 | 63 | 68 | 66 | >100 | 63 | 74 | >100 | 69 | >100 |
| Wyrm (70) | 77 | 60 | 66 | 61 | >100 | 61 | 71 | >100 | 68 | >100 |
| Rokken (80) | 85 | 64 | 75 | 68 | >100 | 64 | 77 | >100 | 71 | >100 |

Esta tabla se calculó con la primera versión del balance (Normandos con armadura 5 y
Griegos con 6 en todos los niveles). Con la armadura escalonada, los valores de
Normandos y Griegos por debajo de combate 50 serían algo más altos; los de 50 o más no
cambian. Los Normandos y Griegos necesitan unos 10 a 25 puntos menos de combate que antes;
los Japoneses y Mughules, unos 5. Los Mayas con combate bajo necesitan un poco más
por su armadura 3.

### Fryhtans invocados con el balance del mod

La tabla anterior (Fryhtans contra soldados) es sin estos cambios: solo incluye el
balance de razas. Cambios de los Fryhtans invocados:

- **+25% de daño** para los Fryhtans invocados (los de un reino); los salvajes no
  cambian. Se aplica antes del bono del general, en `Unit::actual_damage()`.
- **Los Mughules invocan al Doink** en lugar del Pfith. Es el único Fryhtan con ataque
  a distancia; su alcance sube de 5 a 6, igual que los arqueros humanos.
- **Broosken:** velocidad 4 (antes 3), la normal.
- **Costos** según la fuerza (vida × daño por segundo con combate 50 y el +25%), de 40
  a 80 redondeado a 5, con −5 para los lentos (velocidad 3). El Haubudam queda en 75 y
  el Doink, un arquero débil, en 30.

| Raza | Fryhtan | Costo original → mod | Velocidad | Vida (combate 50) | Daño/s con +25% |
|---|---|---|---|---|---|
| Mughul | Pfith → **Doink** | 55 → 30 | 5 | 90 | ~1.2 (alcance 6, +1 de daño base) |
| Zulú | Broosken | 40 → 40 | 3 → 4 | 75 | 2.26 |
| Maya | Deezboanz | 50 → 45 | 4 | 100 | 1.99 |
| Japonés | Rattus | 40 → 55 | 5 | 75 | 3.32 |
| Egipcio | Karrotten | 60 → 50 | 4 | 135 | 1.88 |
| Normando | Holgh | 60 → 60 | 3 | 140 | 2.20 |
| Griego | Sauroid | 65 → 80 | 5 | 150 | 2.48 |
| Chino | Wyrm | 70 → 75 | 4 | 165 | 2.36 |
| Vikingo | Haubudam | 65 → 75 | 4 | 150 | 2.75 |
| Persa | Rokken | 80 → 90 (armadura 5) | 3 | 200 | 2.06 |

- El daño por segundo con +25% viene de la misma simulación de la sección 3, con el
  bono aplicado. El del Doink está calculado a mano desde sus datos.
- **El bono rinde más de 25% en daño por segundo** (de 30% a 45%), porque se aplica
  antes de la reducción fija de la armadura. Los que tienen daño básico bajo, como el
  Rattus, son los que más ganan; por eso su costo sube más.
- Combate que necesita un Maya para ganarle la mitad de los duelos (simulación
  anterior): Rattus 49 → 55, Holgh 52 → 66, Rokken 64 → 80.
- **Doink:** con +25% hace un daño parecido al de un arquero persa o zulú con combate
  50, con menos vida. Su papel es apoyo barato y rápido desde atrás de la línea.

**Ajuste con duelos:** después se simularon duelos de cada Fryhtan invocado contra
Griegos y Mayas de combate 30 a 100, con y sin general. Combate más alto de soldado
que cada uno vence sin generales:

| Nivel | Fryhtans | Costo | Vence Griegos hasta | Vence Mayas hasta |
|---|---|---|---|---|
| Fuerte | Haubudam, Wyrm | 75 | 80 | 70 |
| Fuerte | Sauroid (rápido) | 80 | 80 | 70 |
| Tanque | Rokken (armadura 5) | 90 | 100 | 80 |
| Medio-alto | Holgh | 60 | 70 | 60 |
| Medio | Rattus | 55 | 60 | 50 |
| Medio | Karrotten | 50 | 60 | 50 |
| Medio-bajo | Deezboanz | 45 | 50–60 | 40 |
| Bajo | Broosken | 40 | 50 | 40 |
| Apoyo | Doink | 30 | 30 | — |

- Con un general de 60 de liderazgo a favor del soldado, casi todos pierden desde
  combate 40–50; el Rokken con armadura 5 aguanta hasta 60 contra Griegos y 50 contra
  Mayas. El general es el factor que más decide estos duelos.
- El Doink no está pensado para duelos: su valor es disparar detrás de una línea.

Los números de las secciones 3 y 5 son del juego original; para recalcularlos con
estos cambios se puede correr el simulador sin `--original`.
