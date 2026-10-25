# 7kmod-gameplay: pendientes

Rama para los cambios que modifican el estilo de juego. La rama `7kmod`
queda solo con mejoras de comodidad (interfaz) sobre la 2.15.7.

---

## Checklist del próximo parche

Cosas anotadas para agregar en un parche. `[x]` = implementado, falta probarlo en partida.

- [ ] **Los disparos a distancia ya no se bloquean por edificios ni obstáculos.** Hoy, un
  proyectil recto (flechas, jabalinas, etc.) no puede pasar por casillas no caminables:
  edificios, murallas, montañas y otros obstáculos del mapa
  (`BulletArray::bullet_path_possible`, `src/OBULLETA.cpp`). Las unidades ya no
  bloqueaban (esa revisión está desactivada en el original), y los proyectiles curvos
  (tipo 'P', como los de las armas de asedio) nunca se revisan. Cuando el tiro está
  bloqueado, el arquero busca otra casilla desde donde disparar y se mueve, a veces
  quedando pegado al enemigo. Con el cambio, todos los disparos pasan por encima de
  todo, como los proyectiles curvos.
  - Aplica a todas las unidades a distancia de todos los reinos (también la IA, los
    Fryhtans como el Doink y los barcos).
  - Efecto secundario: se puede disparar a unidades detrás de murallas o al otro lado
    de una montaña, si están dentro del alcance.
  - Va en `7kmod-gameplay` (y por lo tanto en `7kmod-pathing`).

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
