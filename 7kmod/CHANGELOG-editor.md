# Changelog de 7kmod-editor

Cambios de la rama `7kmod-editor` (ejecutable `7kaa-editor.exe`), basada en `7kmod`. Lo
de `7kmod` está en `CHANGELOG.md`. El diseño y las fases están en `TODO-editor.md`.

---

## [Sin publicar]

### Agregado
- **Editor de escenarios, fase 0 (MVP):**
  - **Acceso:** Single Player → Scenario → botón **New** (arriba de "Your Name"). Lleva a
    la pantalla de opciones de New Game; al darle Start, el mapa se abre en el editor. El
    botón **Edit** está desactivado por ahora.
  - **Modo editor:** el juego queda en pausa (las teclas de velocidad no hacen nada) y se
    ve todo el mapa. Lo explorado no cambia: al probar el escenario se usa la niebla y el
    mapa elegidos. No se dan órdenes a las unidades; solo funcionan las teclas F11
    (captura), F12 (probar) y Esc (quitar la selección).
  - **Barra superior** en lugar de los botones de los reportes: **Scenario**,
    **Kingdoms**, **Goals** y **Save** (oscurecidos, para las siguientes fases),
    **Test (F12)** y **Exit**.
  - **Panel derecho:** pestañas **Units**, **Towns**, **Buildings** y **Nature** (por
    ahora solo con un texto de lo que vendrá) y lo seleccionado en el mapa (nombre y
    reino). Clic derecho o Esc quita la selección.
  - **Test (F12):** guarda una copia (`EDITOR.TMP`, en la carpeta de configuración del
    juego) y juega el escenario normal. **F12** o **Return to Editor** (en el lugar de
    "Training" en el menú del juego) regresan al editor tal como estaba antes de probar.
  - **Exit:** pregunta y regresa al menú principal.
  - En el editor, el menú del juego desactiva Save Game, Load Game, Training y Retire.
    Durante la prueba se desactiva Load Game.
  - El menú principal muestra "7kmod-editor 0.1.0 (7kmod 0.1.0)".
