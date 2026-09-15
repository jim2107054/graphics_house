#pragma once

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

/**
 * ============================================================================
 * [VIVA EXPLANATION]: Shader Class (Modern OpenGL 3.3 Core Profile)
 * ============================================================================
 * In Modern OpenGL, the fixed-function pipeline is deprecated in favor of a
 * programmable pipeline. All rendering is driven by GPU programs called "Shaders".
 *
 * 1. Vertex Shader (*.vert):
 *    - Runs once per vertex.
 *    - Transforms 3D local coordinates into Clip Space (MVP: Model-View-Projection).
 *    - Passes interpolated attributes (normals, texture coordinates, world positions)
 *      to the rasterizer.
 *
 * 2. Fragment Shader (*.frag):
 *    - Runs once per pixel candidate (fragment) produced during rasterization.
 *    - Computes final pixel color incorporating Blinn-Phong/PBR lighting, shadows,
 *      materials, textures, and atmospheric night fog.
 *
 * 3. Compilation & Linking Process:
 *    glCreateShader() -> glShaderSource() -> glCompileShader() -> Check Errors
 *    glCreateProgram() -> glAttachShader() -> glLinkProgram() -> Check Errors
 *    glDeleteShader() (cleanup intermediate compiled objects)
 * ============================================================================
 */
class Shader {
public:
    GLuint ID; // OpenGL Program Object Handle

    Shader() : ID(0) {}

    // Constructor: Reads, compiles, and links vertex and fragment shaders
    Shader(const char* vertexPath, const char* fragmentPath) {
        init(vertexPath, fragmentPath);
    }

    void init(const char* vertexPath, const char* fragmentPath) {
        std::string vertexCode;
        std::string fragmentCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;

        // Ensure ifstream objects can throw exceptions:
        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

        try {
            // Open files
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;

            // Read file's buffer contents into streams
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();

            // Close file handlers
            vShaderFile.close();
            fShaderFile.close();

            // Convert stream into string
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
        }
        catch (std::ifstream::failure& e) {
            std::cerr << "[ERROR::SHADER] File not successfully read: " 
                      << vertexPath << " or " << fragmentPath << "\nDetails: " 
                      << e.what() << std::endl;
        }

        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();

        // 1. Compile Vertex Shader
        GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX", vertexPath);

        // 2. Compile Fragment Shader
        GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT", fragmentPath);

        // 3. Link Shader Program
        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        glLinkProgram(ID);
        checkCompileErrors(ID, "PROGRAM", "Shader Program Linking");

        // 4. Delete the shaders as they're linked into our program now and no longer necessary
        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }

    // Activate the shader program for rendering
    void use() const {
        glUseProgram(ID);
    }

    // Uniform utility functions (communicate CPU -> GPU pipeline)
    void setBool(const std::string& name, bool value) const {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }

    void setInt(const std::string& name, int value) const {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }

    void setFloat(const std::string& name, float value) const {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }

    void setVec2(const std::string& name, const glm::vec2& value) const {
        glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }

    void setVec3(const std::string& name, const glm::vec3& value) const {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }

    void setVec3(const std::string& name, float x, float y, float z) const {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
    }

    void setVec4(const std::string& name, const glm::vec4& value) const {
        glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }

    void setMat3(const std::string& name, const glm::mat3& mat) const {
        glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
    }

    void setMat4(const std::string& name, const glm::mat4& mat) const {
        // [VIVA NOTE]: GL_FALSE indicates matrix is in column-major order (standard for GLM and OpenGL)
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
    }

private:
    // Utility function for checking shader compilation/linking errors.
    void checkCompileErrors(GLuint shader, const std::string& type, const std::string& path) {
        GLint success;
        GLchar infoLog[1024];

        if (type != "PROGRAM") {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cerr << "[ERROR::SHADER_COMPILATION_ERROR] Type: " << type 
                          << "\nSource: " << path 
                          << "\nInfo Log: " << infoLog << "\n -- --------------------------------------------------- -- " 
                          << std::endl;
            }
        }
        else {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cerr << "[ERROR::PROGRAM_LINKING_ERROR] Type: " << type 
                          << "\nTarget: " << path 
                          << "\nInfo Log: " << infoLog << "\n -- --------------------------------------------------- -- " 
                          << std::endl;
            }
        }
    }
};
