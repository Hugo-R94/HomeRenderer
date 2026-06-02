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

