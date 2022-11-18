//
// Created by Deepak Ramalingam on 11/18/22.
//

#include <utility>

#include "dream/scene/component/Component.h"
#include "dream/project/IDUtils.h"

namespace Dream::Component {
    IdComponent::IdComponent() {
        this->id = IDUtils::newGUID();
    }

    IdComponent::IdComponent(std::string id) {
        this->id = std::move(id);
    }

    std::string IdComponent::getID() {
        return this->id;
    }
}