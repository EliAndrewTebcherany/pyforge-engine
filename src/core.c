#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <stdio.h>

// Master window context pointer
extern PyObject* method_load_font_sheet(PyObject* self, PyObject* args);
extern PyObject* method_draw_text(PyObject* self, PyObject* args);
extern PyObject* method_init_audio_hardware(PyObject* self, PyObject* args);
extern PyObject* method_play_sound_file(PyObject* self, PyObject* args);      // Added
extern PyObject* method_play_music_file(PyObject* self, PyObject* args);      // Added
extern PyObject* method_spawn_burst_effect(PyObject* self, PyObject* args);
extern PyObject* method_update_and_render_particles(PyObject* self, PyObject* args);

GLFWwindow* global_window = NULL;



// 1. Pyforge.init(width, height, title)
static PyObject* method_init(PyObject* self, PyObject* args) {
    int width, height;
    const char* title;
    if (!PyArg_ParseTuple(args, "iis", &width, &height, &title)) return NULL;
    if (!glfwInit()) {
        PyErr_SetString(PyExc_RuntimeError, "Could not initialize GLFW");
        return NULL;
    }
    global_window = glfwCreateWindow(width, height, title, NULL, NULL);
    if (!global_window) {
        glfwTerminate();
        PyErr_SetString(PyExc_RuntimeError, "Could not create GLFW window");
        return NULL;
    }
    glfwMakeContextCurrent(global_window);
    
    // Configure an orthogonal 2D screen viewport (0,0 mapping to top-left corner)
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, height, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    Py_RETURN_NONE;
}

// 2. Pyforge.shape(sides)
static PyObject* method_shape(PyObject* self, PyObject* args) {
    int sides;
    if (!PyArg_ParseTuple(args, "i", &sides)) return NULL;
    if (sides < 3) {
        PyErr_SetString(PyExc_ValueError, "Shape must have >= 3 sides");
        return NULL;
    }
    PyObject* point_list = PyList_New(0);
    double angle_step = (2.0 * M_PI) / sides;
    for (int i = 0; i < sides; i++) {
        double current_angle = i * angle_step;
        PyObject* point_tuple = Py_BuildValue("(dd)", cos(current_angle), sin(current_angle));
        PyList_Append(point_list, point_tuple);
        Py_DECREF(point_tuple);
    }
    return point_list;
}

// 3. Pyforge.load_texture(raw_bytes, width, height)
static PyObject* method_load_texture(PyObject* self, PyObject* args) {
    const char* bytes;
    Py_ssize_t len;
    int width, height;
    if (!PyArg_ParseTuple(args, "y#ii", &bytes, &len, &width, &height)) return NULL;

    GLuint texture_id;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    return Py_BuildValue("i", texture_id);
}

// 4. Pyforge.drawshape(shape, x, y, radius, angle, r, g, b, opacity, texture_id)
static PyObject* method_drawshape(PyObject* self, PyObject* args) {
    PyObject* point_list;
    double x, y, radius, angle;
    float r, g, b, alpha;
    int texture_id;

    if (!PyArg_ParseTuple(args, "Oddddffffi", &point_list, &x, &y, &radius, &angle, &r, &g, &b, &alpha, &texture_id)) {
        return NULL;
    }
    if (!global_window || glfwWindowShouldClose(global_window)) Py_RETURN_NONE;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (texture_id > 0) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texture_id);
    }

    glPushMatrix(); 
    glTranslated(x, y, 0.0);
    glRotated(angle, 0.0, 0.0, 1.0);
    glColor4f(r, g, b, alpha);
    
    glBegin(GL_POLYGON);
    Py_ssize_t num_points = PyList_Size(point_list);
    for (Py_ssize_t i = 0; i < num_points; i++) {
        PyObject* point_tuple = PyList_GetItem(point_list, i);
        double local_x, local_y;
        if (!PyArg_ParseTuple(point_tuple, "dd", &local_x, &local_y)) {
            glEnd();
            glPopMatrix();
            if (texture_id > 0) glDisable(GL_TEXTURE_2D);
            glDisable(GL_BLEND);
            return NULL;
        }

        if (texture_id > 0) {
            double u = (local_x + 1.0) * 0.5;
            double v = (local_y + 1.0) * 0.5;
            glTexCoord2d(u, v);
        }

        glVertex2d(local_x * radius, local_y * radius);
    }
    glEnd();
    
    glPopMatrix(); 
    if (texture_id > 0) glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    Py_RETURN_NONE;
}

// 5. Pyforge.draw_texture(texture_id, x, y, w, h, src_x, src_y, src_w, src_h, tex_w, tex_h)
static PyObject* method_draw_texture(PyObject* self, PyObject* args) {
    int texture_id;
    double x, y, w, h;
    double src_x, src_y, src_w, src_h;
    double tex_w, tex_h;

    if (!PyArg_ParseTuple(args, "idddddddddd", &texture_id, &x, &y, &w, &h, &src_x, &src_y, &src_w, &src_h, &tex_w, &tex_h)) return NULL;
    if (!global_window) Py_RETURN_NONE;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
        glTexCoord2d(src_x / tex_w, src_y / tex_h);
        glVertex2d(x, y);

        glTexCoord2d((src_x + src_w) / tex_w, src_y / tex_h);
        glVertex2d(x + w, y);

        glTexCoord2d((src_x + src_w) / tex_w, (src_y + src_h) / tex_h);
        glVertex2d(x + w, y + h);

        glTexCoord2d(src_x / tex_w, (src_y + src_h) / tex_h);
        glVertex2d(x, y + h);
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    Py_RETURN_NONE;
}

// 6. Pyforge.clear_gradient(r1, g1, b1, r2, g2, b2)
static PyObject* method_clear_gradient(PyObject* self, PyObject* args) {
    float r1, g1, b1, r2, g2, b2;
    if (!PyArg_ParseTuple(args, "ffffff", &r1, &g1, &b1, &r2, &g2, &b2)) return NULL;
    if (!global_window) Py_RETURN_NONE;
    int w, h;
    glfwGetWindowSize(global_window, &w, &h);
    glBegin(GL_QUADS);
        glColor3f(r1, g1, b1);   glVertex2d(0, 0);
        glColor3f(r1, g1, b1);   glVertex2d(w, 0);
        glColor3f(r2, g2, b2);   glVertex2d(w, h);
        glColor3f(r2, g2, b2);   glVertex2d(0, h);
    glEnd();
    Py_RETURN_NONE;
}

// 7. Pyforge.is_key_pressed(key_id)
static PyObject* method_is_key_pressed(PyObject* self, PyObject* args) {
    int key_id;
    if (!PyArg_ParseTuple(args, "i", &key_id)) return NULL;
    if (!global_window) Py_RETURN_FALSE;
    if (glfwGetKey(global_window, key_id) == GLFW_PRESS) Py_RETURN_TRUE;
    Py_RETURN_FALSE;
}

// 8. Pyforge.is_open()
static PyObject* method_is_open(PyObject* self, PyObject* args) {
    if (global_window && !glfwWindowShouldClose(global_window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        glfwPollEvents();
        Py_RETURN_TRUE;
    }
    Py_RETURN_FALSE;
}

// 9. Pyforge.refresh()
static PyObject* method_refresh(PyObject* self, PyObject* args) {
    if (global_window) glfwSwapBuffers(global_window);
    Py_RETURN_NONE;
}

// 10. Pyforge.get_mouse_pos()
static PyObject* method_get_mouse_pos(PyObject* self, PyObject* args) {
    if (!global_window) {
        return Py_BuildValue("(dd)", 0.0, 0.0);
    }
    double mx, my;
    glfwGetCursorPos(global_window, &mx, &my);
    return Py_BuildValue("(dd)", mx, my); 
}

// 11. Pyforge.is_mouse_pressed(button_id)
static PyObject* method_is_mouse_pressed(PyObject* self, PyObject* args) {
    int button_id;
    if (!PyArg_ParseTuple(args, "i", &button_id)) return NULL;
    if (!global_window) Py_RETURN_FALSE;
    if (glfwGetMouseButton(global_window, button_id) == GLFW_PRESS) {
        Py_RETURN_TRUE;
    }
    Py_RETURN_FALSE;
}

// Complete binding map table
static PyMethodDef PyforgeMethods[] = {
    {"init", method_init, METH_VARARGS, "Initializes viewport context configurations."},
    {"shape", method_shape, METH_VARARGS, "Generates shape relative node layouts."},
    {"load_texture", method_load_texture, METH_VARARGS, "Uploads raw image data allocations directly to VRAM."},
    {"drawshape", method_drawshape, METH_VARARGS, "Renders vector structures using color paths or texture states."},
    {"draw_texture", method_draw_texture, METH_VARARGS, "Slices and renders custom sheet coordinates."},
    {"clear_gradient", method_clear_gradient, METH_VARARGS, "Fills background using two-color linear blending."},
    {"is_key_pressed", method_is_key_pressed, METH_VARARGS, "Polls keyboard hardware clicks."},
    {"get_mouse_pos", method_get_mouse_pos, METH_VARARGS, "Gets the current cursor (x, y) coordinates."},
    {"is_mouse_pressed", method_is_mouse_pressed, METH_VARARGS, "Checks if a specific mouse button is held down."},
    {"is_open", method_is_open, METH_VARARGS, "Tracks screen availability loop statuses."},
    {"refresh", method_refresh, METH_VARARGS, "Flushes graphic pipelines output buffers."},
    {"load_font_sheet", method_load_font_sheet, METH_VARARGS, "Compiles alpha glyph assets into VRAM font structures."},
    {"draw_text", method_draw_text, METH_VARARGS, "Hardware textures true-type text strings via sprite sheet atlas mapping."},
    {"init_audio_hardware", method_init_audio_hardware, METH_VARARGS, "Hooks sound cards components."},
    {"play_sound_file", method_play_sound_file, METH_VARARGS, "Decodes and triggers short uncompressed local WAV sound effect track layers."},
    {"play_music_file", method_play_music_file, METH_VARARGS, "Streams background ambient local WAV audio tracks with infinite looping parameters."},
    {"spawn_burst_effect", method_spawn_burst_effect, METH_VARARGS, "Spawns visual geometric pixel explosion fragments."},
    {"update_and_render_particles", method_update_and_render_particles, METH_VARARGS, "Updates simulation states maps matrices physics."},


    {NULL, NULL, 0, NULL} 
};

// Module setup description
static struct PyModuleDef pyforgemodule = {
    PyModuleDef_HEAD_INIT, "pyforge_core", "High performance geometry core.", -1, PyforgeMethods
};

// Execution initialization entry hook pointer
PyMODINIT_FUNC PyInit_pyforge_core(void) {
    return PyModule_Create(&pyforgemodule);
}
