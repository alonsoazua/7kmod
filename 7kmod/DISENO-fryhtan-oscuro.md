# Diseño: la economía oscura del reino Fryhtan (borrador)

Documento de referencia para la siguiente etapa del reino Fryhtan, en la rama
`7kmod-fryhtan`. **Es un borrador:** recoge la idea general y lo que ya se decidió, y
deja marcado lo que falta definir. Cuando se apruebe, se pasa a fases en
`TODO-fryhtan.md`.

**Leyenda:** ✅ decidido · 💡 propuesta, por confirmar · ❓ por definir

---

## Idea general

Hoy el reino Fryhtan entrena Fryhtans en sus guaridas, esclaviza pueblos y cobra
tributo, pero no tiene en qué gastar el oro aparte de guaridas y capturas, ni usa a su
gente esclavizada más que para el tributo.

La idea es darle una **economía propia, en espejo oscuro de la humana**:

- En los pueblos esclavizados se entrena un **Enforcer**, un capataz humano que hace de
  constructor y de trabajador.
- El Enforcer construye **versiones oscuras de edificios humanos**: el mercado negro,
  la torre oscura de ciencia y la fábrica de guerra oscura.
- Esos edificios **no usan trabajadores**: los maneja un solo Enforcer, y su skill
  decide qué tan bien funcionan.
- La torre oscura investiga tecnologías, como los **soldados oscuros**, que hace la
  fábrica de guerra oscura: humanos esclavizados con combate fijo, sin lealtad, el
  equivalente a las máquinas de guerra.

---

## 1. El Enforcer

| | |
|---|---|
| ✅ Dónde se entrena | En un pueblo esclavizado. |
| ✅ Costo | **$200** y **1 aldeano** del pueblo. |
| ✅ Raza | La del pueblo donde se entrena (es un humano de esa raza). |
| ✅ Qué hace | Construye los edificios oscuros y luego trabaja en uno. |
| ✅ Uno por edificio | Cada edificio oscuro tiene un solo Enforcer, como el general de un fuerte. |
| ✅ Skill | Sube con el tiempo mientras trabaja, como el de los trabajadores humanos. |
| ✅ Efecto del skill | Más velocidad de producción (investigar, fabricar soldados); en el mercado negro, compras más baratas. |
| 💡 Tiempo de entrenamiento | Como entrenar una unidad en un pueblo humano. |
| 💡 Skill inicial | Bajo (por ejemplo 20), para que valga la pena dejarlo trabajar. |
| 💡 Restricciones | No puede ser general, no funda pueblos, no construye edificios humanos normales. |
| ❓ Lealtad y sueldo | ¿Tiene lealtad (puede desertar) o no? ¿Cobra sueldo? |
| ❓ Si muere | El edificio deja de funcionar hasta que llegue otro Enforcer. |

---

## 2. Los edificios oscuros

Son los mismos edificios humanos, en una versión que solo construye el reino Fryhtan,
con otro nombre y dibujados más oscuros (con la tabla de colores, sin gráficos nuevos).
Ninguno usa trabajadores: solo su Enforcer.

### 2.1 Mercado negro (Black Market)

| | |
|---|---|
| ✅ Qué hace | Compra **esclavos** con oro para aumentar la población de un pueblo esclavizado. |
| ✅ Enlace | Se enlaza a **un solo** pueblo esclavizado. |
| ✅ Raza de los esclavos | La del pueblo enlazado. |
| ✅ Precio | **Sube con cada compra**. |
| ✅ Skill del Enforcer | Baja el precio de compra. |
| 💡 Efecto | Más gente → más tributo, más comida y más tope de población Fryhtan (cada persona esclavizada sube el tope en 1). |
| ❓ Precio | Precio base, cuánto sube por compra, y si baja con el tiempo. |
| ❓ Comercio | ¿También hace de mercado normal (vender materias primas y productos), o solo esclavos? |

### 2.2 Torre oscura de ciencia (Dark Tower of Science)

| | |
|---|---|
| ✅ Qué hace | Investiga tecnologías oscuras. |
| ✅ Skill del Enforcer | Más velocidad de investigación. |
| ✅ Primera tecnología | **Soldados oscuros** (habilita la fábrica de guerra oscura). |
| ❓ Más tecnologías | Por definir (ver sección 4). |

### 2.3 Fábrica de guerra oscura (Dark War Factory)

| | |
|---|---|
| ✅ Qué hace | Fabrica **soldados oscuros** (por ahora solo eso). |
| ✅ Skill del Enforcer | Más velocidad de fabricación. |
| ❓ Enlace | ¿Necesita un pueblo esclavizado enlazado, de donde saca el aldeano de cada soldado? |

### Otros edificios

❓ Por ahora solo esos tres. Falta pensar si tiene sentido una mina o fábrica oscura.

---

## 3. El soldado oscuro

| | |
|---|---|
| ✅ Qué es | Un humano esclavizado entrenado para pelear: el equivalente a una máquina de guerra. |
| ✅ Combate | **Fijo, 65**. No sube con la experiencia. |
| ✅ Lealtad | No tiene. |
| ✅ Restricciones | No puede ser general ni fundar pueblos. |
| ✅ Costo | **Oro**, como una máquina de guerra, y **1 aldeano** del pueblo enlazado. |
| ✅ Raza | La del pueblo enlazado. |
| ✅ Población | **No cuenta** para el tope de población Fryhtan. |
| ❓ Precio y tiempo | Cuánto oro y cuántos días. |
| ❓ Mantenimiento | ¿Cuesta oro al año (como las armas) o comida (como los soldados humanos)? |
| ❓ Fuertes | ¿Puede entrar en una guarida como soldado? |

---

## 4. Tecnologías

✅ Se investigan en **dos lugares**:

- **Torre oscura de ciencia:** tecnologías "humanas oscuras", como los soldados oscuros.
- **Guarida:** tecnologías especiales de los Fryhtans (de la especie, de la horda).

❓ La lista completa está por definir. Ideas para pensar:

- Torre oscura: soldados oscuros mejorados (más combate), Enforcers más hábiles,
  mercado negro más barato.
- Guarida: Fryhtans más fuertes o más rápidos de entrenar, más soldados por guarida,
  guaridas más resistentes.
- ❓ ¿Cómo se investiga en la guarida? ¿La investiga el general (su liderazgo como
  "skill") o cuesta comida?

---

## 5. Notas técnicas

- **Edificios:** usar los mismos tipos (`FIRM_MARKET`, `FIRM_RESEARCH`,
  `FIRM_WAR_FACTORY`) cuando el dueño es un reino Fryhtan, con su nombre propio, dibujo
  oscurecido y un panel distinto (Enforcer en lugar de trabajadores).
- **Enforcer y soldado oscuro:** unidades humanas de la raza del pueblo, con una marca
  propia (Enforcer o soldado oscuro) guardada en `ModFryhtan`.
- **Construcción:** el Enforcer tiene un menú Build con solo los edificios oscuros. Los
  Fryhtans siguen sin poder construir edificios humanos.
- **Partidas guardadas:** ✅ sube la versión de partida de la rama Fryhtan. Las
  partidas Fryhtan que ya existen se siguen cargando.
- ❓ **Capturas:** qué pasa si un reino humano captura un edificio oscuro, o si el reino
  Fryhtan captura uno humano.

---

## 6. Fases propuestas

1. **El Enforcer:** entrenarlo en el pueblo esclavizado y su menú de construcción.
2. **Mercado negro:** compra de esclavos, enlace a un pueblo y precio creciente.
3. **Torre oscura de ciencia:** investigar, con la tecnología de soldados oscuros.
4. **Fábrica de guerra oscura:** fabricar soldados oscuros.
5. **Tecnologías de la guarida:** cuando esté definida la lista.
