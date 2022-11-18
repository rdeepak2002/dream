//
// Created by Deepak Ramalingam on 10/21/22.
//

#include "dream/Application.h"

#include "dream/renderer/OpenGLRenderer.h"
#include "dream/editor/ImGuiSDL2OpenGLEditor.h"
#include "dream/window/SDL2OpenGLWindow.h"
#include "dream/scene/component/Component.h"
#include "dream/renderer/OpenGLCubeMesh.h"
#include "dream/renderer/OpenGLTexture.h"
#include <iostream>
#include <filesystem>

namespace Dream {
    Application::Application() {
        Project::open(this->getResourcesRoot().append("examples").append("hello_world"));
        this->window = new SDL2OpenGLWindow();
        this->renderer = new OpenGLRenderer();
        this->editor = new ImGuiSDL2OpenGLEditor(this->window);

        // TODO: load from project's scene file
        auto sphereEntity = Project::getScene()->createEntity("Sphere");
        sphereEntity.addComponent<Component::MeshComponent>(new OpenGLSphereMesh());
        sphereEntity.getComponent<Component::TransformComponent>().translation = {-0.7, -0.7, 0};
        sphereEntity.getComponent<Component::TransformComponent>().scale = {0.4, 0.4, 0.4};

        auto cubeEntity = Project::getScene()->createEntity("Cube");
        cubeEntity.addComponent<Component::MeshComponent>(new OpenGLCubeMesh());
        cubeEntity.addComponent<Component::MaterialComponent>(new OpenGLTexture(Project::getPath().append("assets").append("textures").append("container.jpg")));
        cubeEntity.getComponent<Component::TransformComponent>().translation = {1, 0.7, 0};
        cubeEntity.getComponent<Component::TransformComponent>().scale = {0.4, 0.4, 0.4};

//        Entity teapotEntity = Project::getAssetImporter()->importMesh(Project::getPath().append("assets").append("models").append("teapot.stl"));
//        teapotEntity.getComponent<Component::TransformComponent>().scale = {0.05, 0.05, 0.05};

//        Entity cuteGhostEntity = Project::getAssetImporter()->importMesh(Project::getPath().append("assets").append("models").append("cute-ghost").append("source").append("Ghost.fbx"));
//        if (cuteGhostEntity) {
//            cuteGhostEntity.getComponent<Component::TransformComponent>().translation = {0, 0, 0};
//            cuteGhostEntity.getComponent<Component::TransformComponent>().scale = {0.2, 0.2, 0.2};
//            cuteGhostEntity.getComponent<Component::TransformComponent>().rotation = {0.707, -0.5, 0, 0.0};
//        } else {
//            std::cout << "Error importing ghost model" << std::endl;
//        }

        Entity knightEntity = Project::getAssetImporter()->importMesh(Project::getPath().append("assets").append("models").append("knight").append("scene.gltf"));
        if (knightEntity) {
            knightEntity.getComponent<Component::TransformComponent>().translation = {0, -0.3, 0};
            knightEntity.getComponent<Component::TransformComponent>().scale = {0.007, 0.007, 0.007};
        } else {
            std::cout << "Error importing knight model" << std::endl;
        }

        Entity dragonEntity = Project::getAssetImporter()->importMesh(Project::getPath().append("assets").append("models").append("alduin").append("scene.gltf"));
        if (dragonEntity) {
            dragonEntity.getComponent<Component::TransformComponent>().translation = {0.5, 0, 0};
            dragonEntity.getComponent<Component::TransformComponent>().scale = {0.0025, 0.0025, 0.0025};
            dragonEntity.getComponent<Component::TransformComponent>().rotation = {0.707, 0.707, 0, 0};
        } else {
            std::cout << "Error importing dragon model" << std::endl;
        }
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
