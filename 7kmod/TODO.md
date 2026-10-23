# 7kmod-gameplay: pendientes

Rama para los cambios que modifican el estilo de juego. La rama `7kmod`
queda solo con mejoras de comodidad (interfaz) sobre la 2.15.7.

---

## Checklist del próximo parche

Cosas anotadas para agregar en un parche. `[x]` = implementado, falta probarlo en partida.

- [ ] **Tooltip con la raza en el retrato del rey (F1)** (comodidad: va en
  7kmod y pasa a gameplay con el rebase).
  - Al dejar el ratón sobre el retrato del rey en la lista de reinos
    (`put_nation_rec` en `src/OR_NAT.cpp`) aparece la raza del rey, por
    ejemplo "Japanese".
  - Se toma la misma raza que decide el retrato: la del rey actual, o la del
    reino si en ese momento no hay rey.
  - Usar `help.set_custom_help` sobre el área del retrato (24×20), como los
    tooltips de lealtad. Igual que toda la ayuda del juego, solo aparece en
    partidas de un jugador y con la ayuda activada.
  - Actualizar el changelog de 7kmod.

---

## Tecnologías del mod (`ModTech`)

Las tecnologías del mod están en `include/ModTech.h` y `src/ModTech.cpp`,
separadas de las armas de `TechRes`. Se investigan en "Technologies" de la
Torre de Investigación y no tocan la diplomacia, los reportes ni las demás
partes que asumen que una tecnología es un arma.

- **Hechas:** Elite Warriors, Elite Workers y Building Repair.
- **Agregar una tecnología:** una entrada en el enum y en
  `mod_tech_info_array` (nombre, descripción, costo, niveles), su efecto con
  `mod_tech_res.get_level(...)`, y subir `GAME_VERSION_7KMOD` con su
  `MAX_MOD_TECH_BEFORE_...` para leer las partidas anteriores
  (`GameFile::read_file_3` en `src/OGFILE2.cpp`).
- **Pendientes:**
  - Costo real de Building Repair (hoy es muy barato, solo para pruebas).
  - Que la IA investigue las tecnologías del mod.
  - Íconos propios (hoy reutilizan imágenes del juego).
  - Más tecnologías. Ideas: entrenar más rápido o más barato, minas o
    fábricas más productivas, bonos de lealtad.

El análisis original de las mejoras permanentes quedó en el historial de git
de este archivo.
