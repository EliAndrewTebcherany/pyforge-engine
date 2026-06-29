from setuptools import setup, Extension, find_packages

pyforge_backend = Extension(
    'pyforge.pyforge_core',
    sources=['src/core.c', 'src/text.c', 'src/effects.c'], 
    libraries=['glfw', 'GL', 'openal', 'm'],
)

setup(
    name="pyforge-engine",
    version="0.2.0",
    author="Eli Andrew Tebcherany",
    packages=find_packages(),
    ext_modules=[pyforge_backend],
    # Automatically downloads Pillow so your users don't need local files
    install_requires=[
        "Pillow>=10.0.0"
    ],
)
