#include "HomeRenderer.hpp"

void handle_input(Data *data)
{
	if (ImGui::IsKeyPressed(ImGuiKey_Tab))
	{
		if (data->is_outliner_visible)
			data->is_outliner_visible = false; 
		else
			data->is_outliner_visible = true; 
		std::cout << "outliner visible ? : " << data->is_outliner_visible << std::endl;
	}
}