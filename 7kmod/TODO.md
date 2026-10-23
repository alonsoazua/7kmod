# 7kmod-gameplay: pendientes

Rama para los cambios que modifican el estilo de juego. La rama `7kmod`
queda solo con mejoras de comodidad (interfaz) sobre la 2.15.7.

---

## Checklist del próximo parche

Cosas anotadas para agregar en un parche. `[x]` = implementado, falta probarlo en partida.

- [ ] **Corregir: restos gráficos al dar clic en un soldado oscurecido del
  fuerte** (7kmod).
  - Al dar clic en un soldado que está fuera del fuerte, la cámara va a él y
    se abre su panel, pero siguen apareciendo barras de vida de los soldados
    del fuerte encima (solo las del lado izquierdo).
  - Causa probable: `Firm::detect_worker_list` selecciona al soldado
    (`world.go_loc(..., 1)`) y devuelve 1; entonces `FirmCamp::detect_info`
    vuelve a dibujar el panel del fuerte (`disp_camp_info`,
    `disp_worker_list`, `disp_worker_info` con `INFO_UPDATE`) encima del panel
    del soldado que ya se abrió.
  - Corrección: cuando el clic selecciona a una unidad fuera del fuerte, no
    volver a dibujar el panel del fuerte (por ejemplo, revisar que el fuerte
    siga seleccionado antes de redibujar, o devolver un código distinto).
    Revisar lo mismo con el clic en el general oscurecido.

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
