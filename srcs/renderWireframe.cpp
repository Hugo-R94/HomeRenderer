#include "../includes/HomeRenderer.hpp"


void renderWireframe(Mesh mesh, Cam camera, int W, int H)
{
    Mat4 M = modelMatrix(mesh.getPos(), mesh.getRot(), mesh.getMeshScale());
    Mat4 V = viewMatrix(camera.getPos());
    Mat4 P = projectionMatrix(camera.getFov(), (float)W/H, 0.1f, 100.0f);
    Mat4 MVP = multiplyMat4(P, multiplyMat4(V, M)); 

    //affichage des vertex en rouge
    glPointSize(5.0f);
    glBegin(GL_POINTS);
    glColor3f(1.0f, 0.0f, 0.0f);
    const std::vector<Vec3>& localVerts = mesh.getVerticesLocal();

    for (size_t i = 0; i < localVerts.size(); i++)
    {
        // Vec2 ndc = toNDC(projected[i], W, H);
        Vec2 ndc = project(localVerts[i], MVP);
        glVertex2f(ndc.x, ndc.y);
    }
    glEnd();

    glFrontFace(GL_CW);  
    glPointSize(15.0f);

    //Affichage du centre
    glBegin(GL_POINTS);
    glColor3f(0.0f, 1.0f, 0.0f);
    Vec2 center = project(mesh.getCenter(),MVP);
    glVertex2f(center.x,center.y);
    glEnd();

    //affichage des edges en jaune
    glBegin(GL_LINES);
    glColor3f(1.0f, 1.0f, 0.0f);
    const std::vector<Face>& faces = sortFacesByDepth(mesh.getFaces(),camera, mesh.getWorldVertices());
    for (size_t i = 0; i < faces.size(); i++)
    {
        for (int e = 0; e < 3; e++)
        {
            int a = faces[i].i[e].v;         // index vertex a
            int b = faces[i].i[(e + 1) % 3].v; // index vertex b
            

            Vec2 pa = project(localVerts[a], MVP);
            Vec2 pb = project(localVerts[b], MVP);

            glVertex2f(pa.x, pa.y);
            glVertex2f(pb.x, pb.y);
        }
    }
    glEnd();
}
