# 7kmod-gameplay: pendientes

Rama para los cambios que modifican el estilo de juego. La rama `7kmod`
queda solo con mejoras de comodidad (interfaz) sobre la 2.15.7.

---

## Checklist del próximo parche

Cosas anotadas para agregar en un parche. `[x]` = implementado, falta probarlo en partida.

- [ ] **Prioridad militar en el recuadro sobre unidades de otros reinos**
  (7kmod).
  - Hoy, al hacer el recuadro sin unidades tuyas, la lista
    (`unit_list_set_inspect` desde `Power::detect_select`) toma todas las
    unidades de otros reinos dentro del recuadro, militares y civiles.
  - Cambio: usar la misma regla que con tus unidades. Si dentro hay al menos
    una unidad militar (soldados, generales, reyes, armas de asedio, barcos de
    guerra, Fryhtans y Greater Beings), la lista y la unidad elegida solo
    toman militares. Si solo hay civiles (constructores, trabajadores,
    caravanas, barcos Trader y Transport...), se toman todos como hoy.
  - Con `Alt` presionado se toman todas, como con tus unidades.
  - Reutilizar `is_military_unit` de `src/OPOWER.cpp`. Revisar que no
    revele espías enemigos: un espía enemigo debe clasificarse por lo que
    aparenta ser (su habilidad visible), no por ser espía.
  - Actualizar `KEYBINDINGS.md` y el changelog de 7kmod.

---

## Tecnologías del mod (`ModTech`)

Las tecnologías del mod están en `include/ModTech.h` y `src/ModTech.cpp`,
separadas de las armas de `TechRes`. Se investigan en "Technologies" de la
Torre de Investigación y no tocan la diplomacia, los reportes ni las demás
partes que asumen que una tecnología es un arma.

- **Hechas:** Elite Warriors, Elite Workers, Building Repair y Fryhtan Slayers.
- **Agregar una tecnología:** una entrada en el enum y en
  `mod_tech_info_array` (nombre, descripción, costo, niveles), su efecto con
  `mod_tech_res.get_level(...)`, y subir `GAME_VERSION_7KMOD` con su
  `MAX_MOD_TECH_BEFORE_...` para leer las partidas anteriores
  (`GameFile::read_file_3` en `src/OGFILE2.cpp`).
- **Pendientes:**
  - Costo real de Building Repair y Fryhtan Slayers (hoy es muy barato, solo para pruebas).
  - Que la IA investigue las tecnologías del mod.
  - Íconos propios (hoy reutilizan imágenes del juego).
  - Más tecnologías. Ideas: entrenar más rápido o más barato, minas o
    fábricas más productivas, bonos de lealtad.

El análisis original de las mejoras permanentes quedó en el historial de git
de este archivo.
