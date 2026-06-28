#  Contributing to pyforge-engine

First off, thank you for taking the time to contribute! It is developers like you who make `pyforge-engine` a fast, lightweight alternative to Pygame.

## 🗺️ How Can I Contribute?

### 1. Reporting Bugs & Feature Requests
* Open a GitHub Issue describing the bug or performance bottleneck.
* Provide a minimal reproducible Python code snippet using the framework.

### 2. Expanding the Core Engine
We are actively looking for contributors to help optimize or add low-level features:
* **C/OpenGL Layer (`src/`)**: Optimizing particle array iterations, adding native text rotation, or implementing camera matrix translations.
* **Audio Layer (`src/effects.c`)**: Expanding OpenAL features or streamlining multi-track background streams.
* **Python API Wrapper (`pyforge/`)**: Designing automated delta-time clock managers, vector math helpers, or sprite sheet animation layers.

## Local Development Sandbox Setup

1. Fork this repository on GitHub and clone it to your local machine.
2. Initialize your testing sandbox using our automated macro:
   ```bash
   make clean && make
   ```
3. Create a descriptive feature branch (`git checkout -b feature/amazing-physics`).
4. Ensure your test scripts execute flawlessly via `make run` before opening a Pull Request (PR)!

---
*By contributing, you agree that your code will be licensed under the project's existing MIT License.*
