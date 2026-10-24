# Espionaje

Referencia de cómo funciona el espionaje en Seven Kingdoms: Ancient Adversaries
2.15.7. Los valores salen del código (`src/OSPY.cpp`, `src/OSPY2.cpp`,
`src/OSPYA.cpp`, `src/OFIRMIF3.cpp`, `src/OUNIT.cpp`, `src/OSYS2.cpp`,
`src/ONATIONB.cpp`).

Ninguno de los cambios de 7kmod, 7kmod-gameplay ni 7kmod-pathing modifica el
espionaje: todo lo de este documento es del juego original.

## 1. Cómo se consigue un espía

- **Entrenarlo en un pueblo**, como cualquier otra habilidad. Sale con habilidad
  de espionaje 20 (el nivel de cualquier entrenamiento, `TRAIN_SKILL_LEVEL`).
- **Sobornar a una unidad enemiga** desde un espía que ya está dentro de su
  edificio (ver la sección 5). La unidad sobornada se vuelve tu espía con
  habilidad 10.

Cada espía tiene:

- **Habilidad (0–100):** sube 1 punto cada 40 días mientras está activo, o cada
  80 días si está dormido.
- **Lealtad** hacia tu reino.
- **Disfraz:** el color del reino que aparenta ser.

## 2. Costo

- **$100 al año** siempre, esté donde esté (`SPY_YEAR_SALARY`).
- **Comida:** si está en el mapa o en tus edificios, come de tu reserva. Si está
  infiltrado, come y cobra sueldo del reino enemigo; tú solo pagas los $100.
- Sin dinero pierde 1 punto de lealtad por día. Sin comida (cuando come de la
  tuya) pierde 1 punto cada 5 días.

## 3. Disfraz e infiltración

- En el mapa, un espía puede cambiar su color al de otro reino, pero **solo si no
  hay unidades enemigas a 5 casillas o menos** (`SPY_ENEMY_RANGE`). Volver a su
  propio color siempre se puede.
- Disfrazado, puede entrar a pueblos y edificios de ese reino como si fuera suyo.
  Dentro ya no puede cambiar de disfraz hasta que sale.
- Un espía en un pueblo o edificio te da visión de ese lugar aunque haya niebla
  de guerra.

## 4. Modos dentro de un pueblo o edificio

Se cambian con el botón del espía. El efecto se aplica cada 30 días.

| Modo | Dónde | Qué hace |
|---|---|---|
| **Dormido** (Idle) | todos | Nada. No lo pueden atrapar (ver la sección 6). |
| **Sembrar discordia** | pueblos | Baja la lealtad de la gente de su misma raza: habilidad ÷ 5 ÷ población de esa raza, cada mes. En un pueblo independiente baja su resistencia a tu reino. |
| **Sembrar discordia** | edificios | A veces quita 1 de lealtad al general si es de su misma raza (es menos probable con generales de mucho liderazgo) y a los trabajadores de su raza. |
| **Sabotaje** | edificios, salvo fuertes | Baja la productividad del edificio: habilidad ÷ 5 por espía, hasta −100. |

Con varios espías en el mismo lugar, los efectos se suman.

## 5. Acciones especiales

### Soborno

Un espía puede sobornar a un trabajador o al general del edificio donde está.
Pagas lo que ofrezcas (hasta $4,000) aunque el soborno falle. La probabilidad es:

> habilidad del espía − lealtad del objetivo − poder de mando (si es general)
> \+ tu reputación + 200 × monto ÷ 4000
> \+ 10 si el espía es de su raza + 10 si tu rey es de su raza

Además se resta lo que la lealtad del objetivo pase de 60, de 70, de 80 y de 90.
Con lealtad 100 es imposible. Sobornar a un espía enemigo siempre falla. Si el
soborno funciona, la unidad se vuelve tu espía con habilidad 10 y lealtad entre
30 y 100 (la probabilidad que tenía el soborno).

### Asesinato

Un espía puede intentar matar al general o al rey del edificio donde está.

- **Ataque:** habilidad + vida del espía ÷ 2, más habilidad ÷ 4 de cada uno de tus
  otros espías en el edificio.
- **Defensa:** vida del objetivo ÷ 2, +50 si es el rey, + la habilidad del
  objetivo si también es espía, + 4 + vida ÷ 30 por cada trabajador normal, +
  habilidad ÷ 2 por cada espía del reino dueño del edificio.

Si el ataque es mayor o igual, el objetivo muere. Si además hay otros defensores y
la diferencia es menor a 80, tu espía también muere. Si el ataque es menor, tu
espía muere.

### Capturar un edificio

- Si tu espía es el **general** de un fuerte enemigo, puede tomar el fuerte. Los
  soldados que no lo obedecen salen y lo atacan. Cada soldado obedece con una
  probabilidad de liderazgo ÷ 2 + tu reputación ÷ 2, +50 si es de la raza del
  espía. Los que obedecen quedan con lealtad de al menos 30.
- En edificios sin general (minas, fábricas, etc.) solo se puede capturar si
  **todos** los trabajadores son tus espías.

### Ver los secretos del reino enemigo

Un espía dentro de un pueblo o edificio enemigo puede abrir los reportes de ese
reino. Cada uno pide una habilidad mínima:

| Reporte | Habilidad mínima |
|---|---|
| Pueblos | 20 |
| Economía | 30 |
| Comercio | 30 |
| Reino | 40 |
| Tecnología | 40 |
| Militar | 50 |
| Espías | 90 |

Al cerrar el reporte el espía puede quedar **expuesto**, y entonces muere al día
siguiente (ver la sección 6). Ver el error del juego original en la sección 9.

## 6. Atrapar espías

- Cada 30 días, cada pueblo y cada edificio intenta atrapar **un** espía enemigo
  que no esté dormido. La probabilidad es de 1 entre (100 + habilidad del espía −
  suma de la habilidad de tus espías en ese lugar), con un mínimo de 1 entre
  (habilidad ÷ 10). Un espía dormido nunca es atrapado.
- Si todos los habitantes del lugar son espías enemigos, no se atrapa a nadie.
- Por eso, poner tus propios espías en tus pueblos y edificios funciona como
  contraespionaje: entre más habilidad sumen, más fácil atrapan a los enemigos.
- Un espía **expuesto** muere al día siguiente, salvo que esté con el color de su
  propio reino.

## 7. Lealtad y traición

- Cada 60 días la lealtad de un espía se acerca 1 punto a: 50 + reputación ÷ 4 +
  calificación de tu reino ÷ 4, +20 si es de la raza de tu rey (máximo 100).
- Puedes premiarlo: $30, +10 de lealtad.
- Si su lealtad baja de 30 y el reino que finge servir le atrae más (su reputación,
  +30 si es de su raza), **deja de ser espía** y se queda con ese reino.

## 8. Un espía que hereda el trono

Cuando muere el rey de un reino de la IA y el sucesor es tu espía:

- Si su lealtad es al menos 90 + poder de ese reino − poder del tuyo, **te entrega
  el reino completo** (el reino se rinde ante ti).
- Si no, te traiciona y se queda como rey.
- Nunca pasa con un reino humano. Si el espía es tuyo y hereda tu propio trono,
  simplemente deja de ser espía.

## 9. Error del juego original: exposición al ver secretos

Al cerrar un reporte secreto, el juego decide si el espía queda expuesto
(`Sys::set_view_mode`, `src/OSYS2.cpp`):

```cpp
int needViewSecretSkill = spy_array.needed_view_secret_skill(info.viewing_spy_recno);
int escapeChance = spyPtr->spy_skill - needViewSecretSkill;

if( escapeChance > 0 )
{
	if( misc2.random( escapeChance/15 )==0 )
		killFlag = 1;
}
```

**El error:** `needed_view_secret_skill()` espera el tipo de reporte (Pueblos,
Militar...), pero recibe el número interno del espía (`viewing_spy_recno`). Los
tipos de reporte son los números 1 a 7, así que para casi todos los espías
devuelve 0 y la habilidad mínima del reporte no se descuenta (para los espías con
número 1 a 7 descuenta la de un reporte cualquiera, no la del que vio). La
probabilidad de quedar expuesto depende solo de la habilidad del espía, sea cual
sea el reporte:

| Habilidad | escapeChance ÷ 15 | Queda expuesto |
|---|---|---|
| 20–29 | 1 | siempre |
| 30–44 | 2 | 1 de cada 2 |
| 45–59 | 3 | 1 de cada 3 |
| 60–74 | 4 | 1 de cada 4 |
| 75–89 | 5 | 1 de cada 5 |
| 90–100 | 6 | 1 de cada 6 |

Un espía recién entrenado (habilidad 20) **siempre** queda expuesto y muere al
día siguiente si ve cualquier reporte, aunque sea el más fácil (Pueblos, que pide
20).

**Aun corrigiendo el parámetro**, la fórmula tiene un borde raro: un espía con la
habilidad justa para el reporte (escapeChance = 0) nunca queda expuesto, mientras
que uno con 1 a 14 puntos de sobra (escapeChance ÷ 15 = 0, y `random(0)` siempre
da 0) queda expuesto siempre. Por ejemplo, para el reporte Militar (50), un espía
con 50 nunca sería atrapado y uno con 60 siempre.

En el código hay además una versión anterior comentada (`src/OINFO.cpp`) en la
que el espía se exponía en promedio en 5 a 15 días mientras veía los secretos.

Posibles correcciones, si se decide hacerlo:

- Pasar el tipo de reporte en lugar del número del espía.
- Usar una probabilidad que baje de forma continua con la habilidad de sobra, por
  ejemplo 1 entre (2 + habilidad de sobra ÷ 15), para que más habilidad siempre
  sea mejor.
