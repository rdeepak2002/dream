//
// Created by Deepak Ramalingam on 11/27/22.
//

#include "dream/editor/ImGuiTextEditor.h"
#include <fstream>
#include <sstream>
#include <iostream>

namespace Dream {
    ImGuiTextEditor::ImGuiTextEditor() {
        this->textEditor = new TextEditor();
        this->visible = false;
    }

    ImGuiTextEditor::~ImGuiTextEditor() {
        delete this->textEditor;
    }

    void ImGuiTextEditor::open(const std::string& filepath) {
        path = std::filesystem::path(filepath);
        if (path.extension() == ".lua") {
            std::cout << "detected lua" << std::endl;
            textEditor->SetLanguageDefinition(TextEditor::LanguageDefinition::Lua());
        } else if (path.extension() == ".vert" || path.extension() == ".frag") {
            textEditor->SetLanguageDefinition(TextEditor::LanguageDefinition::GLSL());
        } else {
            textEditor->SetLanguageDefinition(TextEditor::LanguageDefinition::CPlusPlus());
        }

        if (path.extension() == ".py" || path.extension() == ".scene" || path.extension() == ".yml") {
            this->textEditor->SetShowWhitespaces(true);
        } else {
            this->textEditor->SetShowWhitespaces(false);
        }
        filename = path.filename();
        std::string fileContent;
        this->setVisibility(true);
        std::ifstream file;
        file.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        try {
            // open files
            file.open(filepath);
            std::stringstream fileStream;
            // read file's buffer contents into streams
            fileStream << file.rdbuf();
            // close file handlers
            file.close();
            // convert stream into string
            fileContent = fileStream.str();
            textEditor->SetText(fileContent);
            justLoaded = true;
        }
        catch (std::ifstream::failure& e) {
            std::cout << "Error reading file: " << e.what() << std::endl;
        }
    }

    void ImGuiTextEditor::save() {
        this->setVisibility(true);
    }

    void ImGuiTextEditor::loadEmptyFile() {
        this->setVisibility(true);
        this->justLoaded = true;
    }

    void ImGuiTextEditor::update() {
        if (textEditor->IsTextChanged() && !justLoaded) {
            unsavedChanges = true;
        }

        if (visible) {
            ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(500, 500));
            ImGui::Begin(filename.c_str(), nullptr, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse);

            if (ImGui::BeginMenuBar()) {
                if (ImGui::BeginMenu("File")) {
                    if (ImGui::MenuItem("Save")) {
                        std::ofstream fout(path);
                        fout << textEditor->GetText().c_str();
                        unsavedChanges = false;
                    }
                    if (ImGui::MenuItem("Close")) {
                        this->setVisibility(false);
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMenuBar();
            }

            textEditor->SetPalette(TextEditor::GetLightPalette());
            ImGui::SetWindowFontScale(1.1);
            textEditor->Render("TextEditor");

            ImGui::End();
            ImGui::PopStyleVar(1);
        }

        justLoaded = false;
    }

    void ImGuiTextEditor::setVisibility(bool isVisible) {
        this->visible = isVisible;
    }
}
