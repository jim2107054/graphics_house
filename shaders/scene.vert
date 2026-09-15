#version 330 core

/**
 * ============================================================================
 * [VIVA EXPLANATION]: Vertex Shader - Geometric Transformations
 * ============================================================================
 * Vertex attributes are fed directly from Vertex Buffer Objects (VBOs)
 * configured via a Vertex Array Object (VAO).
 *
 * Coordinates Pipeline Transformation:
 * 1. Local Space: Model raw coordinates
 * 2. World Space: FragPos = vec3(model * vec4(aPos, 1.0))
 * 3. View/Eye Space: view * FragPos
 * 4. Clip Space: gl_Position = projection * view * FragPos
 *
 * Normal Matrix:
 * Normal matrix = transpose(inverse(mat3(model)))
 * Crucial in 3D graphics when non-uniform scaling is applied to prevent normal distortion.
 * ============================================================================
 */

layout (location = 0) in vec3 aPos;       // Vertex Position in Local Space
layout (location = 1) in vec3 aNormal;    // Vertex Normal vector
layout (location = 2) in vec2 aTexCoords; // UV Texture Coordinates

out vec3 FragPos;      // World space position of the fragment (passed to Frag Shader)
out vec3 Normal;       // World space normal vector
out vec2 TexCoords;    // Interpolated UV coordinates
out float ViewDistance;// Distance to camera for atmospheric fog calculation

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    // Transform vertex position into World Space
    vec4 worldPos = model * vec4(aPos, 1.0);
    FragPos = vec3(worldPos);

    // Transform normal to World Space using the Normal Matrix (safe against non-uniform scaling)
    Normal = mat3(transpose(inverse(model))) * aNormal;

    // Pass through texture coordinates
    TexCoords = aTexCoords;

    // Calculate view-space position for depth/fog calculations
    vec4 viewPos = view * worldPos;
    ViewDistance = length(viewPos.xyz);

    // Output final Clip Space position to the rasterizer
    gl_Position = projection * viewPos;
}
