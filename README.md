# 🛠️ pyforge-engine (v0.2.0)

> [!WARNING]
> **N.B. Known Limitation (v0.2.0 Stable Architecture Note):** 
> The Vector Physics Module (`pyforge.phygroup`) is currently undergoing internal namespace optimization. Shape entity generations (`pyforge.shape()`) may temporarily pass un-wrapped primitive data buffers back to the execution subsystem, causing an `AttributeError: 'list' object has no attribute 'get_physics'` under specific configurations. 
> 
> **Workaround for Devs:** If you are testing or writing complex structural multi-body elastic collision sandboxes right now, manually declare an object or class wrapper wrapper over the returned shape matrices to explicitly pass `current_x`, `current_y`, `mass`, and `velocity` properties smoothly down to your local runner pipelines. This will be completely patched inside an upcoming automated cross-platform wheel distribution release.


A blazing-fast, cross-platform 2D game engine framework combining a low-level compiled **C/OpenGL core** with a clean, high-utility **Python API**.

Unlike software-rendered libraries like Pygame which process graphics sequentially on individual CPU threads, `pyforge-engine` processes matrix math directly within hardware-accelerated **GPU layers**. Version 0.2.0 introduces a modular, opt-in **Vector Physics Subsystem**, enabling smooth linear momentum tracking, elastic object-to-object collisions, and dynamic screen boundary containment without sacrificing performance.

---

## 📖 Global Function Reference Guide

The entire framework operates under a single unified coordination matrix grid, mapping `(0,0)` straight to the top-left viewport boundary corner.

| Function Interface Signature | Execution Subsystem | Operational Responsibility Description |
| :--- | :--- | :--- |
| `pyforge.init(width, height, title)` | `core.c` | Initializes the GLFW graphics hardware context, configures an orthogonal 2D screen viewport grid, and boots up OpenAL audio mixers. |
| `pyforge.shape(sides)` | `core.c` / `engine.py` | Returns an `EngineShape` object containing pre-calculated coordinate node layout arrays to generate regular polygons on the GPU. |
| `pyforge.load_image(file_path)` | `engine.py` | Decodes image assets using Pillow and transfers raw byte buffers directly to VRAM texture slots completely offline. |
| `pyforge.clear_gradient(top_color, bottom_color)` | `core.c` | Wipes the active graphics pipeline frame backbuffer and paints a vertical linear gradient blend using raw RGB parameters. |
| `pyforge.drawshape(shape, x, y, size, angle, color, opacity, texture)` | `core.c` | Renders a polygon mesh array using GPU hardware matrix transformations. If physics is enabled on the object, it automatically overrides `x` and `y` inputs with its live calculated physical vector positions. |
| `pyforge.draw_button(x, y, w, h, bg_color)` | `engine.py` | Renders a bounded, flat-color rectangular UI panel box using direct absolute corner quad mesh transformations. |
| `pyforge.load_system_font(font_name, size)` | `engine.py` | Automatically queries host operating system directories to map, draw, and compress font tiles entirely in memory. |
| `pyforge.draw_text(text, x, y, scale, color)` | `engine.py` | Draws string phrases onto active hardware slots by rendering pre-sliced, fileless texture quad maps from the RAM font matrix. |
| `pyforge.get_mouse_pos()` | `core.c` | Polls the native hardware pointer position and returns the absolute cursor location coordinates as an `(x, y)` tuple. |
| `pyforge.is_mouse_pressed(button)` | `core.c` | Queries your system event loop queues to check if the left mouse click button is actively pressed. |
| `pyforge.is_button_clicked(x, y, w, h)` | `engine.py` | Performs real-time bounding box intersection tests to see if a mouse click event matches a specific rectangular coordinate perimeter. |
| `pyforge.is_key_pressed(key_code)` | `core.c` | Polls the active keyboard state directly via GLFW to monitor key interaction triggers instantly. |
| `pyforge.play_sound(file_path)` | `effects.c` | Decodes a short uncompressed local `.wav` sound effect track and routes it to an OpenAL audio channel. |
| `pyforge.play_music(file_path, loop)` | `effects.c` | Intercepts `.mp3` or `.wav` music tracks, decodes them filelessly inside RAM using `miniaudio`, and streams them continuously. |
| `pyforge.spawn_particles(x, y, color)` | `effects.c` | Allocates and spawns an explosive burst array of custom physics-tracked pixel explosion particle fragments on the GPU. |
| `pyforge.update_effects()` | `effects.c` | Computes physics velocities and fades out active particle transparency matrices during loop iterations. |
| `pyforge.phygroup(*entities)` | `vectors.py` | **[NEW]** Instantiates a `PhysicsGroup` container linking multiple shapes together to automatically compute mutual elastic momentum transfers and border wall reflections. |
| `pyforge.get_fps()` | `engine.py` | Computes high-precision frame rate diagnostics by tracking system time deltas between active rendering loops. |
| `pyforge.refresh()` | `core.c` | Swaps the backbuffer frame data onto physical desktop monitors to lock drawing cycles tightly with user displays. |
| `pyforge.is_open()` | `core.c` | Tracks the visibility and availability metrics of the active canvas viewport window to control main application loops. |

---

## 🧮 Physics Subsystem Architecture

Shapes do not process physics unless explicitly requested. Calling `.get_physics("custom")` transforms an regular shape into a physics-tracked entity carrying the following parameters:

```python
# Enable the physics intercept pipeline layout
my_shape.get_physics("custom")

# State Variable Configurations
my_shape.gravity = 150.0   # Continuous vertical downward acceleration force vector
my_shape.mass = 5000.0     # Resistance magnitude against incoming collision impulses
my_shape.velocity = -250.0 # Active directional vector velocity tracking step
my_shape.current_x = 512.0 # Manual local baseline horizontal tracking coordinate 
my_shape.current_y = 150.0 # Manual local baseline vertical tracking coordinate
```

---

## 🛠️ Feature Verification Test Scripts

Developers can copy and run these compact sandboxed files to instantly test and verify features on their local machine.

### Test 1: Geometry Matrix & Colors
Draws a cluster of spinning polygons showcasing the hardware matrix transform pipelines.

```python
import pyforge

pyforge.init(1024, 768, "Pyforge Engine - Geometric Cluster Showcase")

triangle = pyforge.shape(3)
square = pyforge.shape(4)
pentagon = pyforge.shape(5)
circle = pyforge.shape(36)

angle = 0.0

while pyforge.is_open():
    pyforge.clear_gradient((0.02, 0.02, 0.05), (0.08, 0.08, 0.15))
    angle += 1.0

    pyforge.drawshape(triangle, x=200, y=384, size=80, angle=angle, color=(0.9, 0.2, 0.2))
    pyforge.drawshape(square, x=400, y=384, size=80, angle=-angle * 0.5, color=(1.0, 0.6, 0.1))
    pyforge.drawshape(pentagon, x=600, y=384, size=80, angle=angle * 1.5, color=(0.2, 0.8, 0.4))
    pyforge.drawshape(circle, x=800, y=384, size=80, angle=0.0, color=(0.2, 0.5, 0.9))

    pyforge.refresh()
```

### Test 2: Head-On Kinetic Interactions & Vector Matrix
**[UPDATED v0.2.0]** Sets up a high-precision head-on collision matrix demonstrating Newton's Third Law (Conservation of Linear Momentum), dynamic screen boundaries, and custom particle explosion clouds.

```python
import pyforge

pyforge.init(1024, 768, "Pyforge v1.0.0 - Full Physics Sandbox Loop")
pyforge.load_system_font("sans-serif", font_size=20)

# =====================================================================
# 🔴 TOP BALLS ROW (Moving Downward)
# =====================================================================
top_light = pyforge.shape(32)
top_light.get_physics("custom")
top_light.gravity = 0.0  
top_light.mass = 10.0    
top_light.velocity = 80.0  
top_light.current_x = 256.0
top_light.current_y = 150.0

top_heavy = pyforge.shape(32)
top_heavy.get_physics("custom")
top_heavy.gravity = 0.0
top_heavy.mass = 5000.0  
top_heavy.velocity = 80.0
top_heavy.current_x = 512.0
top_heavy.current_y = 150.0

top_equal = pyforge.shape(32)
top_equal.get_physics("custom")
top_equal.gravity = 0.0
top_equal.mass = 500.0   
top_equal.velocity = 80.0
top_equal.current_x = 768.0
top_equal.current_y = 150.0

# =====================================================================
# 🔵 BOTTOM BALLS ROW (Opposite Orientation, Upward Sense)
# =====================================================================
bot_heavy = pyforge.shape(32)
bot_heavy.get_physics("custom")
bot_heavy.gravity = 0.0
bot_heavy.mass = 5000.0     
bot_heavy.velocity = -80.0 
bot_heavy.current_x = 256.0
bot_heavy.current_y = 550.0

bot_light = pyforge.shape(32)
bot_light.get_physics("custom")
bot_light.gravity = 0.0
bot_light.mass = 10.0       
bot_light.velocity = -80.0 
bot_light.current_x = 512.0
bot_light.current_y = 550.0

bot_equal = pyforge.shape(32)
bot_equal.get_physics("custom")
bot_equal.gravity = 0.0
bot_equal.mass = 500.0      
bot_equal.velocity = -80.0 
bot_equal.current_x = 768.0
bot_equal.current_y = 550.0

# =====================================================================
# 👥 COUPLING MATRIX INTERACTION MANAGER
# =====================================================================
physics_manager = pyforge.phygroup(
    top_light, top_heavy, top_equal, 
    bot_heavy, bot_light, bot_equal
)

while pyforge.is_open():
    pyforge.clear_gradient((0.02, 0.02, 0.05), (0.08, 0.08, 0.15))

    # Run boundary reflections and momentum calculation passes simultaneously
    physics_manager.update_interactions(1024, 768)
    pyforge.update_effects()

    # Draw visible dynamic bounding wall boundary lines
    pyforge.draw_button(10, 68, 1004, 2, bg_color=(0.3, 0.3, 0.4))
    pyforge.draw_button(10, 745, 1004, 2, bg_color=(0.3, 0.3, 0.4))

    # Render Left Column (Light vs Heavy)
    pyforge.drawshape(top_light, x=top_light.current_x, y=top_light.current_y, size=30, color=(1.0, 0.3, 0.3))
    pyforge.drawshape(bot_heavy, x=bot_heavy.current_x, y=bot_heavy.current_y, size=30, color=(0.2, 0.6, 1.0))
    pyforge.draw_text("Light vs Heavy", x=180, y=700, scale=0.45, color=(1.0, 1.0, 1.0))

    # Render Center Column (Heavy vs Light)
    pyforge.drawshape(top_heavy, x=top_heavy.current_x, y=top_heavy.current_y, size=30, color=(1.0, 0.3, 0.3))
    pyforge.drawshape(bot_light, x=bot_light.current_x, y=bot_light.current_y, size=30, color=(0.2, 0.6, 1.0))
    pyforge.draw_text("Heavy vs Light", x=440, y=700, scale=0.45, color=(1.0, 1.0, 1.0))

    # Render Right Column (Equal Masses)
    pyforge.drawshape(top_equal, x=top_equal.current_x, y=top_equal.current_y, size=30, color=(1.0, 0.3, 0.3))
        pyforge.drawshape(bot_equal, x=bot_equal.current_x, y=bot_equal.current_y, size=30, color=(0.2, 0.6, 1.0))
    pyforge.draw_text("Equal Masses", x=705, y=700, scale=0.45, color=(1.0, 1.0, 1.0))

    # HUD Interface Overlay Row Banner
    pyforge.draw_button(10, 10, 1004, 50, bg_color=(0.02, 0.02, 0.05))
    pyforge.draw_text(f"PYFORGE INFRASTRUCTURE V1.0.0 RUNNING LIVE | FPS: {pyforge.get_fps()}", x=30, y=22, scale=0.45, color=(1.0, 1.0, 1.0))

    pyforge.refresh()

```

## 🛠️ Code Maintenance & Offline Environment Sandbox

The project includes an automated lifecycle manager utility via the local `Makefile`:

```bash
# Wipes compilation artifact directories and egg-info paths clean
make clean

# Initializes your virtual sandbox environment, updates Pillow, and compiles C extensions
make

# Instantly boots your active test_game.py simulation script frame
make run
```

---

## 🤖 AI Code-Comment Transparency Statement

For transparency and alignment with modern engineering practices, all granular architectural descriptions and functional code documentation blocks written across the `src/core.c`, `src/text.c`, `src/effects.c`, `pyforge/vectors.py`, and `pyforge/engine.py` codebase layout paths were generated with the assistance of artificial intelligence.

These comments provide explanations of the low-level memory translations, buffer byte mappings, elastic collision vectors, and OpenGL/OpenAL hardware pipelines, improving developer readability and long-term module maintenance.

---

*pyforge-engine v0.2.0 — Developed by Eli Andrew Tebcherany. Released under the MIT License.*
