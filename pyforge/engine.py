from . import pyforge_core
import time

_render_batch = []
_last_frame_time = time.perf_counter()

def init(width: int, height: int, title: str):
    pyforge_core.init(int(width), int(height), str(title))

def init_buffers():
    pyforge_core.init_buffers()

def set_vsync(enabled: bool):
    toggle = 1 if enabled else 0
    pyforge_core.set_vsync(toggle)

def is_open() -> bool:
    return pyforge_core.is_open()

# --- 🏎️ EXPANDED 8-VARIABLE ACCUMULATORS ---

def draw_triangle(x: float, y: float, size: float, angle: float, rotation_speed: float, color: tuple):
    _render_batch.append((float(x), float(y), float(size), float(angle), float(rotation_speed), float(color[0]), float(color[1]), float(color[2]), 0.0))

def draw_quad(x: float, y: float, size: float, angle: float, rotation_speed: float, color: tuple):
    _render_batch.append((float(x), float(y), float(size), float(angle), float(rotation_speed), float(color[0]), float(color[1]), float(color[2]), 1.0))

def draw_circle(x: float, y: float, size: float, angle: float, rotation_speed: float, color: tuple):
    _render_batch.append((float(x), float(y), float(size), float(angle), float(rotation_speed), float(color[0]), float(color[1]), float(color[2]), 2.0))

def display_frame():
    global _render_batch, _last_frame_time
    
    # ⏱️ Automatic delta calculation ensures smooth animation regardless of frame spikes
    current_time = time.perf_counter()
    dt = current_time - _last_frame_time
    _last_frame_time = current_time

    if _render_batch:
        pyforge_core.draw_batch(_render_batch)
        _render_batch.clear()
        
    pyforge_core.refresh(dt) # Stream delta frame timing parameters down to the C core clock
