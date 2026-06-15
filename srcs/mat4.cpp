#include "../includes/HomeRenderer.hpp"
#include <cmath>

// Rotation autour de l'axe X d'un angle a (radians)
// Laisse X intact, fait tourner Y et Z :
// y' = cos(a)*y - sin(a)*z
// z' = sin(a)*y + cos(a)*z
Mat4 rotationX4(float a) {
    return {{
        {1,      0,        0,       0},//x
        {0, cosf(a), -sinf(a),      0},//y
        {0, sinf(a),  cosf(a),      0},//z
        {0,      0,        0,       1}
    }};
}

// Rotation autour de l'axe Y d'un angle a (radians)
// Laisse Y intact, fait tourner X et Z :
// x' =  cos(a)*x + sin(a)*z
// z' = -sin(a)*x + cos(a)*z
Mat4 rotationY4(float a) {
    return {{
        { cosf(a), 0, sinf(a), 0},//x
        {       0, 1,       0, 0},//y
        {-sinf(a), 0, cosf(a), 0},//z
        {       0, 0,       0, 1}
    }};
}

// Rotation autour de l'axe Z d'un angle a (radians)
// Laisse Z intact, fait tourner X et Y :
// x' = cos(a)*x - sin(a)*y
// y' = sin(a)*x + cos(a)*y
Mat4 rotationZ4(float a) {
    return {{
        {cosf(a), -sinf(a), 0, 0},//x
        {-sinf(a),  -cosf(a), 0, 0},//y
        {      0,        0, 1, 0},//z
        {      0,        0, 0, 1}
    }};
}

// Multiplication de deux matrices 4x4 : R = A * B
// Chaque element R[i][j] = somme des produits ligne i de A par colonne j de B
// C'est ce qui permet de chainer les transformations en une seule matrice
Mat4 multiplyMat4(const Mat4& A, const Mat4& B) {
    Mat4 R = {};
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            for (int k = 0; k < 4; k++)
                R.m[i][j] += A.m[i][k] * B.m[k][j];
    return R;
}

// Multiplication Mat4 * Vec4
// Applique une transformation matricielle a un vertex
// Le w=1 du vertex active la colonne de translation
// Apres projection, w != 1 et il faudra diviser xyz par w
Vec4 mulMat4Vec4(const Mat4& M, const Vec4& v) {
    return {
        M.m[0][0]*v.x + M.m[0][1]*v.y + M.m[0][2]*v.z + M.m[0][3]*v.w,//x
        M.m[1][0]*v.x + M.m[1][1]*v.y + M.m[1][2]*v.z + M.m[1][3]*v.w,//y
        M.m[2][0]*v.x + M.m[2][1]*v.y + M.m[2][2]*v.z + M.m[2][3]*v.w,//z
        M.m[3][0]*v.x + M.m[3][1]*v.y + M.m[3][2]*v.z + M.m[3][3]*v.w//w
    };
}

// Construit la model matrix : transforme les coordonnees locales en coordonnees monde
// Ordre obligatoire : T * R * S
// Scale d'abord (taille), puis Rotation (orientation), puis Translation (position)
// Inverser l'ordre donnerait un resultat faux (ex: T*S translaterait avant de scaler)
Mat4 modelMatrix(Vec3 pos, Vec3 rot, Vec3 scale) {

    // Scale : etire le mesh sur chaque axe
    Mat4 S = {{
        {scale.x, 0,       0,       0},//x
        {0,       scale.y, 0,       0},//y
        {0,       0,       scale.z, 0},//z
        {0,       0,       0,       1}//w
    }};

    // Translation : deplace le mesh en position monde
    Mat4 T = {{
        {1, 0, 0, pos.x},//x
        {0, 1, 0, pos.y},//y
        {0, 0, 1, pos.z},//z
        {0, 0, 0,     1}//w
    }};

    // Rotation chainee : Rz * Ry * Rx
    // L'ordre signifie : rotation X appliquee en premier, Z en dernier
    Mat4 R = multiplyMat4(rotationZ4(rot.z), multiplyMat4(rotationY4(rot.y), rotationX4(rot.x)));

    // Resultat final : T * R * S
    return multiplyMat4(T, multiplyMat4(R, S));
}

Mat4 viewMatrix(Vec3 camPos) {
    return {{
        {1, 0, 0, -camPos.x},
        {0, 1, 0, -camPos.y},
        {0, 0, 1, -camPos.z},
        {0, 0, 0,          1}
    }};
}

Mat4 projectionMatrix(float fov, float aspect, float near, float far) {
    float f = 1.0f / tanf(fov / 2.0f);
    return {{
        {f / aspect, 0,  0,                             0},
        {0,          f,  0,                             0},
        {0,          0,  (far + near) / (near - far),  (2 * far * near) / (near - far)},
        {0,          0,  -1,                            0}
    }};
}