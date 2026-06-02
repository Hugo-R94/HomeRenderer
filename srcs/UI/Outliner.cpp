#include "HomeRenderer.hpp"

/**
 * @brief Affiche le panneau Outliner (hiérarchie des objets)
 * 
 * Position: Gauche de l'écran (~10.4% de largeur)
 * Hauteur: Pleine hauteur moins la barre de menu
 * Contient: Arborescence des objets de la scène
 * 
 * @param data Référence aux données application
 */
void render_outliner(Data& data)
{
    if (!data.is_outliner_visible)
        return;

    ImGuiViewport* vp = ImGui::GetMainViewport();
    float width = vp->Size.x;
    float height = vp->Size.y;
    
    // Panneau sur le côté gauche (~10.4% de largeur)
    ImGui::SetNextWindowPos(ImVec2(0, 20), ImGuiCond_Always);
    ImGui::SetNextWindowSize(
        ImVec2(width / 9.6f, height - 20),
        ImGuiCond_Always
    );
    
    if (ImGui::Begin("Outliner", &data.is_outliner_visible,
                     ImGuiWindowFlags_NoCollapse |
                     ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_NoResize))
    {
        ImGui::TextWrapped("Outliner");
        ImGui::Separator();
        
        // TODO: Implémenter arborescence d'objets
        // Exemple:
        // if (ImGui::TreeNodeEx("Scène", ImGuiTreeNodeFlags_DefaultOpen)) {
        //     ImGui::TreeNodeEx("Cube", ImGuiTreeNodeFlags_Leaf);
        //     ImGui::TreeNodeEx("Sphère", ImGuiTreeNodeFlags_Leaf);
        //     ImGui::TreePop();
        // }
    }
    ImGui::End();
}

/**
 * @brief Affiche le panneau Properties (propriétés des objets)
 * 
 * Position: Droite de l'écran (~10.4% de largeur)
 * Hauteur: Pleine hauteur moins la barre de menu
 * Contient: Propriétés de l'objet sélectionné
 * 
 * @param data Référence aux données application
 */
void render_proprieties(Data& data)
{
    if (!data.is_outliner_visible)
        return;

    ImGuiViewport* vp = ImGui::GetMainViewport();
    float width = vp->Size.x;
    float height = vp->Size.y;
    
    // Panneau sur le côté droit (~10.4% de largeur)
    ImGui::SetNextWindowPos(ImVec2(width - width / 9.6f, 20), ImGuiCond_Always);
    ImGui::SetNextWindowSize(
        ImVec2(width / 9.6f, height - 20),
        ImGuiCond_Always
    );
    
    if (ImGui::Begin("Properties", &data.is_outliner_visible,
                     ImGuiWindowFlags_NoCollapse |
                     ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_NoResize))
    {
        ImGui::TextWrapped("Properties");
        ImGui::Separator();
        
        // TODO: Afficher propriétés de l'objet sélectionné
        // Exemple:
        // ImGui::Text("Position");
        // ImGui::SliderFloat3("##pos", position, -10.0f, 10.0f);
        // ImGui::Text("Rotation");
        // ImGui::SliderFloat3("##rot", rotation, -3.14f, 3.14f);
        // ImGui::Text("Scale");
        // ImGui::SliderFloat3("##scale", scale, 0.1f, 10.0f);
    }
    ImGui::End();
}

// ============================================================
// ALIASES POUR COMPATIBILITÉ
// ============================================================

void draw_outliner(Data& data)
{
    render_outliner(data);
}

void draw_proprieties(Data& data)
{
    render_proprieties(data);
}