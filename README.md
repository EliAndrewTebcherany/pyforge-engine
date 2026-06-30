# ⚡ PyForge Engine (v1.5.0-alpha)

A high-performance, state-aware 2D graphics rendering engine built using a **Modern OpenGL 3.3 Core Profile** backend wrapped in a lightweight, thin Python exposure interface. 

---

## 🚧 Work in Progress (Not Finished)
**CRITICAL NOTICE:** This engine is currently undergoing a massive, complete architectural rewrite from scratch. Many legacy features (like the old Python vector scripts) have been intentionally stripped out to eliminate bloat. The rendering engine is fully operational, but high-level developer modules and scene systems are still under active development.

---

## 🏎️ Performance & Telemetry Baseline
* **Framerate Speed:** ~1,800 to 5,500+ Unthrottled FPS (Tested on RTX 4050 / Intel Core i5 LOQ Laptop Platform)
* **CPU Loop Overhead:** <0.60ms per frame pass (Zero Python runtime context-switching latency)
* **Memory Footprint:** Clean, locked static allocation footprint with zero heap fragmentation leaks.

---

## 🛠️ The Architecture Shift (What changed?)
PyForge has transitioned from a slow, immediate-mode prototype into a modern, hardware-accelerated batch pipeline:
* 🚫 **Banned OpenGL 1.0:** Completely deleted all deprecated immediate functions (`glBegin()`, `glEnd()`, fixed matrix modes).
* 🛡️ **Enforced Core Profile:** Runs entirely inside a strict OpenGL 3.3 Core Profile sandbox context.
* 📦 **GPU Instanced Array Streaming:** Replaced individual draw loops with high-speed memory block buffer updates (`glDrawArraysInstanced`) sent over the C bridge exactly once per frame.
* 🔄 **Hardware Delegated Rotations:** Offloaded intense trigonometric physics math (`sin`/`cos` transformation matrices) straight onto parallel GPU vertex shader threads.

---

## 💻 Quick Installation & Local Build
Initialize a completely clean virtual environment sandbox and build the native C extension module:

```bash
python3 -m venv venv
source venv/bin/activate
pip install --upgrade pip setuptools wheel psutil  # psutil is used strictly for the test telemetry frame counters
pip install -e .
```

---

## 📖 API Reference & Core Functions
PyForge 1.5 exposes a clean, thin wrapper layer. Developers can handle standard game state logic in Python, while the C backend handles the heavy lifting.

| Function | Arguments | Description |
| :--- | :--- | :--- |
| `pyforge.init()` | `width: int`, `height: int`, `title: str` | Creates the GLFW viewport and binds a strict OpenGL 3.3 Core Profile context. |
| `pyforge.init_buffers()` | *None* | Pre-bakes primitive vertex configurations (Triangle, Quad, Circle) into VRAM memory once at startup. |
| `pyforge.set_vsync()` | `enabled: bool` | `True` locks frame intervals to monitor refresh rates. `False` completely unlocks raw GPU speeds. |
| `pyforge.draw_triangle()`| `x`, `y`, `size`, `angle`, `rot_speed`, `color: tuple` | Batches a precise, hardware-instanced 3-sided primitive into the current frame queue. |
| `pyforge.draw_quad()` | `x`, `y`, `size`, `angle`, `rot_speed`, `color: tuple` | Batches an optimized, triangulated square instance into the frame queue. |
| `pyforge.draw_circle()` | `x`, `y`, `size`, `angle`, `rot_speed`, `color: tuple` | Batches a high-fidelity, 32-sided triangulated circle instance into the frame queue. |
| `pyforge.display_frame()`| *None* | Flushes all accumulated instance queues over the C bridge in one single burst and swaps buffers. |
| `pyforge.is_open()` | *None* | Polls operating system hardware tracking streams and returns window status lifecycle states. |

---

## 🎮 Running the Showcase (Full API Demo)
Create a file named `test_showcase.py` and drop the following code inside it to test every function across your local hardware configuration:

```python
import pyforge
import random
import time
import os
import psutil

# 1. pyforge.init() -> Create the window and bind strict modern core context
WIDTH, HEIGHT = 1024, 768
pyforge.init(WIDTH, HEIGHT, "PyForge 1.5 - Complete Core Feature Showcase")

# 2. pyforge.set_vsync() -> Completely unlock raw GPU hardware speeds
pyforge.set_vsync(False)  

# 3. pyforge.init_buffers() -> Pre-bake all 3 mesh types into VRAM once
pyforge.init_buffers()

print("🎮 Generating 100 showcase shapes with diverse rotational parameters...")
process = psutil.Process(os.getpid())

entities = []
for i in range(100):
    shape_type = random.choice(["triangle", "quad", "circle"])
    
    # Configure multiple rotation speed variations to test hardware threads
    if i % 3 == 0:
        rot_speed = 0.0                      # Stands completely still
        base_angle = random.uniform(0.0, 360.0) # Locked initial tilt
    elif i % 3 == 1:
        rot_speed = random.uniform(0.4, 1.2)   # Automatic Clockwise spin
        base_angle = 0.0
    else:
        rot_speed = random.uniform(-1.2, -0.4) # Automatic Counter-Clockwise spin
        base_angle = 0.0

    entities.append({
        "x": random.randint(100, WIDTH - 100),
        "y": random.randint(100, HEIGHT - 100),
        "size": random.uniform(50.0, 120.0),   
        "base_angle": base_angle,
        "rot_speed": rot_speed,
        "vx": random.uniform(-0.5, 0.4),       
        "vy": random.uniform(-0.4, 0.5),
        "color": (random.random(), random.random(), random.random()),
        "type": shape_type
    })

frame_count = 0
last_metrics_time = time.time()

# 4. pyforge.is_open() -> Loop while window thread remains active
while pyforge.is_open():
    for e in entities:
        e["x"] += e["vx"]
        e["y"] += e["vy"]

        if e["x"] <= 50 or e["x"] >= WIDTH - 50: e["vx"] *= -1
        if e["y"] <= 50 or e["y"] >= HEIGHT - 50: e["vy"] *= -1

        # 5. pyforge.draw_*() -> Accumulate visual parameters inside batch structures
        if e["type"] == "triangle":
            pyforge.draw_triangle(e["x"], e["y"], e["size"], e["base_angle"], e["rot_speed"], e["color"])
        elif e["type"] == "quad":
            pyforge.draw_quad(e["x"], e["y"], e["size"], e["base_angle"], e["rot_speed"], e["color"])
        elif e["type"] == "circle":
            pyforge.draw_circle(e["x"], e["y"], e["size"], e["base_angle"], e["rot_speed"], e["color"])

    # 6. pyforge.display_frame() -> Single burst copy streaming flush to VRAM
    pyforge.display_frame()

    frame_count += 1
    current_time = time.time()
    if current_time - last_metrics_time >= 1.0:
        elapsed = current_time - last_metrics_time
        actual_fps = frame_count / elapsed
        
        cpu_pct = psutil.cpu_percent()
        ram_mb = process.memory_info().rss / (1024 * 1024)
        
        print("\n" + "="*55)
        print("⚡ PYFORGE 1.5 CORE ENGINE SHOWCASE TELEMETRY")
        print(f"  Live Rendering Speed : {actual_fps:.1f} FPS")
        print(f"  System CPU Thread Load: {cpu_pct}%")
        print(f"  Process RAM Footprint: {ram_mb:.2f} MB")
        print("="*55)
        
        frame_count = 0
        last_metrics_time = current_time
```
