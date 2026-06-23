#include "HomeRenderer.hpp"

/**
 * @brief Traite les entrées clavier et souris
 * 
 * Raccourcis clavier:
 * - Tab: Toggle Outliner/Properties visibility
 * 
 * @param data Pointeur vers les données application à modifier
 */

void processInput(GLFWwindow *window, std::vector<Mesh> &meshes,
                  int *displaymode, DataGlobal &data)
{
    if (meshes.empty())
        return;

    for(size_t i = 0; i < meshes.size(); i++)
    {
        Mesh &mesh = meshes[i];

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        // Déplacement
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
            mesh.setPos({mesh.getPos().x,
                        mesh.getPos().y + 0.1f,
                        mesh.getPos().z});

        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
            mesh.setPos({mesh.getPos().x,
                        mesh.getPos().y - 0.1f,
                        mesh.getPos().z});

        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
            mesh.setPos({mesh.getPos().x - 0.1f,
                        mesh.getPos().y,
                        mesh.getPos().z});

        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
            mesh.setPos({mesh.getPos().x + 0.1f,
                        mesh.getPos().y,
                        mesh.getPos().z});

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            mesh.setPos({mesh.getPos().x,
                        mesh.getPos().y,
                        mesh.getPos().z + 0.1f});

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            mesh.setPos({mesh.getPos().x,
                        mesh.getPos().y,
                        mesh.getPos().z - 0.1f});

        // Rotation
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            mesh.setRot({mesh.getRot().x,
                        mesh.getRot().y + 0.05f,
                        mesh.getRot().z});

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            mesh.setRot({mesh.getRot().x,
                        mesh.getRot().y - 0.05f,
                        mesh.getRot().z});

        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
            mesh.setRot({mesh.getRot().x - 0.05f,
                        mesh.getRot().y,
                        mesh.getRot().z});

        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
            mesh.setRot({mesh.getRot().x + 0.05f,
                        mesh.getRot().y,
                        mesh.getRot().z});
    }
    static bool mLastPressed = false;
    static bool tabLastPressed = false;

    // Changement de mode d'affichage
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
    {
        if (!mLastPressed)
        {
            *displaymode *= -1;
            std::cout << "Switch mode : " << *displaymode << std::endl;
        }
        mLastPressed = true;
    }
    else
        mLastPressed = false;

    // Changement de mesh
    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS)
    {
        if (!tabLastPressed)
        {
            data.setMeshID((data.getMeshID() + 1) % meshes.size());

            std::cout << "Mesh ID selected : "
                      << data.getMeshID()
                      << std::endl;
        }
        tabLastPressed = true;
    }
    else
        tabLastPressed = false;
}