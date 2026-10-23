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
- [ ] **Botón Withdraw en el fuerte** (comodidad: va en 7kmod y pasa a
  gameplay con el rebase).
  - Hace que todas las unidades que salieron del fuerte regresen a él: el
    general (o rey), los soldados, las armas y los Fryhtans. Son las mismas
    que el panel muestra oscurecidas (las que tienen ese fuerte en
    `home_camp_firm_recno` y siguen en el mapa), pero sin el límite de
    lugares libres.
  - Usa la orden de regresar al fuerte que ya existe:
    `UnitArray::return_camp(remoteAction, unitArray, count)`, que ya tiene su
    mensaje de red (`MSG_UNITS_RETURN_CAMP`) para multijugador. Revisar que
    mande el mensaje también cuando recibe la lista de unidades.
  - Se desactiva si no hay unidades fuera del fuerte.
  - **Posición:** en el lugar del botón Alert / At ease (defensa automática),
    en la página 1 de los botones del fuerte.
  - **El botón Alert / At ease pasa a la página 2,** en la misma posición
    (a la derecha). Hoy la página 2 solo existe si tienes espías dentro del
    fuerte (`Firm::button_page` devuelve 0 si `player_spy_count` es 0). Con
    este cambio, los fuertes propios siempre tienen página 2, con las flechas
    para cambiar de página; el botón de espías sigue en la página 2 cuando hay
    espías.
  - Ícono: reutilizar el de "regresar al fuerte" del panel de la unidad
    (`RETCAMP`).
  - Tooltip "Withdraw: all the units of this fort outside of it return".
  - Actualizar `KEYBINDINGS.md` si se le asigna una tecla, y el changelog de
    7kmod.

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
