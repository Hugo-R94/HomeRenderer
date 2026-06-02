#include "HomeRenderer.hpp"

/**
 * @brief Point d'entrée principal de l'application HomeRenderer
 * 
 * Initialise:
 * - GLFW et contexte OpenGL 3.3
 * - ImGui avec support du docking
 * - Data (état application)
 * - Boucle principale: événements → UI → rendu → swap
 * 
 * @return 0 si succès, 1 si erreur
 */
// int main()
// {
//     // ============================================================
//     // INITIALIZATION
//     // ============================================================
    
//     if (!glfwInit())
//     {
//         std::cerr << "[ERROR] Impossible d'initialiser GLFW" << std::endl;
//         return 1;
//     }

//     // Initialiser le gestionnaire de données
//     Data data;
//     std::cout << "[INFO] Résolution détectée: " << data.GetResx() 
//               << "x" << data.GetResy() << std::endl;

//     // Créer la fenêtre et initialiser ImGui
//     GLFWwindow* window = init_ImGUI(data);
//     if (!window)
//     {
//         std::cerr << "[ERROR] Impossible de créer la fenêtre GLFW" << std::endl;
//         return 1;
//     }

//     std::cout << "[OK] Application initialisée avec succès" << std::endl;

//     // ============================================================
//     // MAIN LOOP
//     // ============================================================
    
//     while (!glfwWindowShouldClose(window))
//     {
//         // Poll events
//         glfwPollEvents();
//         handle_input(&data);

//         // Start ImGui frame
//         ImGui_ImplOpenGL3_NewFrame();
//         ImGui_ImplGlfw_NewFrame();
//         ImGui::NewFrame();

//         // Update et rendu de l'UI
//         update_data(data);
//         RenderUI(window, data);

//         // Rendu OpenGL
//         ImGui::Render();
//         glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//         ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

//         // Swap buffers
//         glfwSwapBuffers(window);
//     }

//     // ============================================================
//     // CLEANUP
//     // ============================================================
    
//     cleanup_ImGUI(window);
//     std::cout << "[OK] Application terminée proprement" << std::endl;

//     return 0;
// }

// int main(){}

void updateAllMeshes(std::vector<Mesh> meshes)
{
    for (size_t i = 0; i < meshes.size(); i++)
    {
        meshes[i].updateMeshData();
    }
}

Vec2 project(const Vec3& vertex, const Mat4& MVP) {
    Vec4 clip = mulMat4Vec4(MVP, {vertex.x, vertex.y, vertex.z, 1.0f});
    return {clip.x / clip.w, clip.y / clip.w};
}

Mat4 calculateMVP(Mesh mesh, Cam camera, int W, int H)
{
    Mat4 M = modelMatrix(mesh.getPos(), mesh.getRot(), mesh.getMeshScale());
    Mat4 V = viewMatrix(camera.getPos());
    Mat4 P = projectionMatrix(camera.getFov(), (float)W/H, 0.1f, 100.0f);
    Mat4 MVP = multiplyMat4(P, multiplyMat4(V, M)); 
    return MVP;
}

int main(int ac, char **av)
{
    // --- Cube 1x1x1 : 8 vertices locaux ---
    std::vector<Mesh> meshes;
    for (int i = 1; i < ac; i++)
    {
        std::string path = "/home/hugz/Documents/PROJET_PERSO/HomeRenderer/models/";
        path += av[i];
        Mesh mesh(path);
        meshes.push_back(mesh);
    }
    // --- Creation camera et data ---
	Cam camera(120);
	DataGlobal data(1200, 800);

    // --- GLFW + OpenGL ---
    if (!glfwInit()) return -1;
    GLFWwindow* window = glfwCreateWindow(data.getResx(), data.getResy(), "Projection", NULL, NULL);
    if (!window) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);

    //utilisation
    std::cout << "====== KEYS ======\n" << BOLD YELLOW "translate up : up | translate down : down | translate left : left | translat right : right\n" <<
        "rotate left : a | rotate right : d\n" <<
        "zoom in : s | zoom out : w" RESET << std::endl;

    int displaymode = 1;
    double lastTime = glfwGetTime();
    int frameCount = 0;
   glPointSize(5.0f);

   //boucle de rendu et de controle
    while (!glfwWindowShouldClose(window))
    {
        //calcul fps
        double currentTime = glfwGetTime();
        frameCount++;
        if (currentTime - lastTime >= 1.0) {
            std::cout << "FPS: " << frameCount << "\r" << std::flush;
            frameCount = 0;
            lastTime = currentTime;
        }

        //gestion des inputs
        processInput(window, meshes, &displaymode, data);

        //udate meshes and calculate : 
        //maj des coordonnees monde des vertex de chaque mesh
        updateAllMeshes(meshes);

        //couleur arriere plan
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        //repartition du rendu.
        if (displaymode == -1)
        {
            
            for (size_t i = 0; i < meshes.size(); i++)
                renderWireframe(meshes[i], camera, data.getResx(), data.getResy());
        }
        else
        {
            for (size_t i = 0; i < meshes.size(); i++)
                renderAmbiant(meshes[i], camera, data.getResx(), data.getResy());
       }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    //destruction de la fenetre
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

/*
New main 
//setup


*/