#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <GLFW/glfw3.h>

extern GLFWwindow* global_window;

// Stub hooks matching Python expectations to avoid build linkage errors
PyObject* method_load_font_sheet(PyObject* self, PyObject* args) {
    Py_RETURN_NONE;
}

PyObject* method_draw_text(PyObject* self, PyObject* args) {
    Py_RETURN_NONE;
}
