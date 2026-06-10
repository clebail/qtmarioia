# qtmarioia — Contexte projet

## Objectif
Construire une IA qui joue à un jeu SNES via snes9x. Ce dépôt est le point de départ : il crée un gamepad virtuel Linux que l'IA pourra piloter programmatiquement.

---

## Ce qui est fait

### Gamepad virtuel (`gamepad.cpp` / `gamepad.h`)
- Crée un périphérique d'entrée virtuel via le module kernel `uinput` (bibliothèque `suinput`)
- Boutons enregistrés : `BTN_A`, `BTN_B`, `BTN_X`, `BTN_Y`, `BTN_TL`, `BTN_TR`, `BTN_START`, `BTN_SELECT`, `BTN_DPAD_UP/DOWN/LEFT/RIGHT`
- Axes enregistrés : `ABS_X`, `ABS_Y`, `ABS_RX`, `ABS_RY` (range ±512)
- Le périphérique apparaît sous `/dev/input/js*` et `/dev/input/event*`

### API Gamepad
- `sendPress(__u16 code)` — envoie uniquement le press (EV_KEY=1 + SYN)
- `sendRelease(__u16 code)` — envoie uniquement le release (EV_KEY=0 + SYN)
- Tous les boutons (face + croix) utilisent press/release — le personnage réagit au maintien

### Interface Qt (`mainwindow.cpp`)
- Boutons face et croix : `sendPress`/`sendRelease` via signaux `pressed`/`released`
- Checkboxes Auto A / Auto B : envoient press+release toutes les 300ms via un `QTimer`
- Bouton "Snes9x" : recherche la fenêtre snes9x par titre via X11 (`_NET_WM_NAME`)
- Label `lblCapture` : affiche le flux vidéo capturé, mis à l'échelle au label (ratio conservé)

### Capture vidéo (`mainwindow.cpp`)
- Capture via **XComposite** (`XCompositeRedirectWindow` + `XCompositeNameWindowPixmap` + `XGetImage`)
- Rafraîchissement toutes les 33ms (~30fps) via `QTimer`
- Conversion `XImage → QImage → QPixmap` affichée dans `lblCapture`
- `Display*` ouvert une seule fois dans le constructeur (`captureDpy`), fermé dans le destructeur
- `XCompositeRedirectWindow` appelé une seule fois dans `on_pbSnes9x_clicked()`
- **CPU encore élevé (~12.6%)** : `XGetImage` copie toute la pixmap via socket X11 à chaque frame — prochain fix : **MIT-SHM (`XShmGetImage`)** et/ou vérifier la résolution capturée vs 256×224 natif SNES

### Points techniques importants
- **Press + release dans le même sync frame = input ignoré par SDL/snes9x** — toujours séparer par un `EV_SYN`
- **Les directions doivent utiliser `BTN_DPAD_*` (EV_KEY)** et non `EV_ABS`
- **Environnement Wayland** : lancer snes9x avec `GDK_BACKEND=x11` et l'app avec `QT_QPA_PLATFORM=xcb` — sinon `grabWindow` retourne noir
- **XComposite obligatoire** sous XWayland : `grabWindow` et `XGetImage` directs retournent noir même en mode software
- **`/dev/uinput` permissions** : ajouter l'utilisateur au groupe `input` + règle udev `KERNEL=="uinput", MODE="0660", GROUP="input"` dans `/etc/udev/rules.d/99-uinput.rules`
- **Conflit X11/Qt dans les headers** : inclure `<X11/Xlib.h>` uniquement dans les `.cpp`, jamais dans les `.h` (conflit sur `Bool`, `Status`, etc.)

### Build snes9x 1.53
Correctifs appliqués pour compiler avec GCC 14 / C++17 :
- `jma/jma.h` + `jma/jma.cpp` : suppression des `throw(jma_errors)` (dynamic exception specs supprimées en C++17)
- `jma/crc32.h` : suppression du mot-clé `register` (supprimé en C++17)
- `conffile.h` + `conffile.cpp` : ajout de `const` sur les `operator()` des comparateurs `section_then_key_less` et `line_less`
- `unzip/crc32.h`, `unzip/unzip.c`, `unzip/zip.c`, `unzip/crypt.h` : `pcrc_32_tab` passé de `unsigned long*` à `unsigned int*` (incompatibilité `z_crc_t` sur 64 bits)

---

## Ce qui reste à faire

### Prochaine session : IA de suivi du personnage
Objectif : l'utilisateur montre le personnage principal une fois (ex: clic sur sa position dans le label de capture), et l'IA le suit automatiquement en envoyant les bonnes commandes au gamepad.

Pistes techniques :
- **Détection par template matching** : capturer une ROI autour du personnage au moment du clic, puis chercher cette ROI dans chaque frame (OpenCV `matchTemplate`)
- **Suivi par corrélation** : plus robuste aux changements de frame (CSRT, KCF trackers d'OpenCV)
- **Logique de déplacement** : comparer la position du personnage détecté avec un point cible → envoyer les touches directionnelles correspondantes

Dépendances à ajouter : OpenCV (`libopencv-dev`)

### Boucle temps-réel
Pour un RL futur, il faudra :
- Piloter snes9x en mode headless ou via son API (savestates pour reset d'épisode)
- Tourner à ~60fps en lockstep avec l'émulateur

---

## Fichiers clés
| Fichier | Rôle |
|---------|------|
| `gamepad.h/cpp` | Création et pilotage du gamepad virtuel |
| `suinput.c/h` | Wrapper libsuinput autour de uinput |
| `mainwindow.h/cpp` | Interface Qt : gamepad + capture vidéo |
| `mainwindow.ui` | Layout Qt Designer |
