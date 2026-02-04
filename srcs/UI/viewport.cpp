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
			ImVec2(vp->Size.x - ((vp->Size.x / 9.6) * 2), vp->Size.y - 20),
			ImGuiCond_Always
		);
	}
	else
	{
			ImGui::SetNextWindowPos(ImVec2(0, 20), ImGuiCond_Always);
		ImGui::SetNextWindowSize(
			ImVec2(vp->Size.x, vp->Size.y - 20),
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
