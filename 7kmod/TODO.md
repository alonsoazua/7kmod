# 7kmod-gameplay: pendientes

Rama para los cambios que modifican el estilo de juego. La rama `7kmod`
queda solo con mejoras de comodidad (interfaz) sobre la 2.15.7.

---

## 1. Mejoras permanentes en la Torre de Investigación

**Estado:** primera tecnología implementada con un enfoque distinto al de abajo:
un sistema de tecnologías propio del mod (`ModTech.h` / `ModTech.cpp`), separado de
las armas de `TechRes`. La torre muestra primero "Technologies" o "War Machines";
las tecnologías del mod solo aparecen en "Technologies", así que no tocan la
diplomacia, los reportes ni las demás partes que asumen que una tecnología es un arma.

- **Guerreros élite (Elite Warriors):** las unidades entrenadas en Leadership empiezan
  con 30 de combate en lugar de 20 (el Leadership sigue en 20). La IA no la investiga.
  El costo (`complex_level` 0.1) es muy bajo, solo para pruebas.
- Pendiente: costo real, que la IA use las tecnologías, ícono propio y más tecnologías.

El análisis original (agregar las mejoras a la lista de armas) queda abajo como referencia.

**Idea:** además de desbloquear armas, la Torre de Investigación puede
investigar mejoras permanentes para la nación. Primera mejora:
**Entrenamiento avanzado**, que hace que Entrenar genere unidades con
habilidad 30 en vez de 20.

### Cómo funciona hoy la investigación

- Las tecnologías vienen de `TECH.DBF` dentro de `RESOURCE/STD.SET`
  (clase, niveles máximos, costo `complex_level`, requisito padre).
  Las carga `TechRes::load_tech_info()` en `src/OTECHRES.cpp`.
- Ícono en `I_TECH.RES`. El nombre es el del arma: `TechInfo::tech_des()`
  devuelve el nombre de `unit_id` o `firm_id`.
- La torre (`src/OF_RESE.cpp`) lista lo que cumple `TechInfo::can_research()`.
  Al completar un nivel, `TechInfo::set_nation_tech_level()` sube el nivel
  del arma para esa nación.

### Plan propuesto

- **Efecto:** `Town::recruit()` usa la constante `TRAIN_SKILL_LEVEL` (20)
  para la habilidad y, con Leadership, también para el combate. Se cambia
  por una función, por ejemplo `20 + 10 * nivel de la mejora`.
- **No editar `STD.SET`**: lo comparte el juego original en la misma
  carpeta. Las mejoras se definen en el código del mod y se agregan al
  final de la lista de tecnologías después de cargar los datos.
- **Base de "mejoras permanentes"**: una tabla en el código (nombre,
  descripción, costo, niveles, requisito, ícono) y una consulta tipo
  `mod_tech_level(nation, MODTECH_ADVANCED_TRAINING)`. Las mejoras
  siguientes serían casi solo agregar una fila y su efecto.
- **Ícono:** reutilizar una imagen existente del juego al inicio. Una
  imagen nueva requiere convertirla a la paleta de 8 bits y empaquetarla.

### Lugares que asumen que toda tecnología es un arma (hay que revisarlos)

- `src/OF_RESE.cpp`: el sonido al terminar la investigación usa
  `unit_res[tech->unit_id]->sprite_id`. Con `unit_id == 0` **el juego se cierra**.
- `src/ONATIONB.cpp` `NationBase::total_tech_level()`: filtra por
  `unit_res[unit_id]->unit_class`.
- `src/OTECHRES.cpp` `tech_des()` / `set_nation_tech_level()`: nombre y
  efecto para tecnologías sin `unit_id` ni `firm_id`.
- Noticias: `src/ONEWSENG.cpp` (tecnología investigada).
- Diplomacia: `src/OAI_DIPL.cpp`, `src/OAI_TALK.cpp`, `src/OTALKFRE.cpp`,
  `src/ONATIONB.cpp` (pedir, regalar e intercambiar tecnologías).
- Reporte de tecnologías: `src/OR_TECH.cpp`.
- IA de investigación: `src/OF_RESE2.cpp` (recorre todas las tecnologías;
  `total_tech_level` se usa para saber si ya investigó todo).

### Partidas guardadas

- `TechRes::read_file()` (`src/OGFILE2.cpp`) lee una entrada por
  tecnología. Al agregar mejoras cambia la cantidad, así que hay que subir
  `GAME_VERSION_7KMOD` (290 → 291) y leer la cantidad anterior en
  partidas viejas. Igual que se hizo con `TownGF` y
  `TOWN_GF_SIZE_BEFORE_7KMOD`.
- Los escenarios (`.SCN`) son partidas guardadas: se aplica lo mismo.

### Dudas de diseño pendientes

1. ¿A qué entrenamiento aplica? (a) solo Leadership (habilidad y combate)
   o (b) todas las habilidades que entrena el pueblo.
2. ¿Cuántos niveles? Uno (20 → 30) o varios (30, 40, 50).
3. ¿Costo de investigación? Igual, más o menos que un nivel de arma.
4. ¿Requisito? Disponible desde el inicio o con requisito previo.
5. ¿La IA también la investiga? (recomendado: sí)
6. ¿Afecta a Reclutar instantáneo? (supuesto: no, solo a Entrenar)

### Ideas para mejoras siguientes

- Entrenar más rápido o más barato.
- Minas o fábricas más productivas.
- Bonos de lealtad.

### Etapas sugeridas

1. Base de mejoras permanentes + Entrenamiento avanzado (ícono reutilizado).
2. Más mejoras sobre la misma base.
