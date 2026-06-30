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

## 🎮 Running the Patterns Showcase
Execute the optimized multi-shape instance array pattern streaming test:
```bash
python3 test_showcase.py
```
