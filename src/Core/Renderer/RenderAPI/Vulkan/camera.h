#include "vk_types.h"
#include "../../../Events/Event.h"

class Camera
{
  public:
    glm::vec3 velocity;
    glm::vec3 position;
    // vertical rotation
    float pitch{0.f};
    // horizontal rotation
    float yaw{0.f};

    glm::mat4 getViewMatrix() const;
    glm::mat4 getRotationMatrix() const;

    void processEvent(Core::Event &e);

    void update();
};