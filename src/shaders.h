#ifndef SHADERS_H
#define SHADERS_H

const char* vertex_shader_source = "#version 330 core\n"
    "layout (location = 0) in vec2 aPos;\n"         // Base mesh template layout positions
    "layout (location = 1) in vec2 aOffset;\n"      // Instance positions (x, y)
    "layout (location = 2) in float aSize;\n"       // Instance Size scaler
    "layout (location = 3) in float aAngle;\n"      // Instance Rotation angle
    "layout (location = 4) in vec3 aColor;\n"       // Instance color vectors (r, g, b)
    "layout (location = 5) in float aShapeID;\n"    // 🌟 NEW: Selection type ID attribute (0=Quad, 1=Triangle, 2=Circle)
    "\n"
    "out vec3 FragColor;\n"
    "\n"
    "void main() {\n"
    "    vec2 vertexCoords = aPos;\n"
    "    \n"
    "    // 🛠️ SHAPE SELECTION MATRIX MULTIPLEXER\n"
    "    if (aShapeID > 0.5 && aShapeID < 1.5) {\n"
    "        // Triangle logic override: Collapse the top-right coordinate to make a perfect 3-sided shape\n"
    "        if (gl_VertexID == 4 || gl_VertexID == 5) vertexCoords = vec2(0.0, 0.5);\n"
    "    } else if (aShapeID > 1.5) {\n"
    "        // Circle logic override: Smoothly stretch corners using trigonometry mapping formulas\n"
    "        float rad = radians(gl_VertexID * 60.0);\n"
    "        vertexCoords = vec2(cos(rad) * 0.5, sin(rad) * 0.5);\n"
    "    }\n"
    "    \n"
    "    // GPU Rotational Matrix Translation Pass\n"
    "    float angleRad = radians(aAngle);\n"
    "    float cosA = cos(angleRad);\n"
    "    float sinA = sin(angleRad);\n"
    "    vec2 rotatedPos = vec2(\n"
    "        (vertexCoords.x * cosA - vertexCoords.y * sinA) * aSize,\n"
    "        (vertexCoords.x * sinA + vertexCoords.y * cosA) * aSize\n"
    "    );\n"
    "    \n"
    "    float normX = ((rotatedPos.x + aOffset.x) / 1024.0) * 2.0 - 1.0;\n"
    "    float normY = 1.0 - ((rotatedPos.y + aOffset.y) / 768.0) * 2.0;\n"
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

#endif
