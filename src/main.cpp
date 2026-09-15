#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>

// Window dimensions
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Callback to adjust viewport when window size changes
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// Process keyboard input
void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// Draw an extruded 3D faceted 5-pointed star
void drawStar3D() {
    const int numPoints = 5;
    const int numVertices = numPoints * 2; // 10 vertices (5 outer tips, 5 inner valleys)
    const float outerRadius = 1.0f;
    const float innerRadius = 0.42f;
    const float depth = 0.35f;             // 3D thickness / apex height

    float x[10], y[10];
    for (int i = 0; i < numVertices; ++i) {
        float angle = i * (float)M_PI / 5.0f - (float)M_PI / 2.0f;
        float r = (i % 2 == 0) ? outerRadius : innerRadius;
        x[i] = r * cosf(angle);
        y[i] = r * sinf(angle);
    }

    glBegin(GL_TRIANGLES);

    // Front Facets (Connected to front apex at +depth)
    for (int i = 0; i < numVertices; ++i) {
        int next = (i + 1) % numVertices;

        // Alternate colors for a faceted 3D metallic gold effect
        if (i % 2 == 0) {
            glColor3f(1.0f, 0.85f, 0.1f); // Bright Gold
        } else {
            glColor3f(0.9f, 0.65f, 0.05f); // Warm Amber Gold
        }

        glVertex3f(0.0f, 0.0f, depth);
        glVertex3f(x[i], y[i], 0.0f);
        glVertex3f(x[next], y[next], 0.0f);
    }

    // Back Facets (Connected to back apex at -depth)
    for (int i = 0; i < numVertices; ++i) {
        int next = (i + 1) % numVertices;

        // Slightly darker shades for back-facing facets
        if (i % 2 == 0) {
            glColor3f(0.75f, 0.55f, 0.05f);
        } else {
            glColor3f(0.60f, 0.40f, 0.0f);
        }

        glVertex3f(0.0f, 0.0f, -depth);
        glVertex3f(x[next], y[next], 0.0f);
        glVertex3f(x[i], y[i], 0.0f);
    }

    glEnd();
}

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Create a GLFW window
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL - 3D Star", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Load OpenGL function pointers using GLAD
    if (!gladLoadGL(glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Enable Depth Testing
    glEnable(GL_DEPTH_TEST);

    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "3D Star active. Press ESC to exit." << std::endl;

    // Render loop (Static view - no auto motion)
    while (!glfwWindowShouldClose(window)) {
        // Input
        processInput(window);

        // Render background clear
        glClearColor(0.08f, 0.09f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Setup Projection Matrix
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        float aspect = (height > 0) ? (float)width / (float)height : 1.0f;

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        
        // Perspective (FOV: 45 deg, near: 0.1, far: 100.0)
        float fov = 45.0f * (float)M_PI / 180.0f;
        float top = 0.1f * tanf(fov / 2.0f);
        float bottom = -top;
        float right = top * aspect;
        float left = -right;
        glFrustum(left, right, bottom, top, 0.1f, 100.0f);

        // Setup Modelview Matrix
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        // Move camera back to view the star clearly
        glTranslatef(0.0f, 0.0f, -3.0f);

        // Draw static 3D Star (no motion)
        drawStar3D();

        // Swap buffers and poll IO events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Terminate GLFW
    glfwTerminate();
    return 0;
}
