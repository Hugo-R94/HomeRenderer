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

		float dot = normal.x*toCamera.x + normal.y*toCamera.y + normal.z*toCamera.z;
		if (dot > 0) continue;
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

static Vec3 rotatedVertex(const Mat4& M, const Vec3& v)
{
    Vec4 r = mulMat4Vec4(M, {v.x, v.y, v.z, 0.0f});
    return {r.x, r.y, r.z};
}

float distanceVertex(Vec3 v1, Vec3 v2)
{
    float dx;
    float dy;
    float dz;

    dx = v2.x - v1.x;
    dy = v2.y - v1.y;
	dz = v2.z - v1.z;
    return (sqrtf(dx * dx + dy * dy + dz * dz));
}


//render avec rasterisation
std::vector<Triangle2D> renderAmbiant2(Mesh mesh, Cam camera, int W, int H, Mat4 MVP)
{
    (void)H; (void)W;

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
		// Au lieu de toCamera depuis worldPos
		Vec3 toCamera = {camera.getPos().x - a.x,
						camera.getPos().y - a.y,
						camera.getPos().z - a.z};
        // Vec3 toCamera = {camera.getPos().x-a.x, camera.getPos().y-a.y, camera.getPos().z-a.z};
        float dot = normal.x*toCamera.x + normal.y*toCamera.y + normal.z*toCamera.z;
		
        const std::vector<Vec3>& localVerts = mesh.getVerticesLocal();

        Vec2 pa = project(localVerts[ia], MVP);
        Vec2 pb = project(localVerts[ib], MVP);
        Vec2 pc = project(localVerts[ic], MVP);
        Triangle2D tri;
        tri.a.x = pa.x;
        tri.a.y = pa.y;
        tri.a.depth = distanceVertex(mesh.getWorldVertices()[ia], camera.getPos());
		tri.a.vn = mesh.getNormalsLocal()[faces[i].i[0].vn];
		// tri.a.vn = mesh.getNormalsLocal()[face.i[0].vt]
        // tri.a.depth = localVerts[ia].z;

        tri.b.x = pb.x;
        tri.b.y = pb.y;
		tri.b.depth = distanceVertex(mesh.getWorldVertices()[ib], camera.getPos());
		tri.a.vn = mesh.getNormalsLocal()[faces[i].i[1].vn];
        // tri.b.depth = localVerts[ib].z;
		tri.normal = normal;
        tri.c.x = pc.x;
        tri.c.y = pc.y;
		tri.c.depth = distanceVertex(mesh.getWorldVertices()[ic], camera.getPos());
		tri.a.vn = mesh.getNormalsLocal()[faces[i].i[2].vn];
        // tri.c.depth = localVerts[ic].z;
		tri.dot = dot;
		
        tris.push_back(tri);
    }
    return tris;
}

float edgeFunction (Vertex2D a, Vertex2D b, Vertex2D c)
{
  return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
};

Vec3 normalise(Vec3 vec){
	float magnitude = sqrt(vec.x * vec.x + vec.y * vec.y + vec.z *  vec.z);
	Vec3 normalised = {vec.x / magnitude, vec.y / magnitude, vec.z /magnitude};
	return normalised;
}

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
//            const b = colourA.b * w0 + colourB.b * w1 + colourC.b * w2;
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
				// std::cout << "vn a = " << triangle.a.vn.x << ":" << triangle.a.vn.y << ":" << triangle.c.vn.y << std::endl;
				
				Vec3 normalpx;

				normalpx.x = b0 * triangle.a.vn.x
							+ b1 * triangle.b.vn.x
							+ b2 * triangle.c.vn.x;

				normalpx.y = b0 * triangle.a.vn.y
							+ b1 * triangle.b.vn.y
							+ b2 * triangle.c.vn.y;

				normalpx.z = b0 * triangle.a.vn.z
							+ b1 * triangle.b.vn.z
							+ b2 * triangle.c.vn.z;
				
    		    // float dot = normal.x*toCamera.x + normal.y*toCamera.y + normal.z*toCamera.z;

				// std::cout << "normal face = " << triangle.normal.x << "," << triangle.normal.y <<":"<< triangle.normal.z << std::endl;
				// std::cout << "normal px   = " << normalpx.x << "," << normalpx.y << "," << normalpx.z << std::endl;
				// Normal.z varie entre -1 et 1, on le ramène à [0, 1]
				// Direction de lumière (à ajuster selon ta scène)

				// float shade = (triangle.normal.z + 1.0f) / 2.0f;
				normalpx = normalise(normalpx);
                // float shade = 1.0f - std::max(0.0f, std::min(1.0f, (depth + 5.0f) / 10.0f));
				// std::cout << shade << std::endl;
                // if (x % 3 == 0 && y %3 == 0)
                //     std::cout << "barycentric coordinate : w0 = " << w0 << " | w1 = " << w1 << " | w2 = " << w2  <<  std::endl;
				Vec3 lightDir = {0.0f, 0.0f, -1.0f};
				//lighting by face normal				
				// float shade =
				// 	normalise(triangle.normal).x * lightDir.x +
				// 	normalise(triangle.normal).y * lightDir.y +
				// 	normalise(triangle.normal).z * lightDir.z;
                // int idx = (y * W + x);
				float shade =
					normalpx.x * lightDir.x +
					normalpx.y * lightDir.y +
					normalpx.z * lightDir.z;
                // utiliser les coordonnees barycentrique pour calculer la moyenne pondere de la depth 
       			// float shade = -(depth / (0.55 * 0.55));
                int idx = (H - 1 - y) * W + x;
                if(depth < zbuffer[idx])
                {
                    // framebuffer[idx].r =  (colourA.r * b0 + colourB.r * b1 + colourC.r * b2) ;
                    // framebuffer[idx].g = (colourA.g * b0 + colourB.g * b1 + colourC.g * b2) ;
                    // framebuffer[idx].b = (colourA.b * b0 + colourB.b * b1 + colourC.b * b2) ;
					uint8_t intensity = -(uint8_t)(shade * 255.0f);
					framebuffer[idx].r = intensity;
					framebuffer[idx].g = intensity;
					framebuffer[idx].b = intensity;
						zbuffer[idx] = depth;
               
                }
            }
        }
    }
}

// Cam.hpp - passer par référence
void rasterizeMesh(Mesh mesh, Cam& camera, Mat4 MVP)
{
    std::vector<Triangle2D> tris = renderAmbiant2(mesh,camera,  camera.getCamW(), camera.getCamH(), MVP);
    for (size_t i = 0; i < tris.size(); i++)
        rasterizeTriangle(tris[i], camera.getCamH(), camera.getCamW(), camera.getFramebuffer(), camera.getZbuffer());
}

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
    camera.setCamW(data.getResx());
    camera.setCamH(data.getResy());

	if (!glfwInit()) return -1;
    GLFWwindow* window = glfwCreateWindow(data.getResx(), data.getResy(), "Projection", NULL, NULL);
    if (!window) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);
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
            std::cout << "pos = " << meshes[0].getPos().x << "|" << meshes[0].getPos().y << "|" << meshes[0].getPos().z << "   |   FPS: " << frameCount << "\r"<< std::flush;
            frameCount = 0;
            lastTime = currentTime;
        }

        processInput(window, meshes, &displaymode, data);
        updateAllMeshes(meshes);
        std::vector<Mat4> MVPs = calculateMVPs(meshes, camera, data.getResx(), data.getResy());
        // Reset framebuffer et zbuffer
        std::fill(camera.getFramebuffer().begin(), camera.getFramebuffer().end(), Pixel{0, 0, 0}); // noir pas 25
        std::fill(camera.getZbuffer().begin(), camera.getZbuffer().end(), 9999.0f);
		
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

