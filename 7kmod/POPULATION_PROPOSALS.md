# Propuestas: población y economía en el juego tardío

Documento de diseño para 7kmod-gameplay. Todavía no se implementó nada: aquí
están el problema, cómo funciona hoy el juego (según el código) y las propuestas
que discutimos, con sus pros, contras y preguntas abiertas.

## 1. El problema

En partidas largas los reinos acumulan muchos pueblos de 60 habitantes (el
máximo). El truco más fuerte es fundar un pueblo nuevo y pasarle 10 aldeanos de
5 pueblos cercanos: ahora hay 6 pueblos de 50 que vuelven a crecer. Eso produce
una explosión de población, y con un impuesto moderado la economía basada en
impuestos supera a la de minas, fábricas y mercados.

## 2. Cómo funciona hoy

### Crecimiento (`Town::population_grow`, una vez al mes)

- Cada raza del pueblo acumula `población × (100 + calidad de vida) / 100`
  puntos; cada 100 puntos nace una persona. En la práctica, un pueblo crece
  entre 1% y 2% al mes.
- Se detiene solo cuando el pueblo llega a 60 (`MAX_TOWN_GROWTH_POPULATION`), o
  mientras tiene defensores fuera.
- No hay límite de población del reino ni de cantidad de pueblos. Repartir
  aldeanos en pueblos nuevos quita el tope, y el crecimiento total del reino se
  vuelve exponencial.
- Los pueblos independientes crecen más lento (sin el bono de calidad de vida).

### Calidad de vida (`Town::update_quality_of_life`)

- Va de 0 a 100% y mide cuánto de la demanda de los 3 productos del pueblo
  cubren los mercados enlazados (cada producto vale 1/3).
- La demanda crece con la población del pueblo.
- Hoy solo es un bono: sin mercado el pueblo crece ×1, y con todo cubierto ×2.

### Comida (`Town::process_food`)

- Cada campesino (aldeano sin empleo) produce 30 de comida al año
  (`PEASANT_FOOD_YEAR_PRODUCTION`).
- Toda persona come 10 al año (`PERSON_FOOD_YEAR_CONSUMPTION`): campesinos,
  trabajadores, soldados y unidades.
- Un campesino deja +20 al año, así que alimenta a dos personas que no producen.
  El reino tiene comida mientras trabajadores + soldados ≤ 2 × campesinos.
- La IA a veces se queda sin comida por convertir demasiados campesinos en
  trabajadores y soldados.
- La comida sobrante no tiene salida: no se vende, solo sirve para tributos y
  ayuda en diplomacia.

### Impuestos (`Town::collect_tax`, `Town::collect_yearly_tax`)

- Cada cobro da $5 por persona (`TAX_PER_PERSON`) y baja 10 de lealtad, más una
  penalización acumulada si se cobra seguido (`accumulated_collect_tax_penalty`,
  +10 por cobro, baja 1 por día).
- El costo en lealtad es el mismo sin importar el tamaño del pueblo; el ingreso
  crece con la población.
- Cada 1 de enero hay un impuesto gratis de $5 por persona que no baja la
  lealtad.
- El cobro automático se dispara cuando la lealtad promedio llega al umbral
  elegido y la penalización acumulada es 0.
- La lealtad se recupera cada 5 días en (objetivo − actual) / 30, con un mínimo
  de 0.5. Un pueblo de 60 con cobro automático rinde unos $300 cada 1 a 3 meses,
  según lo lejos que esté su lealtad del objetivo.
- Los pueblos no tienen costo de mantenimiento; los edificios sí.

### Lealtad objetivo del pueblo (`Town::update_target_loyalty`, cada 15 días)

- Armonía de razas / 3 (0 a 33).
- Reputación del reino / 4 (−25 a +25).
- Empleo: +30 × proporción de aldeanos con trabajo (0 a 30). Un pueblo solo de
  campesinos tiene 30 puntos menos de objetivo.
- +10 si es de la raza del rey.
- Influencia de los generales de los fuertes enlazados.

### Rebeliones (`Town::think_rebel`)

- Un pueblo se rebela si 2/3 de su gente tiene lealtad de 29 o menos
  (`REBEL_LOYALTY`), como máximo una vez cada 3 meses y no en los 10 días
  siguientes a un ataque.
- Del grupo descontento se van entre 30% y 60% como rebeldes que atacan el
  pueblo, y la lealtad de los que se quedan sube.
- Un pueblo con lealtad baja también puede rendirse a otro reino que tenga
  influencia sobre él.

### Reinos nuevos (`Town::think_independent_form_new_nation`)

- Solo un pueblo independiente puede volverse reino de IA: necesita 30+
  habitantes, tiene 1/10 de probabilidad cada vez que se revisa y no puede
  pasar si ya hay 7 reinos.
- Está bloqueado si la población total de los reinos pasa de 420
  (`town_ai_emerge_nation_pop_limit`), lo que en el juego tardío siempre pasa.
- Tampoco aparecen pueblos independientes nuevos si la población total pasa de
  1000 o ya hay 10 independientes (`town_ai_emerge_town_pop_limit`).

## 3. Propuestas

### P1. El crecimiento depende de la calidad de vida y del empleo

**Calidad de vida.** El crecimiento pasa de ser un bono a ser un requisito:

| Calidad de vida | Hoy | Propuesta |
|---|---|---|
| 0% (sin mercado) | ×1 | ×0.25 |
| 33% (un producto) | ×1.33 | ×0.83 |
| 67% | ×1.67 | ×1.42 |
| 100% | ×2 | ×2 |

Fórmula de ejemplo: `×(0.25 + 1.75 × calidad de vida / 100)`.

**Empleo.** Un pueblo con menos de un 30% de trabajadores pierde parte del
crecimiento. El umbral debe ser bajo para no chocar con la comida (cada
campesino alimenta a dos personas).

**Exención para pueblos chicos.** Los pueblos de menos de 20–25 habitantes
crecen como hoy, para no frenar el inicio de la partida, cuando todavía no hay
mercados.

**Pros:**
- Para tener más población se necesita una economía que produzca más.
- La demanda crece con la población, así que el tamaño del reino queda atado a
  la cantidad de minas, fábricas y mercados.
- Las minas se agotan, lo que obliga a buscar y defender nuevas.
- Comprar en el mercado también da dinero, así que la economía de producción
  gana peso frente al impuesto.

**Contras y riesgos:**
- La IA podría crecer más lento que hoy si no cubre bien la demanda de sus
  pueblos. Hay que probarlo y quizá ajustar su lógica.
- No frena a un reino con una economía grande; solo hace que crecer cueste.

### P2. La sobrepoblación baja la lealtad

**Densidad.** Se suma la población de todos los pueblos a 12 casillas o menos,
incluido el propio. Medirla por zona, no por pueblo, hace que repartir aldeanos
en pueblos pegados no sirva: para evitar el castigo hay que expandirse a
territorio nuevo, que además hay que defender.

**Penalización.** −1 de lealtad objetivo por cada 10 personas arriba de 150,
hasta −25. La calidad de vida la reduce hasta un 50%: un pueblo bien abastecido
aguanta más vecinos.

**Opcional.** La misma densidad también reduce el crecimiento de la zona.

**Consecuencias, usando sistemas que ya existen:**
- Menos impuestos: con lealtad más baja, el cobro automático tarda más y rinde
  menos.
- Rebeliones: sacan entre 30% y 60% de los descontentos y la lealtad de los que
  se quedan sube, así que el sistema se regula solo.
- Rendiciones: un rival con influencia puede quedarse con pueblos de un reino
  sobrepoblado.

**Opcional: un reino nuevo sale del tuyo.** Hoy casi nunca pasaría, por el
límite de 420 de población. Haría falta un camino propio: por ejemplo, un pueblo
de 40+ habitantes muy sobrepoblado y con lealtad muy baja se independiza
directamente como reino de IA, ignorando ese límite. Seguiría sujeto al tope de
7 reinos.

**Contras y riesgos:**
- Sin contrapeso se siente como castigo puro y puede encadenar rebeliones; por
  eso la calidad de vida, el general y la reputación deben poder compensarlo.
- La IA también forma grupos de pueblos y podría sufrir rebeliones constantes.
- Debe mostrarse en el desglose de lealtad del pueblo ("Overcrowding −12").

### P3. Tope de población del reino

- Cuentan solo los habitantes de los pueblos, no los soldados ni las unidades,
  para no castigar tener ejército. 1000 equivale a unos 17 pueblos llenos.
- **Tope suave:** el crecimiento baja desde 800 y llega a 0 en 1000. Se siente
  más natural que un tope duro.
- **Lo que se bloquea:** el crecimiento por nacimientos y el poder del Greater
  Being egipcio (que agrega hasta 5 aldeanos a un pueblo).
- **Lo que no se bloquea:** conquistar o capturar pueblos, asentar unidades y
  contratar en el Inn. Con eso se puede pasar del tope, pero no crecer.
- Se muestra como "Population: 812 / 1000" en F1 o F3, con un aviso al llegar.
- Configurable; desactivado por defecto si se combina con P1 y P2, como red de
  seguridad.
- No cambia que el impuesto rinda más que las minas; solo limita cuánto puede
  crecer esa economía.

### P4. Otras ideas para la economía

- **Eficiencia administrativa (como la corrupción en Civilization).** El
  impuesto de cada pueblo baja según la cantidad de pueblos o la distancia al rey
  o a la Sede de Poder. Por ejemplo, 100% hasta 10 pueblos y −3% por cada pueblo
  extra, con mínimo 40%.
- **Impuesto distinto por tipo de habitante.** Los campesinos pagan menos ($3) y
  los trabajadores más ($5 o $6). Dar empleo con minas y fábricas sube el
  ingreso por impuestos.
- **Vender la comida sobrante** en el mercado o en el Harbor. No frena la
  explosión, pero le da valor a tener campesinos.
- **Crecimiento más lento cuanto más grande es el reino.** Un factor según la
  población total del reino; es parecido a P3 pero sin un número fijo.

### P5. La IA y la comida

Independiente de lo demás: que la IA no convierta campesinos en trabajadores ni
entrene soldados cuando su producción de comida no alcanza (trabajadores +
soldados ≤ 2 × campesinos, con margen).

### Descartada: que los aldeanos coman

Se propuso al principio, pero los aldeanos ya comen y los campesinos producen
más de lo que consumen, así que la comida no frena a los reinos con muchos
campesinos.

## 4. Plan sugerido

Las propuestas van en 7kmod-gameplay y cada una es configurable en la
configuración avanzada (0 = desactivada).

1. **P1 primero:** es la más simple y ya cambia mucho el juego tardío. Se prueba
   en partidas.
2. **P2 después,** calibrada con lo que muestre P1.
3. **P3** como red de seguridad opcional, desactivada por defecto.
4. **P4 y P5** según cómo se vea la economía tras P1 y P2.

Cada cambio debe mostrarse en la interfaz: un tooltip con el desglose del
crecimiento del pueblo ("Growth: ×0.83 (Quality of life 33%, Employment OK)") y
del lado de la lealtad ("Overcrowding −12").

## 5. Preguntas abiertas

- ¿Los números de la tabla de P1 están bien?
- ¿El umbral de pueblo chico es 20 o 25 habitantes? ¿Y el de empleo, 30%?
- ¿Incluir el reino nuevo que surge del tuyo, o bastan las rebeliones y las
  rendiciones?
- ¿P3 se usa en lugar de P2, combinado con ella, o solo como red de seguridad?
- Radio y umbrales de la densidad (12 casillas, 150 personas, −1 por cada 10,
  máximo −25): por calibrar.
