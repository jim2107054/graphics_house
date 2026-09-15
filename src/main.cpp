/**
 * ================================================================================================
 * PROJECT: Cinematic Halloween Haunted House at Night
 * GRAPHICS API: Modern OpenGL (3.3 Core Profile)
 * LIBRARIES: GLFW (Windowing & Input), GLAD (Function Loader), GLM (Math), stb_image (Textures)
 * ================================================================================================
 * 
 * [VIVA COMPREHENSIVE GUIDE & CONCEPTS]:
 * 
 * 1. MODERN OPENGL (3.3 CORE) VS LEGACY OPENGL (1.x - 2.x):
 *    - Legacy OpenGL used the "Immediate Mode" (glBegin/glEnd, glMatrixMode, glLightfv).
 *      These commands sent geometry vertex-by-vertex every frame over the slow PCIe bus (CPU bottleneck).
 *    - Modern Core Profile completely strips out fixed-function pipelines and matrix stacks.
 *      Geometry is uploaded once into GPU VRAM (via VBO/VAO), and all lighting and transformations
 *      are computed asynchronously on the GPU shaders (GLSL).
 * 
 * 2. VERTEX ARRAYS (VAO), VERTEX BUFFERS (VBO), ELEMENT BUFFERS (EBO):
 *    - VBO (Vertex Buffer Object): Block of GPU memory storing raw vertex coordinates, normals, UVs.
 *    - VAO (Vertex Array Object): State container storing the memory layout & attribute pointers
 *      (e.g., location 0 = vec3 pos, location 1 = vec3 normal, location 2 = vec2 uv).
 *    - EBO (Element Buffer Object): Stores vertex index arrays for indexed drawing (glDrawElements).
 * 
 * 3. DELTA-TIME FRAME INDEPENDENCE:
 *    - Variable frame rates (e.g. 60 FPS vs 144 FPS) would cause camera and physics to move at
 *      different speeds if updated per-frame. Delta-time (deltaTime = currentTime - lastTime) ensures
 *      constant movement speed regardless of hardware performance.
 * 
 * 4. DEPTH TESTING & Z-BUFFERING:
 *    - Depth Testing (GL_DEPTH_TEST): When rasterizing fragments, OpenGL compares each fragment's
 *      depth (Z value between 0.0 near and 1.0 far) against the current value in the 24-bit Z-Buffer.
 *    - Function GL_LESS renders fragments only if they are closer to the camera, discarding occluded geometry.
 * 
 * 5. sRGB / LINEAR LIGHTING PIPELINE:
 *    - Computer monitors apply an inverse gamma curve (~2.2) to compensate for display characteristics.
 *    - If lighting calculations are performed in non-linear sRGB space, colors darken unnaturally and falloff is wrong.
 *    - Enabling GL_FRAMEBUFFER_SRGB tells the GPU hardware to perform all shader math in linear color space,
 *      then automatically gamma-encode (x^(1/2.2)) to the screen before buffer swap.
 * ================================================================================================
 */

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <cmath>

#include "Shader.h"
#include "Camera.h"

// ------------------------------------------------------------------------------------------------
// Window Configuration & Global State
// ------------------------------------------------------------------------------------------------
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 900;

// Camera System (Initial cinematic perspective framing the haunted mansion)
Camera camera(glm::vec3(0.0f, 3.5f, 18.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, -6.0f);
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// Delta-time timing management
float deltaTime = 0.0f; // Time between current frame and last frame
float lastFrame = 0.0f;

// Interactive controls
bool isFlashlightOn = false;
bool isWireframe = false;

// ------------------------------------------------------------------------------------------------
// Function Declarations
// ------------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void renderCube(GLuint vao);
void renderPlane(GLuint vao);
void renderPyramid(GLuint vao);
void setupGeometry(GLuint &cubeVAO, GLuint &cubeVBO, 
                   GLuint &planeVAO, GLuint &planeVBO, 
                   GLuint &pyramidVAO, GLuint &pyramidVBO, 
                   GLuint &skyVAO, GLuint &skyVBO);

// ================================================================================================
// Main Entry Point
// ================================================================================================
int main() {
    std::cout << "===================================================================\n";
    std::cout << " [CINEMATIC MODERN OPENGL 3.3] Halloween Haunted House at Night     \n";
    std::cout << "===================================================================\n";

    // 1. Initialize GLFW library
    if (!glfwInit()) {
        std::cerr << "[FATAL] Failed to initialize GLFW windowing library!\n";
        return -1;
    }

    // 2. Configure GLFW OpenGL Context Profile (OpenGL 3.3 Core)
    // [VIVA NOTE]: Modern Core Profile removes all deprecated immediate-mode functions
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4); // 4x MSAA (Multisample Anti-Aliasing) for crisp edges

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Required on macOS
#endif

    // 3. Create GLFW Window Object (1600x900 Resolution)
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, 
        "Halloween Haunted House at Night - Modern OpenGL 3.3 Core", NULL, NULL);
    if (!window) {
        std::cerr << "[FATAL] Failed to create GLFW 1600x900 window!\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // 4. Set Event Callbacks & Capture Cursor for cinematic FPS mouse-look
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Enable V-Sync (Vertical Synchronization) to prevent screen tearing
    glfwSwapInterval(1);

    // 5. Initialize GLAD (OpenGL Function Pointer Loader)
    // [VIVA NOTE]: Since OpenGL drivers are dynamically loaded from GPU vendor DLLs,
    // GLAD retrieves function addresses at runtime via glfwGetProcAddress.
    if (!gladLoadGL(glfwGetProcAddress)) {
        std::cerr << "[FATAL] Failed to initialize GLAD OpenGL loader!\n";
        return -1;
    }

    std::cout << "[GPU VENDOR]   : " << glGetString(GL_VENDOR) << "\n";
    std::cout << "[GPU RENDERER] : " << glGetString(GL_RENDERER) << "\n";
    std::cout << "[OPENGL VERSION]: " << glGetString(GL_VERSION) << "\n";
    std::cout << "[GLSL VERSION] : " << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n";
    std::cout << "-------------------------------------------------------------------\n";
    std::cout << "Controls: [W/A/S/D] Move Camera | [SHIFT] Sprint | [Mouse] Look Around\n";
    std::cout << "          [SPACE/LCTRL] Fly Up/Down | [Scroll] Zoom FOV | [TAB] Wireframe\n";
    std::cout << "          [ESC] Exit\n";
    std::cout << "===================================================================\n";

    // 6. Configure Global OpenGL State Machine
    // -------------------------------------------------------------------------
    // Enable Hardware Z-Buffer Depth Testing (Discard hidden/occluded surface fragments)
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Enable Hardware sRGB Gamma Correction (Linear color pipeline for photorealism)
    // [VIVA NOTE]: Prevents color banding and enables realistic light attenuation
    glEnable(GL_FRAMEBUFFER_SRGB);

    // Enable Alpha Blending for atmospheric transparency effects
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Enable Backface Culling (Optimize rasterization by skipping back-facing triangles)
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW); // Counter-Clockwise winding order is front-facing

    // 7. Compile & Link GLSL Shader Programs
    // -------------------------------------------------------------------------
    Shader sceneShader("shaders/scene.vert", "shaders/scene.frag");
    Shader skyShader("shaders/sky.vert", "shaders/sky.frag");

    // 8. Setup GPU Geometry Buffers (VAO/VBOs)
    // -------------------------------------------------------------------------
    GLuint cubeVAO, cubeVBO;
    GLuint planeVAO, planeVBO;
    GLuint pyramidVAO, pyramidVBO;
    GLuint skyVAO, skyVBO;
    setupGeometry(cubeVAO, cubeVBO, planeVAO, planeVBO, pyramidVAO, pyramidVBO, skyVAO, skyVBO);

    // 9. Main High-Performance Render Loop
    // -------------------------------------------------------------------------
    while (!glfwWindowShouldClose(window)) {
        // Compute delta-time (High-precision timer)
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Process Input
        processInput(window);

        // Clear Screen Buffers
        // Atmospheric Dark Night Blue clear color: RGB(0.015, 0.02, 0.045)
        glClearColor(0.015f, 0.020f, 0.045f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Calculate Camera Projection & View Matrices
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        float aspectRatio = (height > 0) ? (float)width / (float)height : (16.0f / 9.0f);
        
        // Perspective Projection: 60° FOV, Aspect Ratio, Configurable Near (0.1f) and Far (300.0f)
        glm::mat4 projection = camera.GetProjectionMatrix(aspectRatio);
        glm::mat4 view = camera.GetViewMatrix();

        // ---------------------------------------------------------------------
        // Render 1: Atmospheric Midnight Sky & Giant Halloween Moon
        // ---------------------------------------------------------------------
        glDisable(GL_CULL_FACE); // Skybox needs inner faces visible
        glDepthFunc(GL_LEQUAL);  // Skybox uses max depth (z = 1.0)
        skyShader.use();
        skyShader.setMat4("projection", projection);
        skyShader.setMat4("view", view);
        // Celestial Moon Position in the Night Sky
        glm::vec3 moonDirection = glm::normalize(glm::vec3(0.55f, 0.70f, -0.65f));
        skyShader.setVec3("moonDir", moonDirection);
        skyShader.setFloat("time", currentFrame);
        
        glBindVertexArray(skyVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        glDepthFunc(GL_LESS);
        glEnable(GL_CULL_FACE);

        // ---------------------------------------------------------------------
        // Render 2: Scene Geometry (Ground, Haunted Mansion, Jack-o'-Lanterns)
        // ---------------------------------------------------------------------
        sceneShader.use();
        sceneShader.setMat4("projection", projection);
        sceneShader.setMat4("view", view);
        sceneShader.setVec3("viewPos", camera.Position);

        // Atmospheric Halloween Ground Fog (Midnight blue fog matching sky)
        sceneShader.setVec3("fogColor", glm::vec3(0.015f, 0.020f, 0.045f));
        sceneShader.setFloat("fogDensity", 0.022f);

        // 1. Moonlight Illumination (Directional Light - Cool Pale Silver-Blue)
        sceneShader.setVec3("moonLight.direction", -moonDirection);
        sceneShader.setVec3("moonLight.ambient", glm::vec3(0.04f, 0.06f, 0.10f));
        sceneShader.setVec3("moonLight.diffuse", glm::vec3(0.25f, 0.35f, 0.55f));
        sceneShader.setVec3("moonLight.specular", glm::vec3(0.40f, 0.50f, 0.70f));

        // 2. Flickering Jack-o'-Lantern Point Light (Warm Eerie Amber-Orange)
        // Dynamic procedural flame flicker simulation
        float flicker = 0.85f + 0.15f * sin(currentFrame * 8.0f) * cos(currentFrame * 13.0f + 1.2f);
        glm::vec3 lanternPos = glm::vec3(1.8f, 0.65f, 5.0f); // Porch steps pumpkin
        sceneShader.setVec3("lanternLight.position", lanternPos);
        sceneShader.setVec3("lanternLight.ambient", glm::vec3(0.12f, 0.05f, 0.01f) * flicker);
        sceneShader.setVec3("lanternLight.diffuse", glm::vec3(1.00f, 0.55f, 0.12f) * flicker * 1.8f);
        sceneShader.setVec3("lanternLight.specular", glm::vec3(1.00f, 0.70f, 0.30f) * flicker);
        // Attenuation factors (Reach ~ 15-20 units)
        sceneShader.setFloat("lanternLight.constant", 1.0f);
        sceneShader.setFloat("lanternLight.linear", 0.09f);
        sceneShader.setFloat("lanternLight.quadratic", 0.032f);

        // ---------------------------------------------------------------------
        // A. Spooky Foggy Terrain / Ground Plane
        // ---------------------------------------------------------------------
        sceneShader.setVec3("material.albedo", glm::vec3(0.05f, 0.08f, 0.04f)); // Dark withered autumn grass
        sceneShader.setVec3("material.specularColor", glm::vec3(0.1f, 0.1f, 0.1f));
        sceneShader.setFloat("material.shininess", 8.0f);
        sceneShader.setInt("material.useTexture", 0);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(100.0f, 1.0f, 100.0f));
        sceneShader.setMat4("model", model);
        renderPlane(planeVAO);

        // ---------------------------------------------------------------------
        // B. Haunted House Architecture
        // ---------------------------------------------------------------------
        // Material: Weathered dark wooden siding / gothic stone
        sceneShader.setVec3("material.albedo", glm::vec3(0.11f, 0.09f, 0.08f));
        sceneShader.setVec3("material.specularColor", glm::vec3(0.2f, 0.18f, 0.15f));
        sceneShader.setFloat("material.shininess", 16.0f);

        // 1. Main Mansion Body (2-story central gothic manor)
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 2.5f, 0.0f));
        model = glm::scale(model, glm::vec3(7.0f, 5.0f, 6.0f));
        sceneShader.setMat4("model", model);
        renderCube(cubeVAO);

        // 2. West Wing (Asymmetric Victorian extension)
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-4.5f, 2.0f, 0.5f));
        model = glm::scale(model, glm::vec3(3.5f, 4.0f, 5.0f));
        sceneShader.setMat4("model", model);
        renderCube(cubeVAO);

        // 3. Spooky East Victorian Lookout Tower
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(4.2f, 4.5f, 0.0f));
        model = glm::scale(model, glm::vec3(2.8f, 9.0f, 3.2f));
        sceneShader.setMat4("model", model);
        renderCube(cubeVAO);

        // Roof Material (Weathered dark shingles / slate)
        sceneShader.setVec3("material.albedo", glm::vec3(0.06f, 0.06f, 0.08f));
        sceneShader.setFloat("material.shininess", 32.0f);

        // 4. Main Manor Gable Roof (Steep Gothic Pitch)
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 6.5f, 0.0f));
        model = glm::scale(model, glm::vec3(7.4f, 3.2f, 6.4f));
        sceneShader.setMat4("model", model);
        renderPyramid(pyramidVAO);

        // 5. East Tower Steep Spire Roof
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(4.2f, 10.5f, 0.0f));
        model = glm::scale(model, glm::vec3(3.2f, 4.0f, 3.6f));
        sceneShader.setMat4("model", model);
        renderPyramid(pyramidVAO);

        // 6. Crooked Brick Chimney
        sceneShader.setVec3("material.albedo", glm::vec3(0.18f, 0.08f, 0.06f)); // Dark aged brick
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-2.2f, 7.2f, -1.0f));
        model = glm::rotate(model, glm::radians(4.0f), glm::vec3(0.0f, 0.0f, 1.0f)); // Sinister slight tilt
        model = glm::scale(model, glm::vec3(0.9f, 3.5f, 0.9f));
        sceneShader.setMat4("model", model);
        renderCube(cubeVAO);

        // 7. Dilapidated Front Porch
        sceneShader.setVec3("material.albedo", glm::vec3(0.09f, 0.07f, 0.05f));
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.35f, 4.0f));
        model = glm::scale(model, glm::vec3(5.5f, 0.7f, 2.5f));
        sceneShader.setMat4("model", model);
        renderCube(cubeVAO);

        // Porch Columns
        for (float colX : {-2.2f, 2.2f}) {
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(colX, 1.8f, 5.0f));
            model = glm::scale(model, glm::vec3(0.25f, 2.2f, 0.25f));
            sceneShader.setMat4("model", model);
            renderCube(cubeVAO);
        }

        // 8. Glowing Haunted Windows (Eerie candlelit yellow-orange)
        sceneShader.setVec3("material.albedo", glm::vec3(1.2f, 0.8f, 0.25f) * flicker);
        sceneShader.setVec3("material.specularColor", glm::vec3(0.0f));
        sceneShader.setFloat("material.shininess", 1.0f);

        // Second floor center window
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 3.5f, 3.05f));
        model = glm::scale(model, glm::vec3(1.2f, 1.8f, 0.05f));
        sceneShader.setMat4("model", model);
        renderCube(cubeVAO);

        // Tower attic arched window
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(4.2f, 6.8f, 1.65f));
        model = glm::scale(model, glm::vec3(0.9f, 1.6f, 0.05f));
        sceneShader.setMat4("model", model);
        renderCube(cubeVAO);

        // ---------------------------------------------------------------------
        // C. Glowing Jack-o'-Lantern on Porch (Halloween Pumpkin)
        // ---------------------------------------------------------------------
        // Pumpkin Body (Vibrant Halloween Orange)
        sceneShader.setVec3("material.albedo", glm::vec3(0.95f, 0.40f, 0.05f));
        sceneShader.setVec3("material.specularColor", glm::vec3(0.4f, 0.2f, 0.05f));
        sceneShader.setFloat("material.shininess", 32.0f);
        model = glm::mat4(1.0f);
        model = glm::translate(model, lanternPos);
        model = glm::scale(model, glm::vec3(0.75f, 0.65f, 0.75f));
        sceneShader.setMat4("model", model);
        renderCube(cubeVAO);

        // Pumpkin Stem (Dark Gnarled Green)
        sceneShader.setVec3("material.albedo", glm::vec3(0.12f, 0.25f, 0.08f));
        model = glm::mat4(1.0f);
        model = glm::translate(model, lanternPos + glm::vec3(0.0f, 0.45f, 0.0f));
        model = glm::scale(model, glm::vec3(0.12f, 0.35f, 0.12f));
        sceneShader.setMat4("model", model);
        renderCube(cubeVAO);

        // Pumpkin Carved Glowing Eyes & Mouth (Self-Illuminating)
        sceneShader.setVec3("material.albedo", glm::vec3(1.8f, 0.9f, 0.2f) * flicker);
        model = glm::mat4(1.0f);
        model = glm::translate(model, lanternPos + glm::vec3(0.0f, 0.05f, 0.39f));
        model = glm::scale(model, glm::vec3(0.45f, 0.30f, 0.02f));
        sceneShader.setMat4("model", model);
        renderCube(cubeVAO);

        // ---------------------------------------------------------------------
        // D. Spooky Cemetery Gravestones in the Yard
        // ---------------------------------------------------------------------
        sceneShader.setVec3("material.albedo", glm::vec3(0.20f, 0.22f, 0.25f)); // Weathered granite
        sceneShader.setVec3("material.specularColor", glm::vec3(0.15f, 0.15f, 0.15f));
        sceneShader.setFloat("material.shininess", 16.0f);

        std::vector<glm::vec3> gravestones = {
            glm::vec3(-4.0f, 0.6f, 8.0f),
            glm::vec3(-6.2f, 0.5f, 10.5f),
            glm::vec3( 5.5f, 0.55f, 7.5f),
            glm::vec3( 7.8f, 0.45f, 11.0f)
        };

        float tiltAngles[] = { -7.0f, 12.0f, -5.0f, 15.0f };
        for (size_t i = 0; i < gravestones.size(); ++i) {
            model = glm::mat4(1.0f);
            model = glm::translate(model, gravestones[i]);
            model = glm::rotate(model, glm::radians(tiltAngles[i]), glm::vec3(0.0f, 0.0f, 1.0f)); // Aged crooked slant
            model = glm::scale(model, glm::vec3(0.7f, 1.2f, 0.25f));
            sceneShader.setMat4("model", model);
            renderCube(cubeVAO);
        }

        // 10. Swap Front/Back Framebuffers & Poll Window Events
        // [VIVA NOTE]: Double-buffering prevents screen flickering by drawing to back buffer
        // and instantly presenting to the display when completed.
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 11. Cleanup and Deallocate GPU Resources
    // -------------------------------------------------------------------------
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteVertexArrays(1, &planeVAO);
    glDeleteBuffers(1, &planeVBO);
    glDeleteVertexArrays(1, &pyramidVAO);
    glDeleteBuffers(1, &pyramidVBO);
    glDeleteVertexArrays(1, &skyVAO);
    glDeleteBuffers(1, &skyVBO);

    glfwDestroyWindow(window);
    glfwTerminate();
    std::cout << "[SUCCESS] Application shut down cleanly.\n";
    return 0;
}

// ================================================================================================
// Input & Window Callbacks
// ================================================================================================

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Sprint Modifier (Left Shift or Right Shift)
    bool isSprinting = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || 
                        glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS);

    // Camera Navigation (WASD + Space/LCtrl) - Scaled with delta-time & sprint
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime, isSprinting);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime, isSprinting);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime, isSprinting);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime, isSprinting);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime, isSprinting);
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime, isSprinting);

    // Toggle Wireframe Mode (Press TAB)
    static bool tabPressed = false;
    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS && !tabPressed) {
        tabPressed = true;
        isWireframe = !isWireframe;
        glPolygonMode(GL_FRONT_AND_BACK, isWireframe ? GL_LINE : GL_FILL);
    }
    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_RELEASE) {
        tabPressed = false;
    }
}

// Adjust OpenGL Viewport when window is resized
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // [VIVA NOTE]: glViewport maps Normalized Device Coordinates (-1 to 1) to Window Pixels
    glViewport(0, 0, width, height);
}

// Mouse movement for FPS look-around
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // Reversed since Y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// Mouse scroll wheel for FOV zoom
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

// ================================================================================================
// Geometry Helper Functions (VAO / VBO modern OpenGL layout)
// ================================================================================================

void renderCube(GLuint vao) {
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

void renderPlane(GLuint vao) {
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void renderPyramid(GLuint vao) {
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 18);
    glBindVertexArray(0);
}

void setupGeometry(GLuint &cubeVAO, GLuint &cubeVBO, 
                   GLuint &planeVAO, GLuint &planeVBO, 
                   GLuint &pyramidVAO, GLuint &pyramidVBO, 
                   GLuint &skyVAO, GLuint &skyVBO) 
{
    // -------------------------------------------------------------------------
    // 1. Cube Geometry (Positions, Normals, UVs)
    // -------------------------------------------------------------------------
    float cubeVertices[] = {
        // Position           // Normals           // UVs
        // Back Face
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        // Front Face
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
        // Left Face
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        // Right Face
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        // Bottom Face
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
        // Top Face
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f
    };

    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    // Attribute 0: Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    // Attribute 1: Normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    // Attribute 2: TexCoords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

    // -------------------------------------------------------------------------
    // 2. Terrain Plane Geometry
    // -------------------------------------------------------------------------
    float planeVertices[] = {
        // Positions          // Normals         // UVs
         1.0f, 0.0f,  1.0f,   0.0f, 1.0f, 0.0f,  50.0f, 0.0f,
        -1.0f, 0.0f,  1.0f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f,
        -1.0f, 0.0f, -1.0f,   0.0f, 1.0f, 0.0f,   0.0f, 50.0f,

         1.0f, 0.0f,  1.0f,   0.0f, 1.0f, 0.0f,  50.0f, 0.0f,
        -1.0f, 0.0f, -1.0f,   0.0f, 1.0f, 0.0f,   0.0f, 50.0f,
         1.0f, 0.0f, -1.0f,   0.0f, 1.0f, 0.0f,  50.0f, 50.0f
    };

    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

    // -------------------------------------------------------------------------
    // 3. Pyramid / Gothic Roof Geometry
    // -------------------------------------------------------------------------
    float pyramidVertices[] = {
        // Positions           // Normals (approx) // UVs
        // Front Face
         0.0f,  0.5f,  0.0f,   0.0f, 0.5f,  0.8f,  0.5f, 1.0f,
        -0.5f, -0.5f,  0.5f,   0.0f, 0.5f,  0.8f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,   0.0f, 0.5f,  0.8f,  1.0f, 0.0f,
        // Right Face
         0.0f,  0.5f,  0.0f,   0.8f, 0.5f,  0.0f,  0.5f, 1.0f,
         0.5f, -0.5f,  0.5f,   0.8f, 0.5f,  0.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,   0.8f, 0.5f,  0.0f,  1.0f, 0.0f,
        // Back Face
         0.0f,  0.5f,  0.0f,   0.0f, 0.5f, -0.8f,  0.5f, 1.0f,
         0.5f, -0.5f, -0.5f,   0.0f, 0.5f, -0.8f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,   0.0f, 0.5f, -0.8f,  1.0f, 0.0f,
        // Left Face
         0.0f,  0.5f,  0.0f,  -0.8f, 0.5f,  0.0f,  0.5f, 1.0f,
        -0.5f, -0.5f, -0.5f,  -0.8f, 0.5f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  -0.8f, 0.5f,  0.0f,  1.0f, 0.0f,
        // Bottom Base Face (2 Triangles)
        -0.5f, -0.5f, -0.5f,   0.0f,-1.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,   0.0f,-1.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,   0.0f,-1.0f,  0.0f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,   0.0f,-1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,   0.0f,-1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,   0.0f,-1.0f,  0.0f,  0.0f, 1.0f
    };

    glGenVertexArrays(1, &pyramidVAO);
    glGenBuffers(1, &pyramidVBO);
    glBindVertexArray(pyramidVAO);
    glBindBuffer(GL_ARRAY_BUFFER, pyramidVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidVertices), pyramidVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

    // -------------------------------------------------------------------------
    // 4. Sky Backdrop Cube (Only 3D positions needed)
    // -------------------------------------------------------------------------
    float skyboxVertices[] = {
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    glGenVertexArrays(1, &skyVAO);
    glGenBuffers(1, &skyVBO);
    glBindVertexArray(skyVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindVertexArray(0);
}
