#include "HomeRenderer.hpp"

void draw_outliner(Data& data)
{
	ImGuiViewport* vp = ImGui::GetMainViewport();
	float width  = vp->Size.x;
	
    if (!data.is_outliner_visible)
        return;
    ImGui::SetNextWindowPos(ImVec2(0 , 20), ImGuiCond_Always);
    ImGui::SetNextWindowSize(
        ImVec2(width / 9.6 , ImGui::GetIO().DisplaySize.y - 20),
        ImGuiCond_Always
    );
    if (	ImGui::Begin("Outliner", &data.is_outliner_visible, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Outliner content");
    }

    ImGui::End();
}


void draw_proprieties(Data &data)
{
	ImGuiViewport* vp = ImGui::GetMainViewport();
	float width  = vp->Size.x;

	if (!data.is_outliner_visible)
        return;
    ImGui::SetNextWindowPos(ImVec2(width - width / 9.6 , 20), ImGuiCond_Always);
    ImGui::SetNextWindowSize(
        ImVec2(width / 9.6 , ImGui::GetIO().DisplaySize.y - 20),
        ImGuiCond_Always
    );
    if (	ImGui::Begin("Proprieties", &data.is_outliner_visible, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Proprieties content");
    }

    ImGui::End();
}