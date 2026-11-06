# Armas de asedio

[English](SIEGE_WEAPONS.en.md) | **Español**

Referencia de cómo funcionan las armas de asedio en Seven Kingdoms: Ancient
Adversaries 2.15.7 y en 7kmod. Los valores salen de los datos del juego
(`RESOURCE/STD.SET`: tablas UNIT, UNITATTK, SACTION y SPRITE) y del código
(`src/OUNIT.cpp`, `src/OUNITAT.cpp`, `src/OBULLET.cpp`, `src/OB_FLAME.cpp`,
`src/OU_CART.cpp`, `src/OF_CAMP.cpp`, `src/OF_WAR.cpp`). Los daños por segundo
se calcularon con las reglas de `tools/combat_sim.py` (ver `COMBAT_ANALYSIS.md`).

Ninguno de los cambios de 7kmod ni de 7kmod-gameplay modifica las armas de
asedio: todo lo de este documento es del juego original. Lo único que cambia en
gameplay es la armadura de algunos soldados (ver `CHANGELOG-gameplay.md`), que
afecta al daño que reciben.

## 1. Costos

El costo de construcción en la War Factory es igual al costo de mantenimiento
anual (`build_cost = year_cost`).

| Arma | Construir | Mantener / año | Días de construcción |
|---|---|---|---|
| Porcupine | $80 | $80 | 60 |
| Catapult | $100 | $100 | 80 |
| Ballista | $100 | $100 | 80 |
| Spitfire | $110 | $110 | 90 |
| Unicorn | $110 | $110 | 90 |
| Cannon | $120 | $120 | 100 |

- El mantenimiento se cobra cada día: 1/365 del costo anual (un Cannon cuesta
  unos $0.33 al día).
- No cambia con la versión del arma: una Catapult III cuesta lo mismo que una I.
- Se paga igual dentro de un fuerte (`FirmCamp::pay_weapon_expense`) que en el
  mapa (`Unit::pay_expense`).

Otras unidades no humanas, como referencia: caravana $10, barcos Trader y
Transport $50, Caravel $120 y Galleon $150 al año.

### Sin dinero

Si el tesoro está en $0 o menos, cada arma (y cada barco) pierde **1 punto de
vida por día** en lugar de pagar. Cuando llega a 0 se destruye y aparece la
noticia de que se desgastó. Las armas no tienen lealtad, así que no desertan.

Las caravanas son la excepción: siguen a tu servicio aunque no se les pague.

**Fryhtans invocados:** su costo anual es $0, así que con dinero no cuestan
nada, pero sin dinero caen en la misma regla: en el mapa pierden 1 punto de vida
por día y mueren sin noticia. En gameplay regeneran 1 punto cada 10 días, lo que
no lo compensa (unos −0.9 por día). Dentro de un fuerte o de la Sede de Poder no
pierden nada, porque el cobro del fuerte solo revisa las armas.

En gameplay los Fryhtans invocados además comen (de 20 a 50 de comida al año,
ver `CHANGELOG-gameplay.md`). Sin comida pierden otro punto de vida por día, en
el mapa y dentro del fuerte, así que sin dinero ni comida pierden 2 por día.

## 2. Estadísticas

Las armas siempre tienen combate 100, así que su vida es la de los datos y no
sube. La versión (I, II o III) depende de la tecnología investigada en la Tower
of Science al momento de construirla y cambia los ataques, no la vida.

| Arma | Vida | Velocidad | Alcance (I/II/III) | Daño base (I/II/III) | Frames del ataque | Espera (I/II/III) | Radio del proyectil (I/II/III) | Fuego |
|---|---|---|---|---|---|---|---|---|
| Catapult | 60 | 3 | 7 | 12 / 15 / 18 | 16 | 30 | 30 / 46 / 62 | II y III |
| Ballista | 60 | 4 | 7 | 10 (dos ataques alternados) | 6 | 20 / 16 / 12 | 30 | no |
| Cannon | 80 | 3 | 6 / 7 / 8 | 16 / 20 / 24 | 4 | 30 | 30 / 30 / 46 | sí |
| Spitfire | 60 | 5 | 2 | 2 por golpe, ~11 golpes por llamarada | 3 | 25 / 20 / 15 | 30 | sí |
| Unicorn, a distancia | 60 | 4 | 7 | 12 / 14 / 14 | 4 | 6 / 6 / 4 | 30 | no |
| Unicorn, cuerpo a cuerpo | | | 1 | 8 / 8 / 10 | 4 | 4 | — | no |
| Porcupine | 40 | 4 | — | 50 al explotar | — | — | 3×3 | sí |

Como referencia, los soldados humanos tienen 200 de vida con combate 100 (100
con combate 50) y velocidad 4 (Zulú 5 en gameplay).

## 3. Daño en combate

Daño por segundo contra una unidad con armadura 4 (la normal), en versión
I / II / III. El daño de un golpe es (base + random(3)) / 4, menos armadura / 4
(1 punto con armadura 4). Si queda por debajo de 0.25 por encima de la
armadura, el golpe hace como máximo 0.25.

| Arma | Daño/s contra armadura 4 | Daño/s crudo (sin armadura) |
|---|---|---|
| Catapult | 0.8 / 1.1 / 1.4 | 1.2 / 1.5 / 1.8 |
| Ballista | 0.9 / 1.1 / 1.4 | 1.5 / 1.7 / 2.2 |
| Cannon | 1.3 / 1.7 / 2.0 | 1.7 / 2.0 / 2.4 |
| Spitfire | 1.3 / 1.6 / 2.1 | 3.9 / 4.7 / 6.2 |
| Unicorn, a distancia | 3.0 / 3.7 / 4.7 | 4.3 / 5.0 / 6.4 |
| Unicorn, cuerpo a cuerpo | 2.1 / 2.1 / 3.0 | 3.8 / 3.8 / 4.7 |

Soldados como referencia (contra armadura 4):

| Soldado | Combate 50 | Combate 100 |
|---|---|---|
| Vikingo (cuerpo a cuerpo) | 2.2 | 2.6 |
| Maya (cuerpo a cuerpo) | 2.2 | 2.8 |
| Chino (cuerpo a cuerpo) | 2.1 | 3.2 |
| Normando (cuerpo a cuerpo) | 1.2 | 1.7 |
| Persa (a distancia) | 1.1 | 1.5 |
| Egipcio (a distancia) | 0.8 | 1.5 |

**Conclusión:** en versión I las armas pegan como un soldado de combate 50, y
en versión III como uno de combate 100, pero con un tercio o menos de su vida.
Su ventaja es el alcance (7–8 casillas contra las 4–6 de los arqueros) y el
daño en área. La Unicorn es la excepción: pega más que cualquier soldado.

### Daño en área

El radio del proyectil está en píxeles (una casilla mide 32). El daño baja
desde el 100% en el centro hasta el 50% en el borde del radio
(`Bullet::attenuated_damage`).

- **Radio 30:** solo la casilla del objetivo.
- **Radio 46** (Catapult II, Cannon III): también las 4 casillas vecinas
  (arriba, abajo, izquierda y derecha) con un 65% del daño.
- **Radio 62** (Catapult III): las 4 vecinas con un 74% y las 4 diagonales con
  un 64%.

Contra grupos apretados, una Catapult III hace hasta 9 golpes por disparo.

### Spitfire

La llamarada no viaja: aparece en la casilla del objetivo y golpea en cada
frame de su animación (12 frames, unos 11 golpes, `BulletFlame::process_idle`).
Cada golpe hace (2 + random(3)) / 4 ≈ 0.75, y contra armadura 4 se queda en el
mínimo de 0.25, así que contra unidades hace unos 2.75 por llamarada sin
importar su armadura. Su debilidad es el alcance 2, que la obliga a meterse en
el combate con solo 60 de vida.

### Unicorn

Dispara cada 10–12 frames, contra 33–45 de la Catapult y del Cannon, y además
tiene ataque cuerpo a cuerpo cuando el enemigo está pegado. Incluso en versión I
supera a casi cualquier soldado de combate 100. Es la candidata más clara a un
ajuste de balance.

### Porcupine

No ataca: se detona (por orden del jugador o si la golpea un proyectil de su
propio reino). Hace 50 de daño (49 tras la armadura 4) a todo lo que esté en el
3×3 a su alrededor, incluidas las unidades propias, y 50/3 a los edificios.
Mata a cualquier soldado con combate menor a 25 y deja a uno de combate 50 a la
mitad. Además hace explotar a otras Porcupines a 2 casillas o menos, en cadena.

### Fuego

La Catapult II y III, el Cannon, la Spitfire y la Porcupine dejan fuego en el
suelo. Por lo visto en el código, el fuego no daña a las unidades
directamente: quema plantas y hace que las unidades quietas se aparten.

## 4. Escudos contra proyectiles

Algunos soldados se cubren con el escudo cuando ven venir un proyectil y le
quitan **2.5 de daño** a cada uno (10 / 4, `Bullet::hit_target`), antes de
aplicar la armadura:

| Raza | Escudo desde combate |
|---|---|
| Griego | siempre |
| Normando | 30 |
| Mughul | 40 |
| Zulú | 50 |

El escudo solo funciona si el soldado mira hacia el proyectil (±45°) y está
quieto o avanzando hacia él. Un soldado que está atacando no se cubre, salvo el
Zulú, que también se cubre durante la espera entre ataques. Así que los
escudos protegen sobre todo a las unidades que avanzan o esperan, no a las que
ya están peleando cuerpo a cuerpo.

Efecto por golpe contra un soldado con escudo y armadura 4:

| Arma | Golpe normal | Con escudo |
|---|---|---|
| Ballista | 1.75 | 0.25 |
| Unicorn II/III, a distancia | 2.75 | 0.25 |
| Catapult III | 3.75 | 1.25 |
| Cannon III | 5.25 | 2.75 |
| Spitfire (cada golpe de la llamarada) | 0.25 | 0 |

El escudo anula por completo la Spitfire, deja a la Ballista y a la Unicorn a
distancia casi sin daño y solo deja rendir al Cannon y a la Catapult de versión
alta. El ataque cuerpo a cuerpo de la Unicorn no es un proyectil, así que el
escudo no lo detiene. Esto también afecta a los arqueros humanos.

## 5. Contra edificios

Los edificios no tienen armadura: reciben el daño crudo ÷ 3
(`Unit::hit_firm`, "so that it takes longer to destroy a firm"). Esto vale para
todas las unidades, no solo para las armas. Daño por segundo contra edificios,
versión I / II / III:

| Arma | Daño/s a edificios |
|---|---|
| Catapult | 0.40 / 0.49 / 0.59 |
| Ballista | 0.49 / 0.58 / 0.74 |
| Cannon | 0.55 / 0.68 / 0.81 |
| Spitfire | 1.31 / 1.60 / 2.05 |
| Unicorn, a distancia | 1.44 / 1.66 / 2.13 |

Como referencia, un soldado Chino con combate 100 hace unos 1.66 por segundo a
un edificio, un Maya 1.43 y un Vikingo 1.32.

**Conclusión:** la Catapult, la Ballista y el Cannon no destruyen edificios más
rápido que un buen soldado. Su ventaja contra edificios es atacar desde fuera
del alcance de las defensas. La Spitfire y la Unicorn sí derriban edificios
rápido, si llegan vivas.
