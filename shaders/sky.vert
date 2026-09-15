#version 330 core

layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main() {
    TexCoords = aPos;
    // Remove translation from the view matrix so the sky background stays centered on the camera
    mat4 staticView = mat4(mat3(view));
    vec4 pos = projection * staticView * vec4(aPos, 1.0);
    
    // [VIVA TRICK]: Setting z = w ensures depth buffer test produces 1.0 (the farthest possible depth in NDC)
    // This allows rendering the skybox with glDepthFunc(GL_LEQUAL) so it doesn't overwrite foreground objects.
    gl_Position = pos.xyww;
}
