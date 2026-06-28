from setuptools import setup, Extension, find_packages

pyforge_backend = Extension(
    'pyforge.pyforge_core',
    # Added your new effects compilation source file
    sources=['src/core.c', 'src/text.c', 'src/effects.c'], 
    # Linked openal directly to hook native audio hardware pipelines
    libraries=['glfw', 'GL', 'openal', 'm'],
)

setup(
    name="Pyforge",
    version="0.2.0",
    author="Eli Andrew Tebcherany",
    packages=find_packages(),
    ext_modules=[pyforge_backend],
)
