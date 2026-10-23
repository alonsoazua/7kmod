# 7kmod-gameplay: pendientes

Rama para los cambios que modifican el estilo de juego. La rama `7kmod`
queda solo con mejoras de comodidad (interfaz) sobre la 2.15.7.

---

## Checklist del próximo parche

Cosas anotadas para agregar en un parche. `[x]` = implementado, falta probarlo en partida.

- [ ] **Corregir: el fondo de la lista de unidades se aclara cada vez más**
  (7kmod).
  - En el panel de la lista de unidades seleccionadas, el fondo se vuelve más
    blanco cada vez que cambia la lista (por ejemplo, cuando muere una
    unidad).
  - Causa (revisada en el código): cuando la lista cambia, `unit_list_disp`
    (`src/OUNITLST.cpp`) vuelve a pintar el panel por su cuenta con
    `vga_util.d3_panel_up`. Ese panel no es opaco: aclara lo que ya hay debajo
    (`adjust_brightness` sobre el fondo), y como debajo ya estaba el panel
    aclarado de antes, cada repintado lo aclara un poco más. En un repintado
    normal (`info.disp()`) el juego primero restaura el fondo, por eso solo se
    aclara una vez.
  - Corrección: cuando la lista cambie, pedir el repintado completo con
    `info.disp()` en lugar de repintar el panel directamente.
  - **El mismo problema en el fuerte:** `FirmCamp::disp_camp_info` repinta su
    panel con `d3_panel_up` cuando cambia el general que está fuera del fuerte
    (cuando sale, muere o regresa), así que ese panel también se aclara. Misma
    corrección.
  - Revisar si queda algún otro repintado parcial con `d3_panel_up` en el
    código del mod.
- [ ] **Barra de vida en el mapa para las unidades de otros reinos del
  recuadro** (7kmod).
  - Hoy el juego dibuja la barra de vida sobre las unidades seleccionadas
    (`Unit::draw_selected`, llamado desde `Unit::draw` y
    `Unit::draw_outlined` en `src/OUNITDRW.cpp` cuando `selected_flag` está
    activo). Al seleccionar con el recuadro unidades de otros reinos, solo la
    unidad elegida queda seleccionada; las demás están en la lista aparte
    (`unit_list_set_inspect`) y no muestran su barra.
  - Cambio: mientras la unidad mostrada en el panel sea una de esa lista,
    todas las unidades de la lista muestran su barra de vida sobre ellas en
    el mapa, igual que las unidades seleccionadas.
  - Hace falta una función en `OUNITLST` que diga si una unidad está en la
    lista activa (la lista y la unidad del panel), y usarla en `Unit::draw` y
    `Unit::draw_outlined` junto a `selected_flag`.
  - No se muestra el ícono de habilidad que acompaña a la barra en las
    unidades enemigas, igual que hoy con una sola unidad enemiga
    seleccionada.

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
