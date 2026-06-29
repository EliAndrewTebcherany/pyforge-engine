PYTHON = python3
VENV = .venv
BIN = $(VENV)/bin

.PHONY: all setup clean run

# Default target runs when you just type 'make'
all: setup

# 1. Automated Virtual Environment initialization and extension compilation
setup:
	@if [ ! -d "$(VENV)" ]; then \
		echo "No environment found. Creating isolated Python Virtual Environment sandbox..."; \
		$(PYTHON) -m venv $(VENV); \
		echo "Upgrading base packaging tools..."; \
		$(BIN)/pip install --upgrade pip setuptools wheel; \
		echo "Compiling C extensions and installing pyforge-engine with dependencies..."; \
		$(BIN)/pip install -e .; \
	else \
		echo "Environment already exists. Ready to run."; \
	fi
	@echo "=========================================================="
	@echo "✅ Pyforge compiled and installed successfully!"
	@echo "To work manually in this workspace, type: source .venv/bin/activate"
	@echo "Or run your game test instantly by typing: make run"
	@echo "=========================================================="

# 2. Automated testing runner (automatically triggers setup if .venv is missing)
run: setup
	$(BIN)/python3 test_game.py

# 3. Clean up build artifacts, caches, and the environment folder
clean:
	rm -rf $(VENV)
	rm -rf *.egg-info
	rm -rf build/
	rm -rf dist/
	find . -type d -name "__pycache__" -exec rm -rf {} +
	clear
	@echo "🧹 Environment wiped clean."
