#include "imgui.h"
#include <GL/gl.h>   // OpenGL
#include <cstdint>   // intptr_t
#include "HomeRenderer.hpp"

void display_viewport(Data &data)
{
    ImGuiViewport* vp = ImGui::GetMainViewport();

    // Position et taille du viewport
	if(data.is_outliner_visible)
	{
		ImGui::SetNextWindowPos(ImVec2(vp->Size.x / 9.6, 20), ImGuiCond_Always);
		ImGui::SetNextWindowSize(
			ImVec2((vp->Size.x - ((vp->Size.x / 9.6) * 2)) / 2, vp->Size.y - 20),
			ImGuiCond_Always
		);
	}
	else
	{
			ImGui::SetNextWindowPos(ImVec2(0, 20), ImGuiCond_Always);
		ImGui::SetNextWindowSize(
			ImVec2(vp->Size.x / 2, vp->Size.y - 20),
			ImGuiCond_Always
		);
	}
    ImGui::Begin("Viewport", nullptr,
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize);

    // Taille disponible dans la fenêtre ImGui
    ImVec2 size = ImGui::GetContentRegionAvail();

    // ---- Rendu OpenGL simple ----
    glViewport(1, 0, (int)size.x, (int)size.y);

    // Couleur unie
    glClearColor(01.2f, 0.3f, 0.4f, 1.0f); // bleu-gris
    glClear(GL_COLOR_BUFFER_BIT);

    // Optionnel : si tu avais une texture brute 2D
    // GLuint texture_id = ...;
    // ImGui::Image((void*)(intptr_t)texture_id, size);

    // Affiche "le rendu" dans ImGui
    ImGui::Image(
        (void*)(intptr_t)0, // On met 0 car on utilise juste le backbuffer pour l'instant
        size
    );

    ImGui::End();
}

void display_double_viewport(Data &data)
{
    ImGuiViewport* vp = ImGui::GetMainViewport();
    float screen_w = vp->Size.x;
    float screen_h = vp->Size.y - 20; // garder top bar

    float offset_x = data.is_outliner_visible ? screen_w / 9.6f : 0.0f;
    float width_available = screen_w - 2 * offset_x;

    static float split = 0.7f; // proportion des viewports
ImGui::InvisibleButton("splitter", ImVec2(5, screen_h));
	if (ImGui::IsItemActive())
	{
		split += ImGui::GetIO().MouseDelta.x / width_available;
		if (split < 0.1f) split = 0.1f;
		if (split > 0.9f) split = 0.9f;
	}
    float width_vp1 = width_available * split;
    float width_vp2 = width_available * (1.0f - split);

    ImVec2 pos( offset_x, 20 );
    ImVec2 size1(width_vp1, screen_h);
    ImVec2 size2(width_vp2, screen_h);

    // ---------------- Viewport 1 ----------------
    ImGui::SetNextWindowPos(pos, ImGuiCond_Appearing);
    ImGui::SetNextWindowSize(size1, ImGuiCond_Always);
    ImGui::Begin("Viewport 1", nullptr,
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

    ImVec2 avail1 = ImGui::GetContentRegionAvail();
    glViewport(0, 0, (int)avail1.x, (int)avail1.y);
    glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui::Image((void*)(intptr_t)0, avail1);

    ImGui::End();

    // ---------------- Viewport 2 ----------------
    ImGui::SetNextWindowPos(ImVec2(pos.x + width_vp1, 20), ImGuiCond_Always);
    ImGui::SetNextWindowSize(size2, ImGuiCond_Always);
    ImGui::Begin("Viewport 2", nullptr,
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize );

    ImVec2 avail2 = ImGui::GetContentRegionAvail();
    glViewport(0, 0, (int)avail2.x, (int)avail2.y);
    glClearColor(0.4f, 0.2f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui::Image((void*)(intptr_t)0, avail2);
	

    ImGui::End();
}
