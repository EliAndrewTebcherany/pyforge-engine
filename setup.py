from setuptools import setup, Extension, find_packages

# Configure the modern native C extension module
pyforge_backend = Extension(
    'pyforge.pyforge_core',
    sources=['src/core.c'],
    libraries=['glfw', 'GL', 'm'],  # Links GLFW, OpenGL, and the math library
)

setup(
    name="pyforge-engine",
    version="1.5.0", 
    author="Eli Andrew Tebcherany",
    packages=find_packages(),
    ext_modules=[pyforge_backend],
)
