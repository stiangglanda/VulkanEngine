#include "camera.h"
#include "../../../Events/KeyEvent.h"
#include "../../../Events/MouseEvent.h"
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>

// glm::mat4 Camera::getViewMatrix() const
//{
//     // to create a correct model view, we need to move the world in opposite
//     // direction to the camera
//     //  so we will create the camera model matrix and invert
//     glm::mat4 cameraTranslation = glm::translate(glm::mat4(1.f), position);
//     glm::mat4 cameraRotation = getRotationMatrix();
//     return glm::inverse(cameraTranslation * cameraRotation);
// }
//
// glm::mat4 Camera::getRotationMatrix() const
//{
//     // fairly typical FPS style camera. we join the pitch and yaw rotations into
//     // the final rotation matrix
//
//     glm::quat pitchRotation = glm::angleAxis(pitch, glm::vec3{1.f, 0.f, 0.f});
//     glm::quat yawRotation = glm::angleAxis(yaw, glm::vec3{0.f, -1.f, 0.f});
//
//     return glm::toMat4(yawRotation) * glm::toMat4(pitchRotation);
// }

void Camera::processEvent(Core::Event &e, float delta)
{
    if (e.IsInCategory(Core::EventCategory::EventCategoryKeyboard))
    {
        if (e.GetEventType() == Core::EventType::KeyPressed)
        {
            Camera_Movement cm = Camera_Movement::NONE;
            if (static_cast<Core::KeyPressedEvent &>(e).GetKeyCode() == Core::Key::W)
            {
                cm = Camera_Movement::FORWARD;
                // velocity.z = -1;
                e.Handled = true;
            }
            if (static_cast<Core::KeyPressedEvent &>(e).GetKeyCode() == Core::Key::S)
            {
                cm = Camera_Movement::BACKWARD;
                // velocity.z = 1;
                e.Handled = true;
            }
            if (static_cast<Core::KeyPressedEvent &>(e).GetKeyCode() == Core::Key::A)
            {
                cm = Camera_Movement::LEFT;
                // velocity.x = -1;
                e.Handled = true;
            }
            if (static_cast<Core::KeyPressedEvent &>(e).GetKeyCode() == Core::Key::D)
            {
                cm = Camera_Movement::RIGHT;
                // velocity.x = 1;
                e.Handled = true;
            }
            ProcessKeyboard(cm, delta); // TODO this needs delta
        }
        // if (e.GetEventType() == Core::EventType::KeyReleased)
        //{
        //     if (static_cast<Core::KeyReleasedEvent &>(e).GetKeyCode() == Core::Key::W)
        //     {
        //         velocity.z = 0;
        //         e.Handled = true;
        //     }
        //     if (static_cast<Core::KeyReleasedEvent &>(e).GetKeyCode() == Core::Key::S)
        //     {
        //         velocity.z = 0;
        //         e.Handled = true;
        //     }
        //     if (static_cast<Core::KeyReleasedEvent &>(e).GetKeyCode() == Core::Key::A)
        //     {
        //         velocity.x = 0;
        //         e.Handled = true;
        //     }
        //     if (static_cast<Core::KeyReleasedEvent &>(e).GetKeyCode() == Core::Key::D)
        //     {
        //         velocity.x = 0;
        //         e.Handled = true;
        //     }
        // }
    }
    if (e.IsInCategory(Core::EventCategory::EventCategoryMouse))
    {
        if (e.GetEventType() == Core::EventType::MouseMoved)
        {
            ProcessMouseMovement(static_cast<Core::MouseMovedEvent &>(e).GetX(),
                                 static_cast<Core::MouseMovedEvent &>(e).GetY());
            e.Handled = true;
        }
    }
}

// void Camera::processSDLEvent(SDL_Event &e)
//{
//     if (e.type == SDL_KEYDOWN)
//     {
//         if (e.key.keysym.sym == SDLK_w)
//         {
//             velocity.z = -1;
//         }
//         if (e.key.keysym.sym == SDLK_s)
//         {
//             velocity.z = 1;
//         }
//         if (e.key.keysym.sym == SDLK_a)
//         {
//             velocity.x = -1;
//         }
//         if (e.key.keysym.sym == SDLK_d)
//         {
//             velocity.x = 1;
//         }
//     }
//
//     if (e.type == SDL_KEYUP)
//     {
//         if (e.key.keysym.sym == SDLK_w)
//         {
//             velocity.z = 0;
//         }
//         if (e.key.keysym.sym == SDLK_s)
//         {
//             velocity.z = 0;
//         }
//         if (e.key.keysym.sym == SDLK_a)
//         {
//             velocity.x = 0;
//         }
//         if (e.key.keysym.sym == SDLK_d)
//         {
//             velocity.x = 0;
//         }
//     }
//
//     if (e.type == SDL_MOUSEMOTION)
//     {
//         yaw += (float)e.motion.xrel / 200.f;
//         pitch -= (float)e.motion.yrel / 200.f;
//     }
// }

// void Camera::update()
//{
//     glm::mat4 cameraRotation = getRotationMatrix();
//     position += glm::vec3(cameraRotation * glm::vec4(velocity * 0.5f, 0.f));
// }
