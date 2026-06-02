#include "HomeRenderer.hpp"

/**
 * @brief Affiche la barre de menu principale
 * 
 * Contient:
 * - Fichier: Nouveau, Ouvrir, Sauvegarder, Quitter
 * - Création: Primitives 3D
 * - Édition: Undo/Redo, Copy/Paste
 * - Préférences: Thèmes d'interface
 * - Aide: Documentation
 * 
 * @param window Pointeur vers la fenêtre GLFW (pour fermeture)
 */
void render_BarMenu(GLFWwindow* window)
{
    if (!ImGui::BeginMainMenuBar())
        return;

    // ====== FICHIER ======
    if (ImGui::BeginMenu("Fichier"))
    {
        if (ImGui::MenuItem("Nouveau", "Ctrl+N"))
        {
            // TODO: Implémenter création nouveau projet
            std::cout << "[UI] Nouveau projet..." << std::endl;
        }
        if (ImGui::MenuItem("Ouvrir...", "Ctrl+O"))
        {
            // TODO: Implémenter ouverture fichier
            std::cout << "[UI] Ouvrir fichier..." << std::endl;
        }
        if (ImGui::MenuItem("Sauvegarder", "Ctrl+S"))
        {
            // TODO: Implémenter sauvegarde
            std::cout << "[UI] Sauvegarder..." << std::endl;
        }
        ImGui::Separator();
        if (ImGui::MenuItem("Quitter"))
        {
            glfwSetWindowShouldClose(window, true);
        }
        ImGui::EndMenu();
    }

    // ====== CRÉATION ======
    if (ImGui::BeginMenu("Creation"))
    {
        if (ImGui::MenuItem("New Cube"))
        {
            // TODO: Créer un cube dans la scène
            std::cout << "[UI] Création cube..." << std::endl;
        }
        if (ImGui::MenuItem("New Sphere"))
        {
            // TODO: Créer une sphère
            std::cout << "[UI] Création sphère..." << std::endl;
        }
        if (ImGui::MenuItem("New Cylinder"))
        {
            // TODO: Créer un cylindre
            std::cout << "[UI] Création cylindre..." << std::endl;
        }
        ImGui::EndMenu();
    }

    // ====== ÉDITION ======
    if (ImGui::BeginMenu("Édition"))
    {
        if (ImGui::MenuItem("Annuler", "Ctrl+Z"))
        {
            // TODO: Undo
        }
        if (ImGui::MenuItem("Rétablir", "Ctrl+Y"))
        {
            // TODO: Redo
        }
        ImGui::Separator();
        if (ImGui::MenuItem("Copier", "Ctrl+C"))
        {
            // TODO: Copy
        }
        if (ImGui::MenuItem("Coller", "Ctrl+V"))
        {
            // TODO: Paste
        }
        ImGui::EndMenu();
    }

    // ====== PRÉFÉRENCES ======
    if (ImGui::BeginMenu("Préférences"))
    {
        static int theme = 1;

        if (ImGui::MenuItem("Dark mode", nullptr, theme == 1))
        {
            theme = 1;
            ImGui::StyleColorsDark();
        }
        if (ImGui::MenuItem("Light mode", nullptr, theme == 2))
        {
            theme = 2;
            ImGui::StyleColorsLight();
        }
        if (ImGui::MenuItem("Classic mode", nullptr, theme == 3))
        {
            theme = 3;
            ImGui::StyleColorsClassic();
        }

        ImGui::EndMenu();
    }

    // ====== AIDE ======
    if (ImGui::BeginMenu("Aide"))
    {
        if (ImGui::MenuItem("Documentation"))
        {
            // TODO: Ouvrir la documentation
            std::cout << "[UI] Documentation..." << std::endl;
        }
        if (ImGui::MenuItem("À propos"))
        {
            // TODO: Afficher dialog 'À propos'
            std::cout << "[UI] À propos de HomeRenderer..." << std::endl;
        }
        ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
}

// ============================================================
// ALIASES POUR COMPATIBILITÉ
// ============================================================

void setup_bar_menu(GLFWwindow* window)
{
    render_BarMenu(window);
}