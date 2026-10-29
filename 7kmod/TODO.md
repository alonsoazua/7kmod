# 7kmod-gameplay: pendientes

Rama para los cambios que modifican el estilo de juego. La rama `7kmod`
queda solo con mejoras de comodidad (interfaz) sobre la 2.15.7.

---

## Checklist del próximo parche

Cosas anotadas para agregar en un parche. `[x]` = implementado, falta probarlo en partida.

- [x] **Los disparos a distancia ya no se bloquean por edificios ni obstáculos.** Hoy, un
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

- [ ] **Tooltip de los poderes de los Greater Beings mughul, egipcio y zulú.** El botón
  del poder (`UnitGod::disp_info`, `src/OU_GOD.cpp`) usa como ayuda el nombre de su ícono,
  y `RESOURCE/HELP.RES` no trae `GODMUGL`, `GODEGYPT` ni `GODZULU` (sí los de Persia,
  Japón, Maya y Thor): esos tres botones no tienen tooltip. Es un error del juego
  original. Arreglo: ayuda propia (`help.set_custom_help`), sin tocar los datos, con el
  estilo de los demás:
  - Mughul: "Lower Enemy Loyalty": el cursor cambia a un círculo; sobre unidades de
    reinos en guerra contigo o independientes, les baja la lealtad (y, con el cambio de
    abajo, convierte a un soldado).
  - Egipcio: "Increase Population": sobre un pueblo de tu reino, le agrega habitantes.
  - Zulú: "Increase Leadership": sobre un general o rey zulú de tu reino, le sube el
    liderazgo.
  - Va en `7kmod` (es un error del original) y se pasa a gameplay y pathing.

- [ ] **Djini (Greater Being mughul): además convierte a un soldado.** Hoy su poder baja
  −20 a −30 de lealtad a las unidades visibles del área que sean de reinos en guerra
  contigo o de nadie (`UnitGod::indian_cast_power`). Nuevo: en cada lanzamiento, además,
  **el soldado con menos combate del área** (rango soldado, no general ni rey) pasa a tu
  reino (`Unit::change_nation`).
  - Solo uno por lanzamiento, entre las mismas unidades a las que baja la lealtad.
  - Por definir:
    - Empates de combate: el de menor lealtad (después de bajarla).
    - ¿Solo personas, o también armas de asedio y Fryhtans invocados? Propuesta: solo
      personas.
    - Espías disfrazados: no se convierten (seguirían siendo espías de su reino).
    - Lealtad con la que llega a tu reino (por ejemplo, la de una unidad que se rinde).
    - El tooltip del poder lo menciona.
  - Va en `7kmod-gameplay` (cambia el balance).

- [ ] **Phoenix (Greater Being griego): poder para exponer espías y menor costo.** Hoy el
  Fénix solo explora y revela información; no tiene poder (`can_cast_power` en 0) y es
  el que más cuesta mantener. Nuevo:
  - **Botón de poder** en su panel, como los de los demás seres (`UnitGod::disp_info`,
    `src/OU_GOD.cpp`): al dar clic, se elige un **pueblo de tu reino** (alcance de una
    casilla, como Isis). Si en el pueblo hay espías de otros reinos, **expone a uno**: si
    hay varios, al de **menor habilidad de espionaje**.
  - **Costo de mantener: de 2.5 a 1.5 por día** (en la tabla `GOD` son 50 puntos de
    existencia ÷ 20; quedaría en 30), igual que la mayoría. Se cambia en el código al
    cargar la tabla (`GodRes`), sin tocar los datos.
  - Por definir:
    - "Exponer": propuesta, como cuando lo atrapa el contraespionaje
      (`SpyArray::catch_spy`): el espía es descubierto y ejecutado, con la noticia normal.
      La otra opción es solo mostrarlo (sin matarlo).
    - Costo del poder: propuesta, 100 puntos como los demás poderes.
    - Si el pueblo no tiene espías ajenos: el poder no se gasta y sale un aviso.
    - ¿También en edificios de tu reino? Propuesta: solo pueblos, como pediste.
    - Ícono y tooltip del botón (no hay ícono del juego para esto; habría que hacer uno
      o usar uno existente, por ejemplo el de los espías).
    - Que la IA lo use.
  - Actualizar `GREATER_BEINGS.md` (hoy dice que el Fénix no detecta espías).
  - Va en `7kmod-gameplay` (cambia el balance).

---

## Tecnologías del mod (`ModTech`)

Las tecnologías del mod están en `include/ModTech.h` y `src/ModTech.cpp`,
separadas de las armas de `TechRes`. Se investigan en "Technologies" de la
Torre de Investigación y no tocan la diplomacia, los reportes ni las demás
partes que asumen que una tecnología es un arma.

- **Hechas:** Elite Warriors, Elite Workers, Building Repair y Fryhtan Slayers.
- **Agregar una tecnología:** una entrada en el enum y en
  `mod_tech_info_array` (nombre, descripción, costo, niveles), su efecto con
  `mod_tech_res.get_level(...)`, su ícono en `src/ModTechIcon.cpp` (bitmap de
  46×38 con índices de `PAL_STD.RES`, mismo orden que el enum), y subir `GAME_VERSION_7KMOD` con su
  `MAX_MOD_TECH_BEFORE_...` para leer las partidas anteriores
  (`GameFile::read_file_3` en `src/OGFILE2.cpp`).
- **Pendientes:**
  - Costo real de Building Repair y Fryhtan Slayers (hoy es muy barato, solo para pruebas).
  - Que la IA investigue las tecnologías del mod.
  - [x] Íconos propios: cada tecnología tiene su ícono de 46×38 con la paleta del juego
    (`src/ModTechIcon.cpp`), en el menú y en el panel de investigación. La descripción
    pasó a la ayuda del botón.
  - Más tecnologías. Ideas: entrenar más rápido o más barato, minas o
    fábricas más productivas, bonos de lealtad.

El análisis original de las mejoras permanentes quedó en el historial de git
de este archivo.
