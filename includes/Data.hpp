#ifndef DATA_HPP
#define DATA_HPP

#include <GL/gl.h>   // AVANT GLFW !
#include <GLFW/glfw3.h>
#include <memory>
#include <vector>

// ============================================================
// MATHEMATICAL STRUCTURES
// ============================================================

/**
 * @struct Vec2
 * @brief Représente un vecteur 2D avec coordonnées flottantes
 */
struct Vec2
{
    float x = 0.0f;
    float y = 0.0f;
    
    Vec2() = default;
    Vec2(float x_, float y_) : x(x_), y(y_) {}
};

/**
 * @struct Square
 * @brief Représente un carré 2D avec 4 sommets
 */
struct Square
{
    Vec2 vertex1;
    Vec2 vertex2;
    Vec2 vertex3;
    Vec2 vertex4;
};

// ============================================================
// RENDERING STRUCTURES
// ============================================================

/**
 * @struct ViewportRender
 * @brief Framebuffer Object pour rendu hors écran
 */
struct ViewportRender
{
    GLuint fbo = 0;
    GLuint texture = 0;
    int width = 0;
    int height = 0;
};

// ============================================================
// APPLICATION DATA
// ============================================================

/**
 * @class Data
 * @brief Conteneur global pour l'état de l'application
 * 
 * Gère les données communes: résolution écran, état UI, etc.
 */
class Data
{
private:
    int _res_x = 1920;
    int _res_y = 1080;
    int _meshID;
    
public:
    bool is_outliner_visible = true;
    
    // Constructeurs
    Data();
    ~Data();
    
    // Accesseurs (avec guards de sécurité)
    int getResx() const { return _res_x; }
    int getResy() const { return _res_y; }
    int getMeshID()const;
    // Mutateurs
    void SetResx(int resx);
    void SetResy(int resy);
    void setMeshID(int meshID);
    
    // ======= Pour compatibilité (deprecated) =======
    void setResx(int resx) { SetResx(resx); }
    void setResy(int resy) { SetResy(resy); }
};

#endif