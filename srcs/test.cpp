#include "HomeRenderer.hpp"



GLFWwindow*    init_ImGUI(Data data)
{
   
    GLFWwindow* window = glfwCreateWindow(data.GetResx(), data.GetResy(), "HOME RENDERER", nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        return NULL;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");
    return window;
}

void    cleanup_ImGUI(GLFWwindow* window)
{
    // ===== Cleanup =====
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
}



int main()
{
    if (!glfwInit())
        return 1;
 	
    Data data;
	std::cout << data.GetResx() << std::endl;
	GLFWwindow *window = init_ImGUI(data);
	float my_color[4];
	for (int i = 0; i < 4 ; i++)
		my_color[i] = 0.0f;
	
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
		setup_bar_menu(window);
		
        ImGui::Render();
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }



    cleanup_ImGUI(window);

    return 0;
}
