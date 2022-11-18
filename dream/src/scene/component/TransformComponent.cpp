//
// Created by Deepak Ramalingam on 11/13/22.
//

#include "dream/scene/component/Component.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Dream::Component {
    glm::mat4 TransformComponent::getTransform(Entity &curEntity) {
        glm::mat4 rotMat4 = glm::toMat4(glm::quat(rotation));
        glm::mat4 model = glm::translate(glm::mat4(1.0f), translation) * rotMat4 * glm::scale(glm::mat4(1.0f), scale);
        glm::mat4 parentModel = glm::mat4(1.0);
        Entity parent = curEntity.getComponent<HierarchyComponent>().parent;
        if (parent) {
            parentModel = parent.getComponent<TransformComponent>().getTransform(parent);
        }
        return model * parentModel;
    }
}