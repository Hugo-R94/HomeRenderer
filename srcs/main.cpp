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

void updateAllMeshes(std::vector<Mesh> &meshes)
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

void backFaceCulling(Mesh& mesh, const Cam& camera)
{
    const std::vector<Face>& faces = mesh.getFaces();
    const std::vector<Vec3>& v = mesh.getWorldVertices();

    std::vector<float> dotlist;
    std::vector<Face> visible;
    dotlist.reserve(faces.size());
    visible.reserve(faces.size());

    Mat4 R = multiplyMat4(rotationZ4(mesh.getRot().z), 
                          multiplyMat4(rotationY4(mesh.getRot().y), 
                                       rotationX4(mesh.getRot().x)));

    for (size_t i = 0; i < faces.size(); i++)
    {
        const Face& f = faces[i];
        Vec3 a = v[f.i[0].v];

        Vec4 n4 = mulMat4Vec4(R, {f.LocalNormal.x, f.LocalNormal.y, f.LocalNormal.z, 0.0f});
        Vec3 normal = {n4.x, n4.y, n4.z};

        Vec3 toCamera = {
            camera.getPos().x - a.x,
            camera.getPos().y - a.y,
            camera.getPos().z - a.z
        };

        float dot = normal.x*toCamera.x
                  + normal.y*toCamera.y
                  + normal.z*toCamera.z;

        dotlist.push_back(dot);

        if (dot > 0)
            visible.push_back(f);
    }

    mesh.setDotFace(dotlist);   // ← manquait
    mesh.setCulledFaces(visible);
}

// Passer par référence !
std::vector<Mat4> calculateMVPs(std::vector<Mesh>& meshes, Cam camera, int W, int H)
{
    std::vector<Mat4> MVPs;
    Mat4 V = viewMatrix(camera.getPos());
    Mat4 P = projectionMatrix(camera.getFov(), (float)W / H, 0.1f, 100.0f);

    for (size_t i = 0; i < meshes.size(); i++)
    {
        backFaceCulling(meshes[i], camera); // modifie le vrai mesh
        Mat4 M = modelMatrix(meshes[i].getPos(), meshes[i].getRot(), meshes[i].getMeshScale());
        MVPs.push_back(multiplyMat4(P, multiplyMat4(V, M)));
    }
    return MVPs;
}
// static Vec3 rotatedVertex(const Mat4& M, const Vec3& v)
// {
//     Vec4 r = mulMat4Vec4(M, {v.x, v.y, v.z, 0.0f});
//     return {r.x, r.y, r.z};
// }
static Vec3 rotatedVertex(const Mat4& M, const Vec3& v)
{
    Vec4 r = mulMat4Vec4(M, {v.x, v.y, v.z, 0.0f});
    return {r.x, r.y, r.z};
}
//render avec rasterisation
std::vector<Triangle2D> renderAmbiant2(Mesh mesh, Cam camera, int W, int H, Mat4 MVP)
{
    (void)H; (void)W;
    // mesh.updateMeshData();
    // std::vector<vec2> projected =
    // std::vector<Vec2> projected = ProjectVertices(mesh.getWorldVertices(), camera);
    std::vector<Triangle2D>tris;
    const std::vector<Face>& faces = mesh.getFaces();
    for (size_t i = 0; i < faces.size(); i++)
    {
        int ia = faces[i].i[0].v;
        int ib = faces[i].i[1].v;
        int ic = faces[i].i[2].v;

        Vec3 a = (mesh.getWorldVertices())[ia];

        // utilise la normale locale de la face
        // Vec3 normal = faces[i].LocalNormal;
        // applique la rotation du mesh a la normale
        Vec3 normal = faces[i].LocalNormal;
        // applique la rotation du mesh a la normale
        normal = rotatedVertex(MVP, normal);

        Vec3 toCamera = {camera.getPos().x-a.x, camera.getPos().y-a.y, camera.getPos().z-a.z};
        if (normal.z >= 0) continue;
        float dot = normal.x*toCamera.x + normal.y*toCamera.y + normal.z*toCamera.z;

        const std::vector<Vec3>& localVerts = mesh.getVerticesLocal();

        Vec2 pa = project(localVerts[ia], MVP);
        Vec2 pb = project(localVerts[ib], MVP);
        Vec2 pc = project(localVerts[ic], MVP);
        Triangle2D tri;
        tri.a.x = pa.x;
        tri.a.y = pa.y;
        tri.a.depth = localVerts[ia].z;

        tri.b.x = pb.x;
        tri.b.y = pb.y;
        tri.b.depth = localVerts[ib].z;

        tri.c.x = pc.x;
        tri.c.y = pc.y;
        tri.c.depth = localVerts[ic].z;
		tri.dot = dot;
        tris.push_back(tri);
    }
    return tris;
}

float edgeFunction (Vertex2D a, Vertex2D b, Vertex2D c)
{
  return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
};

// void rasterizeTriangle(std::vector<Tri> triangles, Cam camera)
// {
//     for (size_t i = 0; i < triangles.size(); i++)
//     {
//         if()
//     }
// }



//fonction pour rasteriser un seul triangle calcul des coordonnee baricentrique w
void rasterizeTriangle(Triangle2D triangle, int H, int W, std::vector<Pixel>& framebuffer, std::vector<float> &zbuffer)
{
    int minX = std::max(0,   (int)((std::min({triangle.a.x, triangle.b.x, triangle.c.x}) * 0.5f + 0.5f) * W));
    int maxX = std::min(W-1, (int)((std::max({triangle.a.x, triangle.b.x, triangle.c.x}) * 0.5f + 0.5f) * W));
    int minY = std::max(0,   (int)((std::min({triangle.a.y, triangle.b.y, triangle.c.y}) * 0.5f + 0.5f) * H));
    int maxY = std::min(H-1, (int)((std::max({triangle.a.y, triangle.b.y, triangle.c.y}) * 0.5f + 0.5f) * H));

    float area = edgeFunction(triangle.a, triangle.b, triangle.c);
    if (area == 0) return;

    for (int y = minY; y <= maxY; y++)
    {
        for (int x = minX; x <= maxX; x++)
        {
            // We can interpolate attributes using the barycentric coordinates
            Pixel colourA = {255, 0, 0}; // Red
            Pixel colourB = {0, 255, 0}; // Red
            Pixel colourC = {0, 0, 255}; // Red

            // We can now calculate the interpolated colour of the point P
            // const g = colourA.g * w0 + colourB.g * w1 + colourC.g * w2;
// k            const b = colourA.b * w0 + colourB.b * w1 + colourC.b * w2;
            Vertex2D p;
            p.x = (x + 0.5f) / W * 2.0f - 1.0f;
            p.y = (y + 0.5f) / H * 2.0f - 1.0f;

            float w0 = edgeFunction(triangle.a, triangle.b, p);
            float w1 = edgeFunction(triangle.b, triangle.c, p);
            float w2 = edgeFunction(triangle.c, triangle.a, p);

            if (w0 >= 0 && w1 >= 0 && w2 >= 0)
            {
                // Coordonnées barycentriques
                float b0 = w1 / area;
                float b1 = w2 / area;
                float b2 = w0 / area;

                // Interpolation de la profondeur
                float depth = b0 * triangle.a.depth
                            + b1 * triangle.b.depth
                            + b2 * triangle.c.depth;
							
    		    // float dot = normal.x*toCamera.x + normal.y*toCamera.y + normal.z*toCamera.z;

				// Normal.z varie entre -1 et 1, on le ramène à [0, 1]
				// Direction de lumière (à ajuster selon ta scène)

				// float shade = (triangle.normal.z + 1.0f) / 2.0f;
                // float shade = 1.0f - std::max(0.0f, std::min(1.0f, (depth + 5.0f) / 10.0f));
				// std::cout << shade << std::endl;
                // if (x % 3 == 0 && y %3 == 0)
                //     std::cout << "barycentric coordinate : w0 = " << w0 << " | w1 = " << w1 << " | w2 = " << w2  <<  std::endl;
				
                // int idx = (y * W + x);
                // utiliser les coordonnees barycentrique pour calculer la moyenne pondere de la depth 
       			float shade = triangle.dot / (0.55 * 0.55) + 0.1;

                int idx = (H - 1 - y) * W + x;
                if(depth < zbuffer[idx])
                {
                    // framebuffer[idx].r =  (colourA.r * w0 + colourB.r * w1 + colourC.r * w2) * shade;
                    // framebuffer[idx].g = (colourA.g * w0 + colourB.g * w1 + colourC.g * w2) * shade;
                    // framebuffer[idx].b = (colourA.b * w0 + colourB.b * w1 + colourC.b * w2) * shade;
                       framebuffer[idx].r =  shade * 255;
                        framebuffer[idx].g = shade * 255;
                        framebuffer[idx].b = shade * 255;
						zbuffer[idx] = depth;
               
                }
            }
        }
    }
}

// void rasterizeMesh(Mesh mesh, Cam camera, Mat4 MVP)
// {
//     std::vector<Triangle2D> tris = renderAmbiant2(mesh, camera, camera.getCamH(), camera.getCamW(), MVP);
//     for (size_t i = 0; i < tris.size(); i++)
//     {
//         rasterizeTriangle(tris[i], camera.getCamH(), camera.getCamW(), camera.getFramebuffer());
//     }
// }
// Cam.hpp - passer par référence
void rasterizeMesh(Mesh mesh, Cam& camera, Mat4 MVP)
{
    std::vector<Triangle2D> tris = renderAmbiant2(mesh,camera,  camera.getCamW(), camera.getCamH(), MVP);
    // std::cout << "tris count: " << tris.size() << std::endl;
    // if (tris.size() > 0)
    // {
    //     std::cout << "tri[0] a=(" << tris[0].a.x << "," << tris[0].a.y << ")"
    //               << " b=(" << tris[0].b.x << "," << tris[0].b.y << ")"
    //               << " c=(" << tris[0].c.x << "," << tris[0].c.y << ")" << std::endl;
    // }
    // std::cout << "camW=" << camera.getCamW() << " camH=" << camera.getCamH() << std::endl;
    // std::cout << "framebuffer size=" << camera.getFramebuffer().size() << std::endl;
    for (size_t i = 0; i < tris.size(); i++)
        rasterizeTriangle(tris[i], camera.getCamH(), camera.getCamW(), camera.getFramebuffer(), camera.getZbuffer());
}
// int main(int ac, char **av)
// {
//     // --- Cube 1x1x1 : 8 vertices locaux ---
//     std::vector<Mesh> meshes;
//     for (int i = 1; i < ac; i++)
//     {
//         std::string path = "/home/hugz/Documents/PROJET_PERSO/HomeRenderer/models/";
//         path += av[i];
//         Mesh mesh(path);
//         meshes.push_back(mesh);
//     }
//     // --- Creation camera et data ---
// 	Cam camera(120);
// 	DataGlobal data(1200, 800);

//     // --- GLFW + OpenGL ---
//     if (!glfwInit()) return -1;
//     GLFWwindow* window = glfwCreateWindow(data.getResx(), data.getResy(), "Projection", NULL, NULL);
//     if (!window) { glfwTerminate(); return -1; }
//     glfwMakeContextCurrent(window);

//     //utilisation
//     std::cout << "====== KEYS ======\n" << BOLD YELLOW "translate up : up | translate down : down | translate left : left | translat right : right\n" <<
//         "rotate left : a | rotate right : d\n" <<
//         "zoom in : s | zoom out : w" RESET << std::endl;

//     int displaymode = 1;
//     double lastTime = glfwGetTime();
//     int frameCount = 0;
//     glPointSize(5.0f);
    
//     camera.getZbuffer().resize(data.getResx() * data.getResy(), 99999.0f);
// // plus besoin de la boucle
//    //boucle de rendu et de controle
//     while (!glfwWindowShouldClose(window))
//     {
//         //calcul fps
//         double currentTime = glfwGetTime();
//         frameCount++;
//         if (currentTime - lastTime >= 1.0) {
//             std::cout << "FPS: " << frameCount << "\r" << std::flush;
//             frameCount = 0;
//             lastTime = currentTime;
//         }

//         //gestion des inputs
//         processInput(window, meshes, &displaymode, data);

//         //udate meshes and calculate :
//         updateAllMeshes(meshes);
//         std::vector<Mat4> MVPs = calculateMVPs(meshes, camera, data.getResx(), data.getResy());

//         //maj des coordonnees monde des vertex de chaque mesh
//         // for (size_t i = 0; i < meshes.size(); i++)
//         // {
//         //     meshes[i].updateMeshData();
//         // }

//         //couleur arriere plan
//         glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
//         glClear(GL_COLOR_BUFFER_BIT);
//     //     // for (size_t i = 0; i < meshes.size(); i++)
//     //     //     renderAmbiant2(meshes[i], camera, MVPs[i], data.getResx(), data.getResy());
//     //     //repartition du rendu.
//     //     if (displaymode == -1)
//     //     {
            
//     //         for (size_t i = 0; i < meshes.size(); i++)
//     //             renderWireframe(meshes[i], camera, data.getResx(), data.getResy());
//     //     }
//     //     else
//     //     {
//     //         for (size_t i = 0; i < meshes.size(); i++)
//     //             renderAmbiant(meshes[i], camera, data.getResx(), data.getResy());
//     //    }
//         renderAmbiant2(meshes[0], camera, data.getResx(), data.getResy(), MVPs[0]);
//         // renderAmbiant1(meshes[0],camera, data.getResx(), data.getResy(), MVPs[0]);
//         glfwSwapBuffers(window);
//         glfwPollEvents();
//     }

//     //destruction de la fenetre
//     glfwDestroyWindow(window);
//     glfwTerminate();
//     return 0;
// }

int main(int ac, char **av)
{
    std::vector<Mesh> meshes;
    for (int i = 1; i < ac; i++)
    {
        std::string path = "./models/";
        path += av[i];
        meshes.push_back(Mesh(path));
    }

    Cam camera(120);
    DataGlobal data(1200, 800);
    if (!glfwInit()) return -1;
    GLFWwindow* window = glfwCreateWindow(data.getResx(), data.getResy(), "Projection", NULL, NULL);
    if (!window) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);
    camera.setCamW(data.getResx());
    camera.setCamH(data.getResy());
    // Init framebuffer et zbuffer
    camera.getFramebuffer().resize(data.getResx() * data.getResy() * 3, {0, 0, 0});
    camera.getZbuffer().resize(data.getResx() * data.getResy(), 99999.0f);
    meshes[0].setPos({0.2, -0.1, 14.5});
    // Texture OpenGL pour afficher le framebuffer
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, data.getResx(), data.getResy(), 0, GL_RGB, GL_UNSIGNED_BYTE, camera.getFramebuffer().data());
    glEnable(GL_TEXTURE_2D);

    int displaymode = 1;
    double lastTime = glfwGetTime();
    int frameCount = 0;

    while (!glfwWindowShouldClose(window))
    {
        // FPS
        double currentTime = glfwGetTime();
        frameCount++;
        if (currentTime - lastTime >= 1.0) {
            // std::cout << "pos = " << meshes[0].getPos().x << "|" << meshes[0].getPos().y << "|" << meshes[0].getPos().z << "   |   FPS: " << frameCount << "\r"<< std::flush;
            frameCount = 0;
            lastTime = currentTime;
        }

        processInput(window, meshes, &displaymode, data);
        updateAllMeshes(meshes);
        std::vector<Mat4> MVPs = calculateMVPs(meshes, camera, data.getResx(), data.getResy());
        // Reset framebuffer et zbuffer
        std::fill(camera.getFramebuffer().begin(), camera.getFramebuffer().end(), Pixel{0, 0, 0}); // noir pas 25
        std::fill(camera.getZbuffer().begin(), camera.getZbuffer().end(), 99999.0f);

        for (size_t i = 0; i < meshes.size(); i++)
            rasterizeMesh(meshes[i], camera, MVPs[i]);
        // Upload et affiche
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, data.getResx(), data.getResy(), GL_RGB, GL_UNSIGNED_BYTE, camera.getFramebuffer().data());
        glClear(GL_COLOR_BUFFER_BIT);
        glBegin(GL_QUADS);
            glTexCoord2f(0, 1); glVertex2f(-1, -1);
            glTexCoord2f(1, 1); glVertex2f( 1, -1);
            glTexCoord2f(1, 0); glVertex2f( 1,  1);
            glTexCoord2f(0, 0); glVertex2f(-1,  1);
        glEnd();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}


// int main()
// {
//     int W = 1000;
//     int H = 800;

//     Triangle2D tri;
//     tri.a = {-0.5f, -0.5f};
//     tri.b = { 0.5f, -0.5f};
//     tri.c = { 0.0f,  0.5f};

//     std::vector<bool> framebuffer(W * H, false);
//     rasterizeTriangle(tri, H, W, framebuffer);

//     // Convertir bool → RGB pour OpenGL
//     std::vector<uint8_t> pixels(W * H * 3, 0);
//     for (int i = 0; i < W * H; i++)
//     {
//         if (framebuffer[i]) {
//             pixels[i * 3 + 0] = 255; // R
//             pixels[i * 3 + 1] = 255; // G
//             pixels[i * 3 + 2] = 0;   // B jaune
//         }
//     }

//     if (!glfwInit()) return -1;
//     GLFWwindow* window = glfwCreateWindow(W, H, "Rasterizer", NULL, NULL);
//     glfwMakeContextCurrent(window);

//     GLuint tex;
//     glGenTextures(1, &tex);
//     glBindTexture(GL_TEXTURE_2D, tex);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, W, H, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

//     glEnable(GL_TEXTURE_2D);

//     while (!glfwWindowShouldClose(window))
//     {
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

/*
New main 
//setup
Update meshes
↓
Calcul Model Matrix
↓
Transformation des vertices (Model → View)
↓
Backface Culling
↓
Projection (View → Clip → Screen)
↓
Rasterisation
↓
Interpolation du Z
↓
Test Z-buffer
↓
Écriture du pixel

*/