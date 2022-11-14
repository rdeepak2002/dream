//
// Created by Deepak Ramalingam on 10/21/22.
//

#include "dream/Application.h"

#include "dream/renderer/OpenGLRenderer.h"
#include "dream/editor/ImGuiSDL2OpenGLEditor.h"
#include "dream/window/SDL2OpenGLWindow.h"
#include "dream/scene/component/Component.h"
#include "dream/renderer/OpenGLCubeMesh.h"
#include <iostream>
#include <filesystem>

namespace Dream {
    Application::Application() {
        Project::open(this->getResourcesRoot().append("examples").append("hello_world"));
        this->window = new SDL2OpenGLWindow();
        this->renderer = new OpenGLRenderer();
        this->editor = new ImGuiSDL2OpenGLEditor(this->window);

        // TODO: load from project's scene file
        auto sphereEntity = Project::getInstance().getScene().createEntity("Sphere");
        sphereEntity.addComponent<Component::MeshComponent>(new OpenGLSphereMesh());
        sphereEntity.getComponent<Component::TransformComponent>().translation = {-0.5, -0.7, 0};
        sphereEntity.getComponent<Component::TransformComponent>().scale = {0.4, 0.4, 0.4};
        auto cubeEntity = Project::getInstance().getScene().createEntity("Cube");
        cubeEntity.addComponent<Component::MeshComponent>(new OpenGLCubeMesh());
        cubeEntity.getComponent<Component::TransformComponent>().translation = {1, 0.7, 0};
        cubeEntity.getComponent<Component::TransformComponent>().scale = {0.4, 0.4, 0.4};
        Project::getInstance().getAssetImporter()->importMesh(Project::getPath().append("assets").append("models").append("teapot.stl"));
//        auto teapotEntity = Project::getInstance().getScene().createEntity("Teapot");
//        teapotEntity.getComponent<Component::TransformComponent>().translation = {0, 0, 0};
//        teapotEntity.getComponent<Component::TransformComponent>().scale = {0.05, 0.05, 0.05};
//        teapotEntity.addComponent<Component::MeshComponent>(new OpenGLMesh(Project::getPath().append("assets").append("models").append("teapot.stl")));
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
