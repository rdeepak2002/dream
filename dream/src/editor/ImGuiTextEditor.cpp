//
// Created by Deepak Ramalingam on 11/27/22.
//

#include "dream/editor/ImGuiTextEditor.h"

namespace Dream {
    ImGuiTextEditor::ImGuiTextEditor() {
        this->textEditor = new TextEditor();
        this->visible = true;
    }

    ImGuiTextEditor::~ImGuiTextEditor() {
        delete this->textEditor;
    }

    void ImGuiTextEditor::open(std::string filepath) {
        this->setVisibility(true);
    }

    void ImGuiTextEditor::save() {
        this->setVisibility(true);
    }

    void ImGuiTextEditor::loadEmptyFile() {
        this->setVisibility(true);
    }

    void ImGuiTextEditor::render() {
        if (visible) {
            ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(500, 500));
            ImGui::Begin("filename", nullptr, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse);

            if (ImGui::BeginMenuBar()) {
                if (ImGui::BeginMenu("File")) {
                    if (ImGui::MenuItem("Save")) {
                        // TODO: save file
                    }
                    ImGui::EndMenu();
                    if (ImGui::MenuItem("Open")) {
                        // TODO: open file
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMenuBar();
            }

            textEditor->SetPalette(TextEditor::GetDarkPalette());
            textEditor->Render("TextEditor");
            textEditor->SetLanguageDefinition(TextEditor::LanguageDefinition::Lua());

            ImGui::End();
            ImGui::PopStyleVar(1);
        }
    }

    void ImGuiTextEditor::setVisibility(bool isVisible) {
        this->visible = isVisible;
    }
}
