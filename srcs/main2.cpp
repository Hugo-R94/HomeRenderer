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

void updateAllMeshes(std::vector<Mesh> &meshes)
{
    for (size_t i = 0; i < meshes.size(); i++)
    {
        meshes[i].updateMeshData();
    }
}

Vec2 project(const Vec3& vertex, const Mat4& MVP) {
    Vec4 clip = mulMat4Vec4(MVP, {vertex.x, vertex.y, vertex.z, 1.0f});
    return {(clip.x / clip.w), (clip.y / clip.w)};  // ← inverser X et Y
}

// int main(int ac, char **av)
// {
//     std::vector<Mesh> meshes;
//     for (int i = 1; i < ac; i++)
//     {
//         std::string path = "./models/";
//         path += av[i];
//         meshes.push_back(Mesh(path));
//     }

//     Cam camera(120);
//     DataGlobal data(1920, 1080);
//     camera.setCamW(data.getResx());
//     camera.setCamH(data.getResy());

// 	if (!glfwInit()) return -1;
//     GLFWwindow* window = glfwCreateWindow(data.getResx(), data.getResy(), "Projection", NULL, NULL);
//     if (!window) { glfwTerminate(); return -1; }
//     glfwMakeContextCurrent(window);
//     // Init framebuffer et zbuffer
//     camera.getFramebuffer().resize(data.getResx() * data.getResy() * 3, {0, 0, 0});
//     camera.getZbuffer().resize(data.getResx() * data.getResy(), 99999.0f);
//     meshes[0].setPos({0.2, -0.1, 14.5});
//     // Texture OpenGL pour afficher le framebuffer
//     GLuint tex;
//     glGenTextures(1, &tex);
//     glBindTexture(GL_TEXTURE_2D, tex);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, data.getResx(), data.getResy(), 0, GL_RGB, GL_UNSIGNED_BYTE, camera.getFramebuffer().data());
//     glEnable(GL_TEXTURE_2D);

//     int displaymode = 1;
//     double lastTime = glfwGetTime();
//     int frameCount = 0;

//     while (!glfwWindowShouldClose(window))
//     {
//         // FPS
//         double currentTime = glfwGetTime();
//         frameCount++;
//         if (currentTime - lastTime >= 1.0) {
//             // std::cout << "pos = " << meshes[0].getPos().x << "|" << meshes[0].getPos().y << "|" << meshes[0].getPos().z << "   |   FPS: " << frameCount << "\r"<< std::flush;
//             frameCount = 0;
//             lastTime = currentTime;
//         }

//         processInput(window, meshes, &displaymode, data);
//         updateAllMeshes(meshes);
//         std::vector<Mat4> MVPs = calculateMVPs(meshes, camera, data.getResx(), data.getResy());
//         // Reset framebuffer et zbuffer
//         std::fill(camera.getFramebuffer().begin(), camera.getFramebuffer().end(), Pixel{0, 0, 0}); // noir pas 25
//         std::fill(camera.getZbuffer().begin(), camera.getZbuffer().end(), 9999.0f);
		
//         for (size_t i = 0; i < meshes.size(); i++)
//             rasterizeMesh(meshes[i], camera, MVPs[i]);
//         // Upload et affiche
//         glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, data.getResx(), data.getResy(), GL_RGB, GL_UNSIGNED_BYTE, camera.getFramebuffer().data());
//         glClear(GL_COLOR_BUFFER_BIT);
//         glBegin(GL_QUADS);
//             glTexCoord2f(0, 1); glVertex2f(-1, -1);
//             glTexCoord2f(1, 1); glVertex2f( 1, -1);
//             glTexCoord2f(1, 0); glVertex2f( 1,  1);
//             glTexCoord2f(0, 0); glVertex2f(-1,  1);
//         glEnd();

//         glfwSwapBuffers(window);
//         glfwPollEvents();
//     }

//     glfwDestroyWindow(window);
//     glfwTerminate();
//     return 0;
// }

