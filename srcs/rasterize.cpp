#include "HomeRenderer.hpp"


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
		if (dot > 0.00001f) continue;
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

// avec rasterisation
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
		tri.a.vn = rotatedVertex(MVP, mesh.getNormalsLocal()[faces[i].i[0].vn]);
		tri.a.uv = mesh.getUV()[faces[i].i[0].vt];
		// tri.a.vn = mesh.getNormalsLocal()[face.i[0].vt]
        // tri.a.depth = localVerts[ia].z;
		// std::cout
		// 	<< "vn indices : "
		// 	<< faces[i].i[0].vn << " "
		// 	<< faces[i].i[1].vn << " "
		// 	<< faces[i].i[2].vn << "\n";
        tri.b.x = pb.x;
        tri.b.y = pb.y;
		tri.b.depth = distanceVertex(mesh.getWorldVertices()[ib], camera.getPos());
		tri.b.vn = rotatedVertex(MVP, mesh.getNormalsLocal()[faces[i].i[1].vn]);
		tri.b.uv = mesh.getUV()[faces[i].i[1].vt];
        // tri.b.depth = localVerts[ib].z;
		tri.normal = normal;
        tri.c.x = pc.x;
        tri.c.y = pc.y;
		tri.c.depth = distanceVertex(mesh.getWorldVertices()[ic], camera.getPos());
		tri.c.vn = rotatedVertex(MVP, mesh.getNormalsLocal()[faces[i].i[2].vn]);
		tri.c.uv = mesh.getUV()[faces[i].i[2].vt];
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

Pixel sampleTexture(const Texture& tex, Vec2 uv)
{
		uv.x = std::max(0.0f, std::min(1.0f, uv.x));
		uv.y = std::max(0.0f, std::min(1.0f, uv.y));
    int x = (int)(uv.x * (tex.width  - 1));
    int y = (int)(uv.y * (tex.height - 1));

    int idx = (y * tex.width + x) * 4;

    Pixel p;
    p.r = tex.data[idx + 0];
    p.g = tex.data[idx + 1];
    p.b = tex.data[idx + 2];

    return p;
}
Vec3 calculateInterpolatedNormal(Triangle2D &triangle, float b0, float b1, float b2)
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

Vec2 calculateInterpolatedUV(Triangle2D &triangle, float b0, float b1, float b2)
{
	Vec2 UVpx;
	UVpx.x = b0 * triangle.a.uv.x + b1 * triangle.b.uv.x + b2 * triangle.c.uv.x;
	UVpx.y = b0 * triangle.a.uv.y + b1 * triangle.b.uv.y + b2 * triangle.c.uv.y;
	return UVpx;
}

Vec3 applyNormalMap(Vec3 normal, Vec2 uv, Texture txt)
{
	Pixel p = sampleTexture(txt, uv);
	Vec3 normalMap;
	normalMap.x = p.b;
	normalMap.y = p.g;
	normalMap.z = p.r;
	normalMap = normalise(normalMap);
	normalMap.x = (normalMap.x + normal.x) / 2;
	normalMap.y = (normalMap.y + normal.y) / 2;
	normalMap.z = (normalMap.z + normal.z) / 2;
	return normalMap;
}

//fonction pour rasteriser un seul triangle calcul des coordonnee baricentrique w
void rasterizeTriangle(Triangle2D triangle, int H, int W, std::vector<Pixel>& framebuffer, std::vector<float> &zbuffer, Texture txt, Texture normaltxt)
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
            Pixel colourA = {255, 0, 0}; // Red
            Pixel colourB = {0, 255, 0}; // Red
            Pixel colourC = {0, 0, 255}; // Red

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
				
				Vec3 normalpx = calculateInterpolatedNormal(triangle, b0, b1, b2);
				Vec2 UVpx = calculateInterpolatedUV(triangle, b0, b1, b2);
				normalpx = applyNormalMap(normalpx, UVpx, normaltxt);

				
				Pixel colour = sampleTexture(txt, UVpx);
				
				Vec3 lightDir = {0.0f, 0.0f, 1.0f};
				lightDir = normalise(lightDir);
				float shade = normalpx.x * lightDir.x + normalpx.y * lightDir.y + normalpx.z * lightDir.z;

				int idx = (H - 1 - y) * W + x;
				// shade = std::max(0.0f, shade);
				
                if(depth < zbuffer[idx])
                {
					
					framebuffer[idx].r = (uint8_t)(colour.r * shade);
					framebuffer[idx].g = (uint8_t)(colour.g * shade);
					framebuffer[idx].b = (uint8_t)(colour.b * shade);
					zbuffer[idx] = depth;
               
                }
            }
        }
    }
}

#include <thread>
// void rasterizeByTile(Mesh mesh, int H, int W, std::vector<Pixel>& framebuffer, std::vector<float> &zbuffer)
void rasterizeByTile()
{
	int numThreads = std::thread::hardware_concurrency();
	std::cout << numThreads << std::endl;
	int H = 1080; int W = 1920;
	int chunksize = (W * H) / numThreads;
	int chunksizex = W / numThreads;
	int chunksizey = H;
	int side = (W*H) / numThreads / 2;
	int nbrows = (int)(sqrt(numThreads));
	int nbcolums = (int)(sqrt(numThreads) + 1);
	int sidex = W / nbcolums;
	int sidey = H / nbrows;
	int bonuspx = W % nbrows;
	int bonuspy = H % nbrows;
	std::cout << nbrows << ":" << nbcolums << " | " << sidex << ":" << sidey  << " | " << bonuspx << ":" << bonuspy << std::endl;
	
	std::cout << chunksize << " : " << chunksizex << "|" << chunksizey << " - " << side << std::endl;
	for (int i = 0; i < numThreads; i++)
	{
		//iteration a travers toutes les chunks
		for (int x = chunksizex * i / H; x < chunksizex * i + 1; x++)
		{
			for (int y = chunksizey * i / W; y < chunksizey * i + 1; y++)
			{
				
			}
		}
	}
}

// Cam.hpp - passer par référence - probleme je rasterize quun mesh au lieu de tout les meshs
void rasterizeMesh(Mesh mesh, Cam& camera, Mat4 MVP)
{
    std::vector<Triangle2D> tris = renderAmbiant2(mesh,camera,  camera.getCamW(), camera.getCamH(), MVP);
    for (size_t i = 0; i < tris.size(); i++)
        rasterizeTriangle(tris[i], camera.getCamH(), camera.getCamW(), camera.getFramebuffer(), camera.getZbuffer(), mesh.getTexture(), mesh.getNormalTxt());
}

int main()
{
	rasterizeByTile();
}