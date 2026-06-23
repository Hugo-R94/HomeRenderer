#include "HomeRenderer.hpp"
#include <thread>


void backFaceCulling(Mesh& mesh, const Cam& camera, float nearPlane = 0.1f)
{
    const std::vector<Face>& faces = mesh.getFaces();
    const std::vector<Vec3>& v     = mesh.getWorldVertices();
    const std::vector<Vec3>& local = mesh.getVerticesLocal();

    std::vector<float> dotlist;
    std::vector<Face>  visible;
    dotlist.reserve(faces.size());
    visible.reserve(faces.size());

    Mat4 R = multiplyMat4(rotationZ4(mesh.getRot().z),
             multiplyMat4(rotationY4(mesh.getRot().y),
                          rotationX4(mesh.getRot().x)));

    for (size_t i = 0; i < faces.size(); i++)
    {
        const Face& f = faces[i];

        int ia = f.i[0].v;
        int ib = f.i[1].v;
        int ic = f.i[2].v;

        float neaPlane = 0.00001f;
        if (v[ia].z < nearPlane ||
            v[ib].z < nearPlane ||
            v[ic].z < nearPlane)
            continue;

        Vec3 a = v[ia];
        Vec4 n4 = mulMat4Vec4(R, {f.LocalNormal.x, f.LocalNormal.y, f.LocalNormal.z, 0.0f});
        Vec3 normal = {n4.x, n4.y, n4.z};

        Vec3 toCamera = {
            camera.getPos().x - a.x,
            camera.getPos().y - a.y,
            camera.getPos().z - a.z
        };

        float dot = normal.x*toCamera.x + normal.y*toCamera.y + normal.z*toCamera.z;

        if (dot < -0.00001f) continue;

        dotlist.push_back(dot);
        visible.push_back(f);
    }

    mesh.setDotFace(dotlist);
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

// avec rasterisation
std::vector<Triangle2D> renderAmbiant2(const Mesh& mesh, const Cam& camera, int W, int H, const Mat4& MVP, DataGlobal& data)
{
    (void)H; (void)W;
    const std::vector<Face>& faces        = mesh.getCulledFaces();
    const std::vector<Vec3>& worldVerts   = mesh.getWorldVertices();
    const std::vector<Vec3>& localVerts   = mesh.getVerticesLocal();
    const std::vector<Vec3>& normalsLocal = mesh.getNormalsLocal();
    const std::vector<Vec2>& uvs          = mesh.getUV();
    Vec3 camPos                           = camera.getPos();

    std::vector<Triangle2D> tris(faces.size());

    int numThreads = std::thread::hardware_concurrency();
    if (numThreads <= 0) numThreads = 4;
    int chunkSize = (faces.size() + numThreads - 1) / numThreads;

    for (int t = 0; t < numThreads; t++)
    {
        int iStart = t * chunkSize;
        int iEnd   = std::min(iStart + chunkSize, (int)faces.size());
        if (iStart >= iEnd) break;

        data.pool.enqueue([=, &faces, &worldVerts, &localVerts, &normalsLocal, &uvs, &tris]()
        {
            for (int i = iStart; i < iEnd; i++)
            {
                int ia = faces[i].i[0].v;
                int ib = faces[i].i[1].v;
                int ic = faces[i].i[2].v;

                Vec3 normal = rotatedVertex(MVP, faces[i].LocalNormal);
                Vec3 toCamera = {
                    camPos.x - worldVerts[ia].x,
                    camPos.y - worldVerts[ia].y,
                    camPos.z - worldVerts[ia].z
                };
                float dot = normal.x*toCamera.x + normal.y*toCamera.y + normal.z*toCamera.z;

                Vec2 pa = project(localVerts[ia], MVP);
                Vec2 pb = project(localVerts[ib], MVP);
                Vec2 pc = project(localVerts[ic], MVP);

                Triangle2D& tri = tris[i];
                tri.a.x     = pa.x;
                tri.a.y     = pa.y;
                tri.a.depth = distanceVertex(worldVerts[ia], camPos);
                tri.a.vn    = rotatedVertex(MVP, normalsLocal[faces[i].i[0].vn]);
                tri.a.uv    = uvs[faces[i].i[0].vt];
                tri.b.x     = pb.x;
                tri.b.y     = pb.y;
                tri.b.depth = distanceVertex(worldVerts[ib], camPos);
                tri.b.vn    = rotatedVertex(MVP, normalsLocal[faces[i].i[1].vn]);
                tri.b.uv    = uvs[faces[i].i[1].vt];
                tri.c.x     = pc.x;
                tri.c.y     = pc.y;
                tri.c.depth = distanceVertex(worldVerts[ic], camPos);
                tri.c.vn    = rotatedVertex(MVP, normalsLocal[faces[i].i[2].vn]);
                tri.c.uv    = uvs[faces[i].i[2].vt];
                tri.normal  = normal;
                tri.dot     = dot;
            }
        });
    }

    data.pool.wait();
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

Pixel sampleTexture(const Texture& tex, Vec2 uv)
{
		uv.x = std::max(0.0f, std::min(1.0f, uv.x));
		uv.y = std::max(0.0f, std::min(1.0f, uv.y));
    int x = (int)(uv.x * (tex.width  - 1));
    // int y = (int)(uv.y * (tex.height - 1));
    int y = (int)((1.0f - uv.y) * (tex.height - 1));
    int idx = (y * tex.width + x) * 4;

    Pixel p;
    p.r = tex.data[idx + 0];
    p.g = tex.data[idx + 1];
    p.b = tex.data[idx + 2];

    return p;
}
Vec3 calculateInterpolatedNormal(const Triangle2D &triangle, float b0, float b1, float b2)
{
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
	return normalise(normalpx);
}

Vec2 calculateInterpolatedUV(const Triangle2D &triangle, float b0, float b1, float b2)
{
	Vec2 UVpx;
	UVpx.x = b0 * triangle.a.uv.x + b1 * triangle.b.uv.x + b2 * triangle.c.uv.x;
	UVpx.y = b0 * triangle.a.uv.y + b1 * triangle.b.uv.y + b2 * triangle.c.uv.y;
	return UVpx;
}

Vec3 applyNormalMap(const Vec3& normal, const Vec2& uv,const Texture& txt)
{
	Pixel p = sampleTexture(txt, uv);
	Vec3 normalMap;
	normalMap.x = p.b;
	normalMap.y = p.g;
	normalMap.z = p.r;
    // normalMap.x = p.r;
    // normalMap.y = p.g;
    // normalMap.z = p.b;
	normalMap = normalise(normalMap);
	normalMap.x = (normalMap.x + normal.x) / 2;
	normalMap.y = (normalMap.y + normal.y) / 2;
	normalMap.z = (normalMap.z + normal.z) / 2;
	return normalMap;
}

//fonction pour rasteriser un seul triangle calcul des coordonnee baricentrique w
// void rasterizeTriangle(Triangle2D triangle, int H, int W, std::vector<Pixel>& framebuffer, std::vector<float> &zbuffer, Texture txt, Texture normaltxt)
// {
//     int minX = std::max(0,   (int)((std::min({triangle.a.x, triangle.b.x, triangle.c.x}) * 0.5f + 0.5f) * W));
//     int maxX = std::min(W-1, (int)((std::max({triangle.a.x, triangle.b.x, triangle.c.x}) * 0.5f + 0.5f) * W));
//     int minY = std::max(0,   (int)((std::min({triangle.a.y, triangle.b.y, triangle.c.y}) * 0.5f + 0.5f) * H));
//     int maxY = std::min(H-1, (int)((std::max({triangle.a.y, triangle.b.y, triangle.c.y}) * 0.5f + 0.5f) * H));

//     float area = edgeFunction(triangle.a, triangle.b, triangle.c);
//     if (area == 0) return;

//     for (int y = minY; y <= maxY; y++)
//     {
//         for (int x = minX; x <= maxX; x++)
//         {
//             Pixel colourA = {255, 0, 0}; // Red
//             Pixel colourB = {0, 255, 0}; // Red
//             Pixel colourC = {0, 0, 255}; // Red

//             Vertex2D p;
//             p.x = (x + 0.5f) / W * 2.0f - 1.0f;
//             p.y = (y + 0.5f) / H * 2.0f - 1.0f;

//             float w0 = edgeFunction(triangle.a, triangle.b, p);
//             float w1 = edgeFunction(triangle.b, triangle.c, p);
//             float w2 = edgeFunction(triangle.c, triangle.a, p);

//             if (w0 >= 0 && w1 >= 0 && w2 >= 0)
//             {
//                 // Coordonnées barycentriques
//                 float b0 = w1 / area;
//                 float b1 = w2 / area;
//                 float b2 = w0 / area;

//                 // Interpolation de la profondeur
//                 float depth = b0 * triangle.a.depth
//                             + b1 * triangle.b.depth
//                             + b2 * triangle.c.depth;
				
// 				Vec3 normalpx = calculateInterpolatedNormal(triangle, b0, b1, b2);
// 				Vec2 UVpx = calculateInterpolatedUV(triangle, b0, b1, b2);
// 				normalpx = applyNormalMap(normalpx, UVpx, normaltxt);

				
// 				Pixel colour = sampleTexture(txt, UVpx);
				
// 				Vec3 lightDir = {0.0f, 0.0f, 1.0f};
// 				lightDir = normalise(lightDir);
// 				float shade = normalpx.x * lightDir.x + normalpx.y * lightDir.y + normalpx.z * lightDir.z;

// 				int idx = (H - 1 - y) * W + x;
// 				// shade = std::max(0.0f, shade);
				
//                 if(depth < zbuffer[idx])
//                 {
					
// 					framebuffer[idx].r = (uint8_t)(colour.r * shade);
// 					framebuffer[idx].g = (uint8_t)(colour.g * shade);
// 					framebuffer[idx].b = (uint8_t)(colour.b * shade);
// 					zbuffer[idx] = depth;
               
//                 }
//             }
//         }
//     }
// }


// // Cam.hpp - passer par référence - probleme je rasterize quun mesh au lieu de tout les meshs
// void rasterizeMesh(Mesh mesh, Cam& camera, Mat4 MVP)
// {
//     std::vector<Triangle2D> tris = renderAmbiant2(mesh,camera,  camera.getCamW(), camera.getCamH(), MVP);
//     for (size_t i = 0; i < tris.size(); i++)
//         rasterizeTriangle(tris[i], camera.getCamH(), camera.getCamW(), camera.getFramebuffer(), camera.getZbuffer(), mesh.getTexture(), mesh.getNormalTxt());
// }
void rasterizeTriangle(
    const Triangle2D& triangle, int H, int W,
    int xStart, int xEnd, Cam &camera,         // <-- bornes de la tuile
    const Texture& txt, const Texture& normaltxt)
{
    int minX = std::max(xStart, (int)((std::min({triangle.a.x, triangle.b.x, triangle.c.x}) * 0.5f + 0.5f) * W));
    int maxX = std::min(xEnd-1, (int)((std::max({triangle.a.x, triangle.b.x, triangle.c.x}) * 0.5f + 0.5f) * W));
    int minY = std::max(0,      (int)((std::min({triangle.a.y, triangle.b.y, triangle.c.y}) * 0.5f + 0.5f) * H));
    int maxY = std::min(H-1,    (int)((std::max({triangle.a.y, triangle.b.y, triangle.c.y}) * 0.5f + 0.5f) * H));

    if (minX > maxX || minY > maxY) return; // triangle hors tuile

    float area = edgeFunction(triangle.a, triangle.b, triangle.c);
    if (area == 0.0f) return;

    Vec3 lightDir = normalise({0.0f, 0.0f, 1.0f}); // sorti de la double boucle

    for (int y = minY; y <= maxY; y++)
    {
        for (int x = minX; x <= maxX; x++)
        {
            Vertex2D p;
            p.x = (x + 0.5f) / W * 2.0f - 1.0f;
            p.y = (y + 0.5f) / H * 2.0f - 1.0f;
            
            float w0 = edgeFunction(triangle.a, triangle.b, p);
            float w1 = edgeFunction(triangle.b, triangle.c, p);
            float w2 = edgeFunction(triangle.c, triangle.a, p);

            if (w0 >= 0 && w1 >= 0 && w2 >= 0)
            {
                float b0 = w1 / area;
                float b1 = w2 / area;
                float b2 = w0 / area;

                float depth = b0 * triangle.a.depth
                            + b1 * triangle.b.depth
                            + b2 * triangle.c.depth;
                
                int idx = (H - 1 - y) * W + x;
                if (depth <  camera.getZbuffer()[idx])
                {
                    Vec3 normalpx = calculateInterpolatedNormal(triangle, b0, b1, b2);
                    Vec2 UVpx     = calculateInterpolatedUV(triangle, b0, b1, b2);
                    normalpx      = applyNormalMap(normalpx, UVpx, normaltxt);
                    Pixel colour  = sampleTexture(txt, UVpx);

                    float shade = normalpx.x * lightDir.x
                                + normalpx.y * lightDir.y
                                + normalpx.z * lightDir.z;
                    // float shade = 0.5f;
                    // camera.getFramebuffer()[idx].r = shade * 255;
                    // camera.getFramebuffer()[idx].g = shade * 255;
                    // camera.getFramebuffer()[idx].b = shade * 255;
                    camera.getFramebuffer()[idx].r = (uint8_t)(colour.r * shade);
                    camera.getFramebuffer()[idx].g = (uint8_t)(colour.g * shade);
                    camera.getFramebuffer()[idx].b = (uint8_t)(colour.b * shade);
                    camera.getZbuffer()[idx]       = depth;
                }
            }
        }
    }
}

// void rasterizeMeshes(const std::vector<Mesh>& meshes, Cam& camera, const std::vector<Mat4>& MVPs)
// {
//     int H = camera.getCamH();
//     int W = camera.getCamW();

//     std::vector<std::pair<std::vector<Triangle2D>, std::pair<Texture, Texture>>> meshData;
//     meshData.reserve(meshes.size());
//     for (size_t i = 0; i < meshes.size(); i++)
//     {
//         std::vector<Triangle2D> tris = renderAmbiant2(meshes[i], camera, W, H, MVPs[i]);
//         meshData.push_back({ std::move(tris), { meshes[i].getTexture(), meshes[i].getNormalTxt() } });
//     }
//     int numThreads = std::thread::hardware_concurrency();
//     if (numThreads <= 0) numThreads = 4;

//     int tileW = W / numThreads;

//     std::vector<std::thread> threads;
//     threads.reserve(numThreads);

//     for (int t = 0; t < numThreads; t++)
//     {
//         int xStart = t * tileW;
//         int xEnd   = (t == numThreads - 1) ? W : xStart + tileW;

//         threads.emplace_back([=, &meshData, &camera]()
//         {
//             auto& fb = camera.getFramebuffer();
//             auto& zb = camera.getZbuffer();

//             for (auto& [tris, textures] : meshData)
//                 for (const Triangle2D& tri : tris)
//                     rasterizeTriangle(tri, H, W, xStart, xEnd, fb, zb, textures.first, textures.second);
//         });
//     }
//     for (auto& th : threads)
//         th.join();

// }
void rasterizeMeshes(const std::vector<Mesh>& meshes, Cam& camera, const std::vector<Mat4>& MVPs, DataGlobal& data)
{
    int H = camera.getCamH();
    int W = camera.getCamW();
    auto t0 = std::chrono::high_resolution_clock::now();
    std::vector<std::pair<std::vector<Triangle2D>, std::pair<Texture, Texture>>> meshData;
    meshData.reserve(meshes.size());
    for (size_t i = 0; i < meshes.size(); i++)
    {
        std::vector<Triangle2D> tris = renderAmbiant2(meshes[i], camera, W, H, MVPs[i], data);
        meshData.push_back({ std::move(tris), { meshes[i].getTexture(), meshes[i].getNormalTxt() } });
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    int numThreads = std::thread::hardware_concurrency();
    if (numThreads <= 0) numThreads = 4;
    int tileW = W / numThreads;

    for (int t = 0; t < numThreads; t++)
    {
        int xStart = t * tileW;
        int xEnd   = (t == numThreads - 1) ? W : xStart + tileW;

        data.pool.enqueue([=, &meshData, &camera]()
        {
            auto& fb = camera.getFramebuffer();
            auto& zb = camera.getZbuffer();
            for (auto& [tris, textures] : meshData)
                for (const Triangle2D& tri : tris)
                    rasterizeTriangle(tri, H, W, xStart, xEnd, camera, textures.first, textures.second);
        });
    }    

    //compte le nombre de pixel calculer au total actuellement 1/3 du nombre de pxl.
    // size_t totalPixels = 0;
    // for (auto& [tris, textures] : meshData)
    //     for (auto& tri : tris) {
    //         int minX = std::max(0,   (int)((std::min({tri.a.x, tri.b.x, tri.c.x}) * 0.5f + 0.5f) * W));
    //         int maxX = std::min(W-1, (int)((std::max({tri.a.x, tri.b.x, tri.c.x}) * 0.5f + 0.5f) * W));
    //         int minY = std::max(0,   (int)((std::min({tri.a.y, tri.b.y, tri.c.y}) * 0.5f + 0.5f) * H));
    //         int maxY = std::min(H-1, (int)((std::max({tri.a.y, tri.b.y, tri.c.y}) * 0.5f + 0.5f) * H));
    //         totalPixels += (maxX - minX) * (maxY - minY);
    //     }
    // std::cout << "pixels bbox total : " << totalPixels << "\n";

    data.pool.wait(); // attend que toutes les tuiles soient finies
    auto t2 = std::chrono::high_resolution_clock::now();
    // std::cout 
    //         << "projection : " << std::chrono::duration_cast<std::chrono::milliseconds>(t1-t0).count() << "ms | "
    //         << "rasterize  : " << std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count() << "ms\n";
    // size_t totalTris = 0;
    // for (auto& [tris, textures] : meshData)
    //     totalTris += tris.size();
    // std::cout << "triangles : " << totalTris << "\n";
    
}
// int main()
// {
// 	rasterizeByTile();
// }