#include "Data.hpp"

// ============================================================
// Data Constructor & Destructor
// ============================================================

Data::Data():_meshID(0)
{
    // Auto-détection de la résolution écran primaire
    const GLFWvidmode* screen = glfwGetVideoMode(glfwGetPrimaryMonitor());
    if (screen)
    {
        _res_x = screen->width;
        _res_y = screen->height;
    }
    is_outliner_visible = true;
}

Data::~Data(){}

// ============================================================
// Accesseurs (Getters)
// ============================================================

// ============================================================
// Mutateurs (Setters) 
// ============================================================

void Data::SetResx(int resx)
{
    if (resx > 0)
        _res_x = resx;
}

void Data::SetResy(int resy)
{
    if (resy > 0)
        _res_y = resy;  // ✅ CORRECTION du bug : était _res_x = resy;
}

void Data::setMeshID(int meshID){_meshID = meshID;}

int Data::getMeshID() const{return _meshID;}

