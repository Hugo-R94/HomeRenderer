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

struct Vec3 { float x, y, z;};
struct Vec4{float x, y, z, w;};
struct Tri{Vec3 p1,p2,p3;};
struct Vertex2D {
    float x, y;   // écran
    float depth;  // z/w pour le z-buffer
    // float u, v;   // UV pour les textures
    // float nx, ny, nz; // normale interpolée pour le shading par pixel (smooth shading)
};

struct Triangle2D {
    Vertex2D a, b, c;
	Vec3 normal;
	float dot;
};

struct Pixel {
    uint8_t r,g,b;
};

struct Mat3 {float m[3][3];};
struct Mat4 {float m[4][4];};

// ============================================================
// INTERNAL HEADERS
// ============================================================
#include "Data.hpp"
#include "colors.hpp"
#include "Node3D.hpp"
#include "mesh.hpp"
#include "Cam.hpp"
#include "DataGlobal.hpp"
#include <iostream>
#include <vector>
#include <cmath>
#include "../includes/mesh.hpp" // adapte selon ton projet


#include <iostream>
#include <vector>
#include <cmath>
#include "../includes/mesh.hpp"
#include <GLFW/glfw3.h>


// ============================================================
// FORWARD DECLARATIONS
// ============================================================
class Data;
class DataGlobal;
struct ViewportRender;
struct Square;
struct Vec2;

// ============================================================
// UI RENDER FUNCTIONS
// ============================================================

void RenderUI(GLFWwindow* window, Data& data);
void render_BarMenu(GLFWwindow* window);
void render_outliner(Data& data);
void render_proprieties(Data& data);
void display_double_viewport(Data& data);
void display_viewport(Data& data);

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

void handle_input(Data* data);
GLFWwindow* init_ImGUI(Data data);
void cleanup_ImGUI(GLFWwindow* window);
void update_data(Data& data);

// ============================================================
// ALIASES FOR BACKWARD COMPATIBILITY
// ============================================================

// Anciennes noms - gardés pour compatibilité
using carre = Square;



void setup_bar_menu(GLFWwindow* window);
void draw_outliner(Data& data);
void draw_proprieties(Data& data);

#endif

