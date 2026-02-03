#include "HomeRenderer.hpp"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <iostream>

GLFWwindow*    init_ImGUI()
{
   
    GLFWwindow* window = glfwCreateWindow(1920, 1080, "HOME RENDERER", nullptr, nullptr);
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
    GLFWwindow *window = init_ImGUI();
    
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // ===== Fenêtre 1 =====
        ImGui::SetNextWindowPos(ImVec2(50, 50), ImGuiCond_Once);
        ImGui::Begin("Fenêtre 1");
        ImGui::Text("Position 50,50");
        ImGui::End();

        // ===== Fenêtre 2 =====
        ImGui::SetNextWindowPos(ImVec2(300, 50), ImGuiCond_Once);
        ImGui::Begin("Fenêtre 2");
        ImGui::Text("Position 300,50");
        ImGui::End();

        // ===== Fenêtre 3 =====
        ImGui::SetNextWindowPos(ImVec2(550, 50), ImGuiCond_Once);
        ImGui::Begin("Fenêtre 3");
        ImGui::Text("Position 550,50");
        ImGui::End();

        ImGui::Render();
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }



    cleanup_ImGUI(window);

    return 0;
}
