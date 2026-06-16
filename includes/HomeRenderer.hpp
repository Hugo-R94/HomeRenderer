#ifndef HOMERENDERER_HPP
#define HOMERENDERER_HPP


// ============================================================
// STANDARD LIBRARY FIRST
// ============================================================
#include <vector>
#include <memory>
#include <iostream>
#include <cmath>
#include <sys/time.h>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>

// ============================================================
// EXTERNAL LIBRARIES
// ============================================================
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>


struct Vec2 {float x, y;};
struct Vec3 { float x, y, z;};
struct Vec4{float x, y, z, w;};
struct Tri{Vec3 p1,p2,p3;};
struct Texture
{
    int width;
    int height;
    int channels;
    uint8_t* data;
};
struct Vertex3D {
	Vec3	pos;
	Vec3	vn;
	Vec2	uv;
};

struct Vertex2D {
    float	x, y;
    float	depth;
	Vec3	vn;
	Vec2	uv;
};

struct Triangle2D {
    Vertex2D a, b, c;
	Vec3 normal;
	float dot;
};

// struct Triangle3D {
//     Index i[3];
// 	Vec3 LocalNormal;	
// }

struct Pixel {
    uint8_t r,g,b;
};

struct Mat3 {float m[3][3];};
struct Mat4 {float m[4][4];};

// ============================================================
// INTERNAL HEADERS
// ============================================================
#include "colors.hpp"
#include "Node3D.hpp"
#include "mesh.hpp"
#include "Cam.hpp"
#include "DataGlobal.hpp"
#include <iostream>
#include <vector>
#include <cmath>
#include "../includes/mesh.hpp" // adapte selon ton projet



#include <algorithm>
#include <cstdint>
#include <iomanip>
#include <iostream>
	
#include <iostream>
#include <vector>
#include <cmath>
#include "../includes/mesh.hpp"
#include <GLFW/glfw3.h>


// ============================================================
// FORWARD DECLARATIONS
// ============================================================
class DataGlobal;
struct ViewportRender;
struct Square;
struct Vec2;

// ============================================================
// UI RENDER FUNCTIONS
// ============================================================

void render_BarMenu(GLFWwindow* window);

// ============================================================
// RENDERING FUNCTIONS
// ============================================================

void render_viewport_simple();
void resize_if_needed(ViewportRender& vp, int w, int h);
void render_viewport(ViewportRender& vp);

// ============================================================
// GEOMETRY FUNCTIONS
// ============================================================

std::unique_ptr<Square> create_square(float size, Vec2 center);
void draw_square_rotate(const Square& square, Vec2 center);

// ============================================================
// INPUT & CORE FUNCTIONS
// ============================================================

void cleanup_ImGUI(GLFWwindow* window);

// ============================================================
// ALIASES FOR BACKWARD COMPATIBILITY
// ============================================================

// Anciennes noms - gardés pour compatibilité
using carre = Square;



void setup_bar_menu(GLFWwindow* window);
float distance(Vec2 v1, Vec2 v2);
std::vector<Mat4> calculateMVPs(std::vector<Mesh>& meshes, Cam camera, int W, int H);
void rasterizeMesh(Mesh mesh, Cam& camera, Mat4 MVP);

#endif

