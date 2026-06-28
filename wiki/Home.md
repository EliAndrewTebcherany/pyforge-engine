# 📚 PyForge Engine Wiki Home

Welcome to the **PyForge Engine** documentation hub! This wiki contains comprehensive guides, tutorials, and references to help you get started and master the engine.

---

## 🚀 Quick Start

### New to PyForge?
1. **[Installation Guide](./Installation)** — Set up PyForge on your system
2. **[Basic Game Tutorial](./Basic-Game-Tutorial)** — Build your first game (Space Shooter)
3. **[Function Reference](../README.md#-global-function-reference-guide)** — API documentation

---

## 📖 Documentation

### Getting Started
- **[Installation](./Installation)** — Complete setup instructions for Windows, macOS, and Linux
- **[Basic Game Tutorial](./Basic-Game-Tutorial)** — Step-by-step guide to creating a Space Shooter game
  - Learn game loops, player controls, enemy spawning, and collision detection
  - Progressive examples from basic to advanced features

### Core Concepts
- **[Game Loop Architecture](../README.md#-global-function-reference-guide)** — Understanding the game loop and rendering pipeline
- **[Coordinate System](../README.md#-global-function-reference-guide)** — How PyForge maps coordinates (0,0 = top-left)
- **[Shapes & Rendering](../README.md#-global-function-reference-guide)** — Working with `pyforge.shape()` and `pyforge.drawshape()`

### Advanced Topics
- **[Audio System](../README.md#-global-function-reference-guide)** — Music and sound effects with OpenAL
- **[Particle Effects](../README.md#-global-function-reference-guide)** — Creating visual effects with `pyforge.spawn_particles()`
- **[Text Rendering](../README.md#-global-function-reference-guide)** — Font loading and text drawing
- **[Input Handling](../README.md#-global-function-reference-guide)** — Keyboard and mouse input detection

### Development
- **[Project Maintenance](../README.md#-code-maintenance--offline-environment-sandbox)** — Using the Makefile for build and deployment
- **[Contributing](../CONTRIBUTING.md)** — Guidelines for contributing to PyForge Engine

---

## 🛠️ API Reference

### Quick API Index

| Function | Purpose |
|----------|---------|
| `pyforge.init(width, height, title)` | Initialize the game window |
| `pyforge.shape(sides)` | Create polygon shapes |
| `pyforge.drawshape(shape, x, y, size, angle, color, opacity, texture)` | Render shapes to screen |
| `pyforge.clear_gradient(top_color, bottom_color)` | Clear screen with gradient |
| `pyforge.load_image(file_path)` | Load image textures |
| `pyforge.draw_text(text, x, y, scale, color)` | Render text |
| `pyforge.load_system_font(font_name, size)` | Load system fonts |
| `pyforge.get_mouse_pos()` | Get mouse position |
| `pyforge.is_mouse_pressed(button)` | Check mouse button state |
| `pyforge.is_key_pressed(key_code)` | Check keyboard input |
| `pyforge.play_sound(file_path)` | Play sound effects |
| `pyforge.play_music(file_path, loop)` | Stream music |
| `pyforge.spawn_particles(x, y, color)` | Create particle effects |
| `pyforge.update_effects()` | Update particle physics |
| `pyforge.refresh()` | Update display |
| `pyforge.is_open()` | Check if window is open |

For detailed documentation, see the [Function Reference Guide](../README.md#-global-function-reference-guide).

---

## 🎮 Example Projects

### Space Shooter
A complete beginner-friendly game tutorial featuring:
- Player spaceship control with arrow keys
- Bullet firing system with fire-rate limiting
- Enemy spawning with collision detection
- Score tracking and game over states

**[View Full Tutorial →](./Basic-Game-Tutorial)**

---

## 📝 Code Examples

### Minimal Game Window
```python
import pyforge

pyforge.init(800, 600, "My Game")

while pyforge.is_open():
    pyforge.clear_gradient((0.1, 0.1, 0.1), (0.2, 0.2, 0.2))
    pyforge.refresh()
```

### Drawing Shapes
```python
import pyforge

pyforge.init(800, 600, "Shape Demo")
triangle = pyforge.shape(3)
square = pyforge.shape(4)
circle = pyforge.shape(36)

while pyforge.is_open():
    pyforge.clear_gradient((0.05, 0.05, 0.05), (0.15, 0.15, 0.15))
    
    pyforge.drawshape(triangle, x=200, y=300, size=50, angle=0, 
                     color=(1.0, 0.2, 0.2))
    pyforge.drawshape(square, x=400, y=300, size=50, angle=45, 
                     color=(0.2, 1.0, 0.2))
    pyforge.drawshape(circle, x=600, y=300, size=50, angle=0, 
                     color=(0.2, 0.2, 1.0))
    
    pyforge.refresh()
```

### Keyboard Input
```python
import pyforge

pyforge.init(800, 600, "Input Demo")

player_x = 400
player_y = 300
speed = 5

while pyforge.is_open():
    pyforge.clear_gradient((0.05, 0.05, 0.05), (0.15, 0.15, 0.15))
    
    if pyforge.is_key_pressed(262):  # Left arrow
        player_x -= speed
    if pyforge.is_key_pressed(263):  # Right arrow
        player_x += speed
    if pyforge.is_key_pressed(265):  # Up arrow
        player_y -= speed
    if pyforge.is_key_pressed(264):  # Down arrow
        player_y += speed
    
    square = pyforge.shape(4)
    pyforge.drawshape(square, x=player_x, y=player_y, size=40, 
                     angle=0, color=(0.2, 0.5, 0.9))
    
    pyforge.refresh()
```

---

## 🐛 Troubleshooting

### Common Issues

**Installation Problems?**
→ See [Installation Guide — Troubleshooting](./Installation#troubleshooting)

**Game Not Rendering?**
→ Ensure `pyforge.refresh()` is called at the end of each loop iteration

**Performance Issues?**
→ Check frame rate with `pyforge.get_fps()` and optimize particle counts

**Audio Not Playing?**
→ Verify OpenAL is installed and files are in the correct format (.wav, .mp3)

For more help, check the [GitHub Issues](https://github.com/EliAndrewTebcherany/pyforge-engine/issues).

---

## 📊 Performance Notes

PyForge Engine uses **GPU-accelerated rendering** with OpenGL, making it significantly faster than CPU-bound libraries like Pygame. Key performance features:

- ✅ Hardware matrix transformations
- ✅ GPU-based particle physics
- ✅ Direct VRAM texture uploads
- ✅ Optimized OpenAL audio mixing
- ✅ Low-latency input polling

---

## 🤝 Contributing

Want to improve PyForge Engine? Contributions are welcome!

- **Report bugs** → [GitHub Issues](https://github.com/EliAndrewTebcherany/pyforge-engine/issues)
- **Suggest features** → [GitHub Discussions](https://github.com/EliAndrewTebcherany/pyforge-engine/discussions)
- **Submit pull requests** → See [Contributing Guide](../CONTRIBUTING.md)

---

## 📄 License

PyForge Engine is released under the **MIT License**. See [LICENSE](../LICENSE) for details.

---

## 🔗 Resources

- **[GitHub Repository](https://github.com/EliAndrewTebcherany/pyforge-engine)**
- **[GitHub Issues](https://github.com/EliAndrewTebcherany/pyforge-engine/issues)**
- **[Contributing Guide](../CONTRIBUTING.md)**
- **[README](../README.md)**

---

## 📞 Support

For questions, issues, or feedback:
1. Check this wiki for existing documentation
2. Search [GitHub Issues](https://github.com/EliAndrewTebcherany/pyforge-engine/issues)
3. Create a new issue with details about your problem

---

**Happy game development! 🚀**

*PyForge Engine v0.2.0 — Developed by Eli Andrew Tebcherany*
