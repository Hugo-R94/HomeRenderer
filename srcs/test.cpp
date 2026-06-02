#include "HomeRenderer.hpp"

/**
 * @brief Initialise GLFW et ImGui pour l'application
 * 
 * @param data Référence aux données application
 * @return Pointeur vers la fenêtre GLFW créée (nullptr en cas d'erreur)
 */
GLFWwindow* init_ImGUI(Data data)
{
    // Créer la fenêtre GLFW
    GLFWwindow* window = glfwCreateWindow(data.getResx(), data.getResy(), 
                                          "HOME RENDERER", nullptr, nullptr);
    if (!window)
    {
        std::cerr << "[ERROR] Impossible de créer la fenêtre GLFW" << std::endl;
        glfwTerminate();
        return nullptr;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);  // VSync activé

    // Initialiser ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;  // Activer le docking
    
    ImGui::StyleColorsDark();

    // Initialiser les backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    std::cout << "[OK] ImGui initialisé (version " << IMGUI_VERSION << ")" << std::endl;

    return window;
}

/**
 * @brief Nettoie les ressources ImGui et GLFW
 * 
 * @param window Pointeur vers la fenêtre GLFW à détruire
 */
void cleanup_ImGUI(GLFWwindow* window)
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    
    if (window)
        glfwDestroyWindow(window);
    
    glfwTerminate();
    std::cout << "[OK] Ressources ImGui et GLFW libérées" << std::endl;
}

/**
 * @brief Met à jour les données application avec l'état ImGui
 * 
 * @param data Référence aux données application à mettre à jour
 */
void update_data(Data& data)
{
    ImGuiViewport* vp = ImGui::GetMainViewport();
    data.SetResx((int)vp->Size.x);
    data.SetResy((int)vp->Size.y);
}
