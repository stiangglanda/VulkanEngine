#include "camera.h"
#include "../../../Events/KeyEvent.h"
#include "../../../Events/MouseEvent.h"
#include "../../../Input.h"
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>

void Camera::ProcessMouseMovement(float xpos, float ypos, bool constrainPitch)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw += xoffset;
    Pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch)
    {
        if (Pitch > 89.0f)
            Pitch = 89.0f;
        if (Pitch < -89.0f)
            Pitch = -89.0f;
    }
}

void Camera::processEvent(float delta)
{
    float velocity = MovementSpeed * delta;

    if (Core::Input::IsKeyPressed(Core::Key::W))
    {
        Position += Front * velocity;
    }
    if (Core::Input::IsKeyPressed(Core::Key::S))
    {
        Position -= Front * velocity;
    }
    if (Core::Input::IsKeyPressed(Core::Key::A))
    {
        Position -= Right * velocity;
    }
    if (Core::Input::IsKeyPressed(Core::Key::D))
    {
        Position += Right * velocity;
    }

    glm::vec2 pos = Core::Input::GetMousePosition();
    ProcessMouseMovement(pos.x, pos.y);
    update();
}

void Camera::ProcessMouseScroll(float yoffset)
{
    Zoom -= (float)yoffset;
    if (Zoom < 1.0f)
        Zoom = 1.0f;
    if (Zoom > 45.0f)
        Zoom = 45.0f;
}

void Camera::update()
{
    // calculate the new Front vector
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);
    // also re-calculate the Right and Up vector
    Right =
        glm::normalize(glm::cross(Front, WorldUp)); // normalize the vectors, because their length gets closer to 0 the
                                                    // more you look up or down which results in slower movement.
    Up = glm::normalize(glm::cross(Right, Front));
}
