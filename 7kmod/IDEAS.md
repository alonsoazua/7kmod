# 7kmod: ideas de cambios

Ideas que se analizaron y que interesan, pero que no se van a implementar por
ahora. Cuando se decida hacer alguna, se pasa al checklist de `TODO.md` (o de
`TODO-pathing.md`).

---

## Hambre (falta de comida)

**Situación actual (juego original):** sin comida, solo las unidades pierden
lealtad (1 punto cada 5 días). Los pueblos siguen comiendo y la reserva baja en
negativo sin ninguna consecuencia. Los soldados dentro de un fuerte no desertan
(la traición por baja lealtad solo se revisa para las unidades en el mapa), y con
el rey al mando su lealtad se equilibra alrededor de 40, porque cada 30 días sube
una décima parte de lo que le falta para llegar a su meta (casi 100 con el rey).

Ejemplo visto en una partida: una IA sin comida se mantuvo indefinidamente con un
pueblo de una docena de trabajadores (ningún campesino), un fuerte con 8 soldados
y el rey.

Ideas:

- **Hambre en los pueblos:** sin comida, la lealtad del pueblo baja poco a poco, o
  la población deja de crecer y empieza a disminuir (una hambruna).
- **Hambre en la meta de lealtad:** en vez de solo restar puntos, el hambre resta
  a la meta de lealtad de los soldados (por ejemplo −30), para que ni el rey pueda
  sostenerla y la lealtad baje hasta un nivel peligroso.
- **Deserción en los fuertes:** los soldados de un fuerte con lealtad muy baja
  (menos de 20, por ejemplo) se van.
- **IA sin comida:** cuando se queda sin comida, la IA convierte trabajadores en
  campesinos o compra comida en el mercado.

---

## Otros pendientes

Temas analizados que quedaron en espera (ver la conversación de cada uno):

- **El general dirige desde atrás:** al atacar un fuerte o un pueblo, el general o
  el rey no se adelanta a sus soldados. En `7kmod-pathing` 0.4.0 ya va en la última
  fila de la formación al moverse, pero no al atacar.
- **IA en bancarrota:** la IA que se queda sin dinero termina rindiéndose (vendiendo
  el trono) a otra IA, aunque fuera más fuerte; revisar el cálculo y mejorar su
  manejo de impuestos.
- **Población en el juego tardío:** ver `POPULATION_PROPOSALS.md`.
