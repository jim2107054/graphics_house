#version 330 core

out vec4 FragColor;

in vec3 TexCoords;

uniform vec3 moonDir;   // Normalized direction towards the moon
uniform float time;

void main() {
    vec3 dir = normalize(TexCoords);

    // 1. Spooky Midnight Sky Gradient
    // Deep midnight indigo at zenith -> atmospheric misty dark teal near horizon
    vec3 zenithColor  = vec3(0.008, 0.012, 0.035); // Deep dark blue
    vec3 horizonColor = vec3(0.025, 0.045, 0.090); // Misty horizon blue
    
    float height = clamp(dir.y * 0.5 + 0.5, 0.0, 1.0);
    vec3 skyColor = mix(horizonColor, zenithColor, height);

    // 2. Giant Cinematic Harvest/Halloween Moon
    float moonDot = dot(dir, normalize(moonDir));
    
    // Moon disc radius threshold
    float moonRadius = 0.985;
    float moonEdgeSoftness = 0.003;
    float moonMask = smoothstep(moonRadius - moonEdgeSoftness, moonRadius, moonDot);
    
    // Moon glow / halo halo in the misty night sky
    float halo = pow(max(moonDot, 0.0), 32.0) * 0.45;

    // Glowing Moon Color (Pale eerie silver-yellow)
    vec3 moonColor = vec3(0.95, 0.92, 0.78);
    vec3 haloColor = vec3(0.35, 0.45, 0.65);

    vec3 finalColor = skyColor + halo * haloColor + moonMask * moonColor;

    FragColor = vec4(finalColor, 1.0);
}
