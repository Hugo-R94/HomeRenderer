#include "../includes/HomeRenderer.hpp"


void renderAmbiant(Mesh mesh, Cam camera , int W, int H)
{
    // mesh.updateMeshData();
    // std::vector<vec2> projected =
    Mat4 M = modelMatrix(mesh.getPos(), mesh.getRot(), mesh.getMeshScale());
    Mat4 V = viewMatrix(camera.getPos());
    Mat4 P = projectionMatrix(camera.getFov(), (float)W/H, 0.1f, 100.0f);
    Mat4 MVP = multiplyMat4(P, multiplyMat4(V, M)); 
    // std::vector<Vec2> projected = ProjectVertices(mesh.getWorldVertices(), camera);
    const std::vector<Face>& faces = sortFacesByDepth(mesh.getFaces(),camera, mesh.getWorldVertices());
    glBegin(GL_TRIANGLES);
    for (size_t i = 0; i < faces.size(); i++)
    {
        int ia = faces[i].i[0].v;
        int ib = faces[i].i[1].v;
        int ic = faces[i].i[2].v;

        Vec3 a = (mesh.getWorldVertices())[ia];

        // utilise la normale locale de la face
        Vec3 normal = faces[i].LocalNormal;
        // applique la rotation du mesh a la normale
        Mat3 R = buildRotationMatrix(mesh.getRot());
        normal = rotatedVertex(R, normal);

        Vec3 toCamera = {camera.getPos().x-a.x, camera.getPos().y-a.y, camera.getPos().z-a.z};
        float dot = normal.x*toCamera.x + normal.y*toCamera.y + normal.z*toCamera.z;
        // if (normal.z >= 0) continue;
        if (dot <= 0) continue;
        
        float len_n = sqrt(normal.x*normal.x + normal.y*normal.y + normal.z*normal.z);
        float len_c = sqrt(toCamera.x*toCamera.x + toCamera.y*toCamera.y + toCamera.z*toCamera.z);
        float shade = dot / (len_n * len_c) / 1.2;
        glColor3f(shade, shade, shade); 
        const std::vector<Vec3>& localVerts = mesh.getVerticesLocal();
        Vec2 pa = project(localVerts[ia], MVP);
        Vec2 pb = project(localVerts[ib], MVP);
        Vec2 pc = project(localVerts[ic], MVP);

        glVertex2f(pa.x, pa.y);
        glVertex2f(pb.x, pb.y);
        glVertex2f(pc.x, pc.y);
    }
    glEnd();
}
static Vec3 rotatedVertex(const Mat4& M, const Vec3& v)
{
    Vec4 r = mulMat4Vec4(M, {v.x, v.y, v.z, 0.0f});
    return {r.x, r.y, r.z};
}

float distanceBVec3(Vec3 p1, Vec3 p2)
{
    return (sqrt(pow(p1.x -p2.x , 2) + pow(p1.y -p2.y , 2) + pow(p1.z -p2.z , 2)));
}

void renderAmbiant1(Mesh mesh, Cam &camera , int W, int H, Mat4 MVP)
{
    const std::vector<Face>& faces = sortFacesByDepth(mesh.getCulledFaces(),camera, mesh.getWorldVertices());
    // const std::vector<Face>& faces = mesh.getCulledFaces();
    glBegin(GL_TRIANGLES);
    for (size_t i = 0; i < faces.size(); i++)
    {
        int ia = faces[i].i[0].v;
        int ib = faces[i].i[1].v;
        int ic = faces[i].i[2].v;

        Vec3 a = (mesh.getWorldVertices())[ia];

        // utilise la normale locale de la face
        Vec3 normal = faces[i].LocalNormal;
        // applique la rotation du mesh a la normale
        normal = rotatedVertex(MVP, normal);

        Vec3 toCamera = {camera.getPos().x-a.x, camera.getPos().y-a.y, camera.getPos().z-a.z};
        float dot = normal.x*toCamera.x + normal.y*toCamera.y + normal.z*toCamera.z;

        float len_n = sqrt(normal.x*normal.x + normal.y*normal.y + normal.z*normal.z);
        float len_c = sqrt(toCamera.x*toCamera.x + toCamera.y*toCamera.y + toCamera.z*toCamera.z);
        float shade = dot / (len_n * len_c) + 0.1;

        glColor3f(shade, shade, shade);

        const std::vector<Vec3>& localVerts = mesh.getVerticesLocal();
        Vec2 pa = project(localVerts[ia], MVP);
        Vec2 pb = project(localVerts[ib], MVP);
        Vec2 pc = project(localVerts[ic], MVP);

        //inserer ici la rasterisation a la place de glVertex2f 
        //rasterisation = creer un vector  de triangle : tri(vertex2d sommet1, sommet2, sommet3) chaque sommet est vertex2d comme defini apres
        //de la creer une bouncing box pour chaque triangle (chercher le rectangle de pixel qui englobe le triangle) 
        //calculer si chaque pixel est dans le triangle ou non. -> calculer la depth du point en utilisant les coordonnes barycentrique << a creuser
    
        glVertex2f(pa.x, pa.y);
        glVertex2f(pb.x, pb.y);
        glVertex2f(pc.x, pc.y);
    }
    glEnd();
}

std::vector<Triangle2D> renderAmbiantTri(Mesh mesh, Cam &camera , int W, int H, Mat4 MVP)
{
    const std::vector<Face>& faces = sortFacesByDepth(mesh.getCulledFaces(),camera, mesh.getWorldVertices());
    // const std::vector<Face>& faces = mesh.getCulledFaces();
    std::vector<Triangle2D> projectedTriangles;
    for (size_t i = 0; i < faces.size(); i++)
    {
        int ia = faces[i].i[0].v;
        int ib = faces[i].i[1].v;
        int ic = faces[i].i[2].v;

        Vec3 a = (mesh.getWorldVertices())[ia];

        // utilise la normale locale de la face
        Vec3 normal = faces[i].LocalNormal;
        // applique la rotation du mesh a la normale
        normal = rotatedVertex(MVP, normal);

        Vec3 toCamera = {camera.getPos().x-a.x, camera.getPos().y-a.y, camera.getPos().z-a.z};
        float dot = normal.x*toCamera.x + normal.y*toCamera.y + normal.z*toCamera.z;

        float len_n = sqrt(normal.x*normal.x + normal.y*normal.y + normal.z*normal.z);
        float len_c = sqrt(toCamera.x*toCamera.x + toCamera.y*toCamera.y + toCamera.z*toCamera.z);
        float shade = dot / (len_n * len_c) + 0.1;

        glColor3f(shade, shade, shade);

        const std::vector<Vec3>& localVerts = mesh.getVerticesLocal();
        Vec2 pa = project(localVerts[ia], MVP);
        Vec2 pb = project(localVerts[ib], MVP);
        Vec2 pc = project(localVerts[ic], MVP);

        Triangle2D projectedTri;
        projectedTri.a = {pa.x,pa.y,dot};
        projectedTri.b = {pb.x,pb.y,dot};
        projectedTri.c = {pc.x,pc.y,dot};
        projectedTriangles.push_back(projectedTri);
        //inserer ici la rasterisation a la place de glVertex2f 
        //rasterisation = creer un vector  de triangle : tri(vertex2d sommet1, sommet2, sommet3) chaque sommet est vertex2d comme defini apres
        //de la creer une bouncing box pour chaque triangle (chercher le rectangle de pixel qui englobe le triangle) 
        //calculer si chaque pixel est dans le triangle ou non. -> calculer la depth du point en utilisant les coordonnes barycentrique << a creuser
    
    }
    return projectedTriangles;
}

