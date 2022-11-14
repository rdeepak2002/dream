//
// Created by Deepak Ramalingam on 11/13/22.
//

#include "dream/scene/component/Component.h"

namespace Dream::Component {
    TagComponent::TagComponent(std::string tag) {
        this->tag = std::move(tag);
    }
}
