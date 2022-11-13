//
// Created by Deepak Ramalingam on 10/21/22.
//

#include "dream/Application.h"

#include "dream/renderer/OpenGLRenderer.h"
#include "dream/editor/ImGuiSDL2OpenGLEditor.h"
#include "dream/window/SDL2OpenGLWindow.h"
#include "dream/scene/Entity.h"
#include "dream/scene/Component.h"
#include <iostream>
#include <filesystem>

namespace Dream {
    Application::Application() {
        Project::open(this->getResourcesRoot().append("examples").append("hello_world"));
        this->window = new SDL2OpenGLWindow();
        this->renderer = new OpenGLRenderer();
        this->editor = new ImGuiSDL2OpenGLEditor(this->window);

        // TODO: make this unit test
        auto entityA = Project::getInstance().getScene().createEntity("A");
        auto entityB = Project::getInstance().getScene().createEntity("B");
        auto entityC = Project::getInstance().getScene().createEntity("C");
        // check root entity
        Entity rootEntity = entityA.scene->getRootEntity();
        assert(rootEntity.numChildren() == 3);
        assert(rootEntity.getComponent<Component::HierarchyComponent>().first == entityC);
        assert(rootEntity.getComponent<Component::HierarchyComponent>().first.getComponent<Component::HierarchyComponent>().next == entityB);
        assert(rootEntity.getComponent<Component::HierarchyComponent>().first.getComponent<Component::HierarchyComponent>().next.getComponent<Component::HierarchyComponent>().next == entityA);
        // check entity A
        assert(entityA.numChildren() == 0);
        assert(rootEntity == entityA.getComponent<Component::HierarchyComponent>().parent);
        assert(!entityA.getComponent<Component::HierarchyComponent>().first);
        assert(!entityA.getComponent<Component::HierarchyComponent>().next);
        assert(entityA.getComponent<Component::HierarchyComponent>().prev == entityB);
        // check entity B
        assert(entityB.numChildren() == 0);
        assert(rootEntity == entityB.getComponent<Component::HierarchyComponent>().parent);
        assert(!entityB.getComponent<Component::HierarchyComponent>().first);
        assert(entityB.getComponent<Component::HierarchyComponent>().next == entityA);
        assert(entityB.getComponent<Component::HierarchyComponent>().prev == entityC);
        // check entity C
        assert(entityC.numChildren() == 0);
        assert(rootEntity == entityC.getComponent<Component::HierarchyComponent>().parent);
        assert(!entityC.getComponent<Component::HierarchyComponent>().first);
        assert(entityC.getComponent<Component::HierarchyComponent>().next == entityB);
        assert(!entityC.getComponent<Component::HierarchyComponent>().prev);
        // make entity C child of entity A
        entityA.addChild(entityC);
        // check sizes
        assert(rootEntity.numChildren() == 2);
        assert(entityA.numChildren() == 1);
        assert(entityB.numChildren() == 0);
        // check entity C is only a child of entity A
        assert(entityA.getComponent<Component::HierarchyComponent>().first == entityC);
        // make entity C child of entity B
        entityB.addChild(entityC);
        // check sizes
        assert(rootEntity.numChildren() == 2);
        assert(entityB.numChildren() == 1);
        assert(entityA.numChildren() == 0);
        // check entity C is only a child of entity B
        assert(entityB.getComponent<Component::HierarchyComponent>().first == entityC);

    }

    Application::~Application() {
        delete this->window;
        delete this->renderer;
        delete this->editor;
    }

    void Application::update() {
        this->window->pollEvents();
        this->fixedUpdate();
        std::pair<int, int> rendererViewportDimensions;
        if (fullscreen) {
            rendererViewportDimensions = this->window->getWindowDimensions();
        } else {
            rendererViewportDimensions = this->editor->getRendererViewportDimensions();
        }
        this->renderer->render(rendererViewportDimensions.first, rendererViewportDimensions.second, fullscreen);
        if (!fullscreen) {
            this->editor->update(this->window, this->renderer->getOutputRenderTexture());
        }
        this->window->swapBuffers();
    }

    bool Application::shouldClose() {
        return this->window->shouldClose();
    }

    void Application::fixedUpdate() {

    }

    std::filesystem::path Application::getResourcesRoot() {
        if (std::filesystem::exists(std::filesystem::current_path().append("examples"))) {
            return std::filesystem::current_path();
        }
        // try to find location of examples folder for desktop debug build
        auto examplesFolder = std::filesystem::current_path()
                .append("..")
                .append("..")
                .append("..")
                .append("examples");
        if (std::filesystem::exists(examplesFolder)) {
            return examplesFolder.append("..");
        }
        fprintf(stderr, "Error: unable to find examples folder\n");
        exit(EXIT_FAILURE);
    }
}
