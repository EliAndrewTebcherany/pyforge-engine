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
PyForge 2.0 exposes a clean, thin wrapper layer. Developers can handle standard game state logic in Python, while the C backend handles the heavy lifting.

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

