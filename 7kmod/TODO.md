# 7kmod-gameplay: pendientes

Rama para los cambios que modifican el estilo de juego. La rama `7kmod`
queda solo con mejoras de comodidad (interfaz) sobre la 2.15.7.

---

## Checklist del próximo parche

Cosas anotadas para agregar en un parche. `[x]` = implementado, falta probarlo en partida.

- [ ] **El general y los soldados fuera del fuerte se siguen viendo, oscurecidos** (comodidad:
  va en 7kmod y pasa a gameplay con el rebase).
  - Cuando el general sale del fuerte (patrulla o clic derecho), el fuerte
    queda sin general y su panel no muestra a nadie (`FirmCamp::disp_camp_info`
    termina si `overseer_recno` es 0). Se pierde de vista a quién pertenece ese
    fuerte.
  - Mientras el fuerte no tenga general, se muestra en su lugar el retrato del
    general que salió de él, oscurecido, con su nombre, para indicar que está
    fuera.
  - Para saber quién es: el juego ya guarda en cada unidad que sale el fuerte
    al que regresa (`home_camp_firm_recno`, lo pone
    `FirmCamp::mobilize_overseer`). Se busca la unidad con ese fuerte como
    casa, que sea general o rey y que siga en el mapa (no dentro de otro
    edificio).
  - Si el general muere, entra a otro fuerte o cambia de reino, ya no se
    muestra. Si otro general entra al fuerte, se muestra el nuevo como
    siempre.
  - También aplica al rey cuando sale de su fuerte.
  - Al dar clic en el retrato oscurecido, la cámara se mueve al general y lo
    selecciona.
  - **Lo mismo para los soldados:** los soldados que salieron del fuerte
    (con el mismo `home_camp_firm_recno`, puesto por
    `FirmCamp::mobilize_worker`) y siguen en el mapa se muestran oscurecidos
    en los lugares vacíos de la lista de soldados del fuerte. Clic en uno
    mueve la cámara a él y lo selecciona.
    - Solo caben en los lugares libres (8 en total). Si mientras estaban fuera
      entraron otros soldados al fuerte, se muestran los que quepan.
    - No se cuentan como soldados del fuerte: no entrenan, no cuentan para la
      lealtad del general ni para sus botones.
  - Actualizar el changelog de 7kmod.
- [ ] **Selección con prioridad militar** (comodidad: va en 7kmod y pasa a
  gameplay con el rebase).
  - Hoy el recuadro de selección (`Power::detect_select` en `src/OPOWER.cpp`)
    toma todas tus unidades dentro de él.
  - Si el recuadro incluye al menos una unidad militar, solo se seleccionan las
    militares. Si solo hay civiles, se seleccionan como hoy.
  - Con `Alt` presionado (hoy no hace nada en la selección) se seleccionan
    todas, como antes.
  - Militares: soldados, generales, rey, armas de asedio, barcos de guerra
    (Caravel, Galleon) y los Fryhtans de tu reino.
  - Civiles: constructores, mineros, trabajadores, investigadores, espías,
    caravanas y barcos Trader y Transport.
  - Shift + recuadro (agregar a la selección) sigue la misma regla.
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
