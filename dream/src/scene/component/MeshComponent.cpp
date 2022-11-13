//
// Created by Deepak Ramalingam on 11/13/22.
//

#include "dream/scene/component/Component.h"

namespace Dream::Component {
    MeshComponent::MeshComponent(Mesh* meshIn) {
        this->mesh = meshIn;
    }
}
