#define GL_GLEXT_PROTOTYPES
#include <Python.h>
#include <GLFW/glfw3.h>
#include <stdbool.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <math.h>

#define MAX_INSTANCES 5000

// --- GLOBAL ENGINE STATE MANAGER TRACKING CONTROLLER ---
typedef struct {
    GLFWwindow* window;
    int screen_width;
    int screen_height;
    GLuint currently_bound_program;
    GLuint currently_bound_vao;
    bool is_initialized;
    float total_time; // Tracks hardware run runtime ticks
} EngineStateManager;

static EngineStateManager EngineState = { NULL, 0, 0, 0, 0, false, 0.0f };

// Global references for our Modern Hardware Buffer layout
GLuint shape_vao = 0;
GLuint base_vertex_vbo = 0;
GLuint instance_data_vbo = 0;
GLuint shader_program = 0;

// Structural layout defining an object's transformations pushed directly to shaders
typedef struct {
    float x, y;
    float size;
    float base_angle;  // The initial static tilt angle
    float rot_speed;   // Custom automatic spin velocity parameter!
    float r, g, b;
    float shape_type; 
} ShapeInstance;

static ShapeInstance instance_cache[MAX_INSTANCES];

// --- CLEAN UN-GLITCHED VERTEX SHADER ---
const char* vertex_shader_source = "#version 330 core\n"
    "layout (location = 0) in vec2 aPos;\n"         // Template geometry points passed from VRAM
    "layout (location = 1) in vec2 aOffset;\n"      // Unique Instance Position (x, y)
    "layout (location = 2) in float aSize;\n"       // Unique Instance Size
    "layout (location = 3) in vec2 aRotParams;\n"   // Packs: x = base_angle, y = rot_speed
    "layout (location = 4) in vec3 aColor;\n"       // Unique Instance Color (r, g, b)
    "layout (location = 5) in float aShapeType;\n"  // State Tracker: 0=Triangle, 1=Square, 2=Circle\n"
    "\n"
    "uniform float u_Time;\n"
    "out vec3 FragColor;\n"
    "\n"
    "void main() {\n"
    "    // Calculate dynamic angles on the fly based on runtime clocks!\n"
    "    float currentAngle = aRotParams.x + (aRotParams.y * u_Time * 50.0);\n"
    "    float angleRad = radians(currentAngle);\n"
    "    \n"
    "    float cosA = cos(angleRad);\n"
    "    float sinA = sin(angleRad);\n"
    "    vec2 rotatedPos = vec2(\n"
    "        (aPos.x * cosA - aPos.y * sinA) * aSize,\n"
    "        (aPos.x * sinA + aPos.y * cosA) * aSize\n"
    "    );\n"
    "    \n"
    "    float absoluteX = rotatedPos.x + aOffset.x;\n"
    "    float absoluteY = rotatedPos.y + aOffset.y;\n"
    "    \n"
    "    float normX = (absoluteX / 1024.0) * 2.0 - 1.0;\n"
    "    float normY = 1.0 - (absoluteY / 768.0) * 2.0;\n"
    "    \n"
    "    gl_Position = vec4(normX, normY, 0.0, 1.0);\n"
    "    FragColor = aColor;\n"
    "}\n";

const char* fragment_shader_source = "#version 330 core\n"
    "in vec3 FragColor;\n"
    "out vec4 color;\n"
    "\n"
    "void main() {\n"
    "    color = vec4(FragColor, 1.0);\n"
    "}\n";


    static PyObject* pyforge_core_init(PyObject* self, PyObject* args) {
    int width, height;
    const char* title;
    if (!PyArg_ParseTuple(args, "iis", &width, &height, &title)) return NULL;
    if (EngineState.is_initialized) Py_RETURN_NONE;

    if (!glfwInit()) {
        PyErr_SetString(PyExc_RuntimeError, "Fatal: Could not initialize GLFW.");
        return NULL;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    EngineState.window = glfwCreateWindow(width, height, title, NULL, NULL);
    if (!EngineState.window) {
        glfwTerminate();
        PyErr_SetString(PyExc_RuntimeError, "Fatal: Failed to create window.");
        return NULL;
    }

    glfwMakeContextCurrent(EngineState.window);
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glViewport(0, 0, width, height);
    EngineState.screen_width = width;
    EngineState.screen_height = height;
    EngineState.is_initialized = true;
    EngineState.total_time = 0.0f;

    Py_RETURN_NONE;
}

static PyObject* pyforge_core_is_open(PyObject* self, PyObject* args) {
    if (!EngineState.is_initialized || !EngineState.window) Py_RETURN_FALSE;
    
    if (glfwWindowShouldClose(EngineState.window)) {
        glfwDestroyWindow(EngineState.window);
        glfwTerminate();
        EngineState.window = NULL;
        EngineState.is_initialized = false;
        Py_RETURN_FALSE;
    }
    
    glfwPollEvents();
    
    // 🌟 FIXED: The macro handles the return statement implicitly!
    Py_RETURN_TRUE; 
}

static PyObject* pyforge_core_refresh(PyObject* self, PyObject* args) {
    float dt;
    if (!PyArg_ParseTuple(args, "f", &dt)) dt = 0.016f; // Standard frame tick fallback
    
    if (EngineState.window) {
        EngineState.total_time += dt; // Accumulate time slice values natively
        glfwSwapBuffers(EngineState.window);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    Py_RETURN_NONE;
}

static PyObject* pyforge_core_set_vsync(PyObject* self, PyObject* args) {
    int enabled;
    if (!PyArg_ParseTuple(args, "i", &enabled)) return NULL;
    glfwSwapInterval(enabled ? 1 : 0);
    Py_RETURN_NONE;
}

static PyObject* pyforge_core_init_buffers(PyObject* self, PyObject* args) {
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
    glCompileShader(vertex_shader);

    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
    glCompileShader(fragment_shader);

    shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    // Allocate a combined vertex memory buffer for all shapes
    // 3 vertices (Triangle) + 6 vertices (Square) + 96 vertices (32-sided Circle) = 105 vertices
    float master_vertex_buffer[105 * 2];
    int idx = 0;

    // 🔺 1. Bake Triangle Geometry (3 Vertices)
    master_vertex_buffer[idx++] =  0.0f;  master_vertex_buffer[idx++] =  0.5f;
    master_vertex_buffer[idx++] = -0.5f; master_vertex_buffer[idx++] = -0.5f;
    master_vertex_buffer[idx++] =  0.5f;  master_vertex_buffer[idx++] = -0.5f;

    // 🟩 2. Bake Square Geometry (6 Vertices)
    master_vertex_buffer[idx++] = -0.5f; master_vertex_buffer[idx++] =  0.5f;
    master_vertex_buffer[idx++] = -0.5f; master_vertex_buffer[idx++] = -0.5f;
    master_vertex_buffer[idx++] =  0.5f;  master_vertex_buffer[idx++] = -0.5f;
    master_vertex_buffer[idx++] = -0.5f; master_vertex_buffer[idx++] =  0.5f;
    master_vertex_buffer[idx++] =  0.5f;  master_vertex_buffer[idx++] = -0.5f;
    master_vertex_buffer[idx++] =  0.5f;  master_vertex_buffer[idx++] =  0.5f;

    // 🔵 3. Bake Circle Geometry (32 Triangles = 96 Vertices)
    for (int i = 0; i < 32; i++) {
        float angle1 = (float)i * (2.0f * M_PI / 32.0f);
        float angle2 = (float)(i + 1) * (2.0f * M_PI / 32.0f);

        // Center point of triangle fan
        master_vertex_buffer[idx++] = 0.0f; master_vertex_buffer[idx++] = 0.0f;
        // Outer point 1
        master_vertex_buffer[idx++] = cosf(angle1) * 0.5f; master_vertex_buffer[idx++] = sinf(angle1) * 0.5f;
        // Outer point 2
        master_vertex_buffer[idx++] = cosf(angle2) * 0.5f; master_vertex_buffer[idx++] = sinf(angle2) * 0.5f;
    }

    glGenVertexArrays(1, &shape_vao);
    glGenBuffers(1, &base_vertex_vbo);
    glGenBuffers(1, &instance_data_vbo);

    glBindVertexArray(shape_vao);

    glBindBuffer(GL_ARRAY_BUFFER, base_vertex_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(master_vertex_buffer), master_vertex_buffer, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, instance_data_vbo);
    glBufferData(GL_ARRAY_BUFFER, MAX_INSTANCES * sizeof(ShapeInstance), NULL, GL_STREAM_DRAW);

    GLsizei stride = sizeof(ShapeInstance);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(ShapeInstance, x));
    glEnableVertexAttribArray(1);
    glVertexAttribDivisor(1, 1); 

    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(ShapeInstance, size));
    glEnableVertexAttribArray(2);
    glVertexAttribDivisor(2, 1);

    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(ShapeInstance, base_angle));
    glEnableVertexAttribArray(3);
    glVertexAttribDivisor(3, 1);

    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(ShapeInstance, r));
    glEnableVertexAttribArray(4);
    glVertexAttribDivisor(4, 1);

    glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(ShapeInstance, shape_type));
    glEnableVertexAttribArray(5);
    glVertexAttribDivisor(5, 1);

    glBindVertexArray(0);
    Py_RETURN_NONE;
}


static PyObject* pyforge_core_draw_batch(PyObject* self, PyObject* args) {
    PyObject* py_list;
    if (!PyArg_ParseTuple(args, "O", &py_list)) return NULL;

    Py_ssize_t count = PyList_Size(py_list);
    if (count == 0) Py_RETURN_NONE;
    if (count > MAX_INSTANCES) count = MAX_INSTANCES;

    for (Py_ssize_t i = 0; i < count; i++) {
        PyObject* item = PyList_GetItem(py_list, i);
        double x, y, size, base_angle, rot_speed, r, g, b, shape_type;
        
        if (PyArg_ParseTuple(item, "ddddddddd", &x, &y, &size, &base_angle, &rot_speed, &r, &g, &b, &shape_type)) {
            instance_cache[i].x = (float)x;
            instance_cache[i].y = (float)y;
            instance_cache[i].size = (float)size;
            instance_cache[i].base_angle = (float)base_angle; 
            instance_cache[i].rot_speed = (float)rot_speed;
            instance_cache[i].r = (float)r;
            instance_cache[i].g = (float)g;
            instance_cache[i].b = (float)b;
            instance_cache[i].shape_type = (float)shape_type;
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, instance_data_vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, count * sizeof(ShapeInstance), instance_cache);

    glUseProgram(shader_program);
    
    GLint time_loc = glGetUniformLocation(shader_program, "u_Time");
    glUniform1f(time_loc, EngineState.total_time);

    glBindVertexArray(shape_vao);

    // 🔒 STATE-AWARE LAYER BATCH ROUTING: Draw shapes using their exact VRAM index boundaries
    for (Py_ssize_t i = 0; i < count; i++) {
        // Point your hardware attribute data to this single element index
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(ShapeInstance), &instance_cache[i]);

        if (instance_cache[i].shape_type < 0.5f) {
            // Draw Triangle: Start vertex index 0, length 3 vertices
            glDrawArrays(GL_TRIANGLES, 0, 3);
        } else if (instance_cache[i].shape_type < 1.5f) {
            // Draw Square: Start vertex index 3, length 6 vertices
            glDrawArrays(GL_TRIANGLES, 3, 6);
        } else {
            // Draw Circle: Start vertex index 9, length 96 vertices
            glDrawArrays(GL_TRIANGLES, 9, 96);
        }
    }

    glBindVertexArray(0);
    Py_RETURN_NONE;
}



static PyMethodDef CoreMethods[] = {
    {"init", pyforge_core_init, METH_VARARGS, ""},
    {"is_open", pyforge_core_is_open, METH_NOARGS, ""},
    {"refresh", pyforge_core_refresh, METH_VARARGS, ""}, // Switch to VARARGS to accept delta clocks
    {"init_buffers", pyforge_core_init_buffers, METH_NOARGS, ""},
    {"draw_batch", pyforge_core_draw_batch, METH_VARARGS, ""},
    {"set_vsync", pyforge_core_set_vsync, METH_VARARGS, ""},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef pyforge_core_module = {
    PyModuleDef_HEAD_INIT, "pyforge_core", "", -1, CoreMethods
};

PyMODINIT_FUNC PyInit_pyforge_core(void) {
    return PyModule_Create(&pyforge_core_module);
}
