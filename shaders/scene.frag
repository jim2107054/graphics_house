#version 330 core

/**
 * ============================================================================
 * [VIVA EXPLANATION]: Fragment Shader - Cinematic Night Illumination
 * ============================================================================
 * Implements a multi-light Blinn-Phong shading model with Halloween atmosphere:
 *
 * 1. Directional Moonlight:
 *    - Simulates distant celestial light source (parallel rays: direction vector only).
 *    - Cool, pale silver-blue color palette (e.g., RGB: 0.25, 0.35, 0.60).
 *
 * 2. Point Light (Flickering Jack-o'-Lantern / Candle):
 *    - Positioned in 3D space with quadratic distance attenuation:
 *      Attenuation = 1.0 / (constant + linear * d + quadratic * d^2)
 *    - Warm eerie orange-amber glow (e.g., RGB: 1.0, 0.55, 0.1).
 *
 * 3. Blinn-Phong Specular Reflection:
 *    - Uses halfway vector H = normalize(LightDir + ViewDir).
 *    - Computationally efficient & avoids specular artifacts at grazing angles.
 *
 * 4. Atmospheric Halloween Fog:
 *    - Exponential squared fog factor: fog = exp(-pow(distance * fogDensity, 2.0)).
 *    - Blends the shaded fragment towards the dark night sky color.
 * ============================================================================
 */

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in float ViewDistance;

struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    
    float constant;
    float linear;
    float quadratic;
};

struct Material {
    vec3 albedo;          // Base surface color if no texture is bound
    vec3 specularColor;
    float shininess;      // Specular exponent (e.g. 16.0 to 64.0)
    int useTexture;       // 1 = sample from diffuseMap, 0 = use albedo color
};

// Uniforms
uniform vec3 viewPos;              // Camera position in world space
uniform DirLight moonLight;        // Celestial Directional Moon Light
uniform PointLight lanternLight;   // Jack-o'-Lantern / Pumpkin Point Light
uniform Material material;
uniform sampler2D diffuseTexture;

// Atmospheric Fog Parameters
uniform vec3 fogColor;             // Dark Night Blue (0.015, 0.02, 0.05)
uniform float fogDensity;          // Fog thickness

// Function Prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 surfaceColor);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 surfaceColor);

void main() {
    // Properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    // Determine Base Surface Color (Texture or procedural material)
    vec3 surfaceColor = material.albedo;
    if (material.useTexture == 1) {
        surfaceColor = texture(diffuseTexture, TexCoords).rgb;
    }

    // Phase 1: Directional Moonlight
    vec3 result = CalcDirLight(moonLight, norm, viewDir, surfaceColor);

    // Phase 2: Point Light (Spooky Pumpkin Lantern)
    result += CalcPointLight(lanternLight, norm, FragPos, viewDir, surfaceColor);

    // Phase 3: Atmospheric Halloween Ground Fog (Exponential Squared)
    float fogFactor = exp(-pow(ViewDistance * fogDensity, 2.0));
    fogFactor = clamp(fogFactor, 0.0, 1.0);

    // Blend lit surface with atmospheric midnight fog
    vec3 finalColor = mix(fogColor, result, fogFactor);

    FragColor = vec4(finalColor, 1.0);
}

// Calculates color contribution from Directional Moonlight
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 surfaceColor) {
    vec3 lightDir = normalize(-light.direction);

    // Diffuse shading (Lambertian cosine law: max(dot(N, L), 0.0))
    float diff = max(dot(normal, lightDir), 0.0);

    // Specular shading (Blinn-Phong Halfway Vector)
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);

    // Combine components
    vec3 ambient  = light.ambient  * surfaceColor;
    vec3 diffuse  = light.diffuse  * diff * surfaceColor;
    vec3 specular = light.specular * spec * material.specularColor;

    return (ambient + diffuse + specular);
}

// Calculates color contribution from Point Light (Lantern / Candle)
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 surfaceColor) {
    vec3 lightDir = normalize(light.position - fragPos);

    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // Specular shading (Blinn-Phong)
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);

    // Distance attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // Combine results
    vec3 ambient  = light.ambient  * surfaceColor * attenuation;
    vec3 diffuse  = light.diffuse  * diff * surfaceColor * attenuation;
    vec3 specular = light.specular * spec * material.specularColor * attenuation;

    return (ambient + diffuse + specular);
}
