#include "HomeRenderer.hpp"

void	setup_bar_menu(GLFWwindow *window)
{
	if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("Fichier"))
			{
				if (ImGui::MenuItem("Nouveau", "Ctrl+N"))
					std::cout << "NOUVO" << std::endl; //mettre fonction ici 
				if (ImGui::MenuItem("Ouvrir...", "Ctrl+O")) {/*ou la*/}
				if (ImGui::MenuItem("Sauvegarder", "Ctrl+S")) {}
				ImGui::Separator();
				if (ImGui::MenuItem("Quitter"))
					glfwSetWindowShouldClose(window, true);
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Creation"))
			{
				ImGui::MenuItem("New cube");
				ImGui::MenuItem("New sphere");
				ImGui::MenuItem("New cylinder");
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Édition"))
			{
				ImGui::MenuItem("Annuler", "Ctrl+Z");
				ImGui::MenuItem("Rétablir", "Ctrl+Y");
				ImGui::Separator();
				ImGui::MenuItem("Copier", "Ctrl+C");
				ImGui::MenuItem("Coller", "Ctrl+V");
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Préférences"))
			{
				static int theme = 1;

				if (ImGui::MenuItem("Dark mode", nullptr, theme == 1))
					theme = 1;

				if (ImGui::MenuItem("Light mode", nullptr, theme == 2))
					theme = 2;

				if (ImGui::MenuItem("Normal mode", nullptr, theme == 3))
					theme = 3;

				switch (theme)
				{
					case 1:
						ImGui::StyleColorsDark();
						break;
					case 2:
						ImGui::StyleColorsLight();
						break;
					case 3:
						ImGui::StyleColorsClassic();
						break;
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Aide"))
			{
				static bool dark_mode = true;
				if (ImGui::MenuItem("Documentation", NULL, dark_mode))
				{
					dark_mode = !dark_mode;
					if (dark_mode)
						ImGui::StyleColorsDark();
					else
						ImGui::StyleColorsLight();
				}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
}