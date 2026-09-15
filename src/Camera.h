#pragma once

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

/**
 * ============================================================================
 * [VIVA EXPLANATION]: 3D First-Person Fly Camera System
 * ============================================================================
 * 
 * 1. VIEW MATRIX (World Space -> View Space):
 *    The View Matrix transforms world coordinates such that the camera is at
 *    the origin (0, 0, 0) looking down its local -Z axis.
 *    Using the Gram-Schmidt orthonormal basis:
 *      Right   = normalize(cross(Front, WorldUp))
 *      Up      = normalize(cross(Right, Front))
 *      LookAt  = [ R.x  R.y  R.z  -dot(R, Pos) ]
 *                [ U.x  U.y  U.z  -dot(U, Pos) ]
 *                [-F.x -F.y -F.z   dot(F, Pos) ]
 *                [  0    0    0         1      ]
 *
 * 2. EULER ANGLES & DIRECTION VECTORS (Spherical -> Cartesian conversion):
 *    - Yaw (ψ): Rotation around World Y-axis (Horizontal look).
 *    - Pitch (θ): Rotation around Camera Right axis (Vertical look).
 *      Clamped between -89.0° and +89.0° to eliminate the Gimbal Lock singularity.
 *    - Equations:
 *        Front.x = cos(radians(Yaw)) * cos(radians(Pitch))
 *        Front.y = sin(radians(Pitch))
 *        Front.z = sin(radians(Yaw)) * cos(radians(Pitch))
 *
 * 3. PERSPECTIVE PROJECTION MATRIX (View Space -> Clip Space):
 *    Maps the viewing frustum into Normalized Device Coordinates (NDC: [-1, 1]^3).
 *    - FOV: Field of view in degrees (Vertical angle of the frustum).
 *    - Aspect: Screen width / Screen height.
 *    - Near & Far: Z-clipping boundaries.
 *
 * 4. VELOCITY & SPRINT MECHANIC:
 *    velocity = BaseSpeed * (isSprinting ? SprintMultiplier : 1.0) * deltaTime
 *    Guarantees frame-rate independent smooth flying across any machine.
 * ============================================================================
 */

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

// Default camera configuration constants
namespace CameraDefaults {
    const float YAW               = -90.0f;
    const float PITCH             =  -5.0f;
    const float SPEED             =   6.0f;
    const float SPRINT_MULTIPLIER =   2.5f;
    const float SENSITIVITY       =   0.1f;
    const float FOV               =  60.0f; // 60-degree Field of View
    const float NEAR_PLANE        =   0.1f;
    const float FAR_PLANE         = 300.0f;
}

class Camera {
public:
    // Camera Coordinate Vectors (Orthonormal Basis)
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    // Euler Angles (degrees)
    float Yaw;
    float Pitch;

    // Movement & Sensitivity parameters
    float MovementSpeed;
    float SprintMultiplier;
    float MouseSensitivity;
    float Fov;

    // Frustum Clipping Planes
    float NearPlane;
    float FarPlane;

    /**
     * @brief Constructor for First-Person Fly Camera
     * @param position Initial 3D world position
     * @param up World up reference vector (typically (0, 1, 0))
     * @param yaw Initial horizontal rotation angle (degrees)
     * @param pitch Initial vertical rotation angle (degrees)
     * @param fov Field of view in degrees (default 60.0f)
     * @param nearPlane Near clipping plane distance (default 0.1f)
     * @param farPlane Far clipping plane distance (default 300.0f)
     */
    Camera(glm::vec3 position = glm::vec3(0.0f, 3.5f, 18.0f), 
           glm::vec3 up       = glm::vec3(0.0f, 1.0f, 0.0f), 
           float yaw          = CameraDefaults::YAW, 
           float pitch        = CameraDefaults::PITCH,
           float fov          = CameraDefaults::FOV,
           float nearPlane    = CameraDefaults::NEAR_PLANE,
           float farPlane     = CameraDefaults::FAR_PLANE) 
        : Front(glm::vec3(0.0f, 0.0f, -1.0f)), 
          MovementSpeed(CameraDefaults::SPEED), 
          SprintMultiplier(CameraDefaults::SPRINT_MULTIPLIER),
          MouseSensitivity(CameraDefaults::SENSITIVITY), 
          Fov(fov),
          NearPlane(nearPlane),
          FarPlane(farPlane)
    {
        Position = position;
        WorldUp  = up;
        Yaw      = yaw;
        Pitch    = pitch;
        updateCameraVectors();
    }

    /**
     * @brief Computes the 4x4 View Matrix via LookAt transformation
     * @return glm::mat4 View Matrix
     */
    glm::mat4 GetViewMatrix() const {
        return glm::lookAt(Position, Position + Front, Up);
    }

    /**
     * @brief Computes the 4x4 Perspective Projection Matrix
     * @param aspectRatio Width / Height of the viewport
     * @return glm::mat4 Perspective Projection Matrix
     */
    glm::mat4 GetProjectionMatrix(float aspectRatio) const {
        return glm::perspective(glm::radians(Fov), aspectRatio, NearPlane, FarPlane);
    }

    /**
     * @brief Processes WASD + Space/Ctrl keyboard navigation with Shift sprint support
     * @param direction Direction enum (FORWARD, BACKWARD, LEFT, RIGHT, UP, DOWN)
     * @param deltaTime Frame delta-time in seconds
     * @param isSprinting True if Left Shift is held
     */
    void ProcessKeyboard(Camera_Movement direction, float deltaTime, bool isSprinting = false) {
        float speed = MovementSpeed * (isSprinting ? SprintMultiplier : 1.0f);
        float velocity = speed * deltaTime;

        if (direction == FORWARD)
            Position += Front * velocity;
        if (direction == BACKWARD)
            Position -= Front * velocity;
        if (direction == LEFT)
            Position -= Right * velocity;
        if (direction == RIGHT)
            Position += Right * velocity;
        if (direction == UP)
            Position += WorldUp * velocity;
        if (direction == DOWN)
            Position -= WorldUp * velocity;
    }

    /**
     * @brief Processes mouse cursor movement to update Euler angles (Yaw & Pitch)
     * @param xoffset Horizontal mouse offset from last frame
     * @param yoffset Vertical mouse offset from last frame
     * @param constrainPitch Clamps pitch to [-89.0°, +89.0°] to prevent screen inversion
     */
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true) {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw   += xoffset;
        Pitch += yoffset;

        // Clamp vertical pitch to prevent Gimbal Lock and view flipping
        if (constrainPitch) {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        // Recompute orthonormal basis vectors
        updateCameraVectors();
    }

    /**
     * @brief Processes mouse scroll wheel input to zoom FOV
     * @param yoffset Vertical scroll wheel offset
     */
    void ProcessMouseScroll(float yoffset) {
        Fov -= static_cast<float>(yoffset);
        if (Fov < 15.0f)
            Fov = 15.0f;
        if (Fov > 90.0f)
            Fov = 90.0f;
    }

    // Setters for configurable clipping planes & FOV
    void SetClippingPlanes(float nearP, float farP) {
        NearPlane = nearP;
        FarPlane  = farP;
    }

    void SetFov(float newFov) {
        Fov = glm::clamp(newFov, 15.0f, 90.0f);
    }

private:
    /**
     * @brief Calculates the Front, Right, and Up vectors from updated Euler angles
     */
    void updateCameraVectors() {
        // Calculate the new Front direction vector from spherical angles
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front   = glm::normalize(front);
        
        // Compute Right and Up vectors using cross products
        // Right = Front x WorldUp (Orthonormal vector pointing to the right of the camera)
        Right = glm::normalize(glm::cross(Front, WorldUp));
        // Up = Right x Front (True camera local up vector)
        Up    = glm::normalize(glm::cross(Right, Front));
    }
};
