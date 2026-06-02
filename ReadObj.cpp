#include <fstream>
#include <sstream>
#include <vector>

struct Vec3 { float x, y, z; };
struct Vec2 { float u, v; };

struct Index {
    int v = -1, vt = -1, vn = -1;
};

struct Face {
    Index i[3]; // triangle
};

struct ObjMesh {
    std::vector<Vec3> vertices;
    std::vector<Vec3> normals;
    std::vector<Vec2> uvs;
    std::vector<Face> faces;
};

// Parse un token de face : "1", "1/2", "1/2/3", "1//3"
Index parseIndex(const std::string& token) {
    Index idx;
    std::stringstream ss(token);
    std::string part;
    int i = 0;
    while (std::getline(ss, part, '/')) {
        if (!part.empty()) {
            int val = std::stoi(part) - 1; // OBJ est 1-indexé
            if (i == 0) idx.v  = val;
            if (i == 1) idx.vt = val;
            if (i == 2) idx.vn = val;
        }
        i++;
    }
    return idx;
}

ObjMesh loadOBJ(const std::string& path) {
    ObjMesh mesh;
    std::ifstream file(path);
    std::string line;

    while (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string token;
        ss >> token;

        if (token == "v") {
            Vec3 v; ss >> v.x >> v.y >> v.z;
            mesh.vertices.push_back(v);
        } else if (token == "vn") {
            Vec3 n; ss >> n.x >> n.y >> n.z;
            mesh.normals.push_back(n);
        } else if (token == "vt") {
            Vec2 uv; ss >> uv.u >> uv.v;
            mesh.uvs.push_back(uv);
        } else if (token == "f") {
            // Supporte les polygones (pas seulement les triangles)
            std::vector<Index> polyIndices;
            std::string t;
            while (ss >> t)
                polyIndices.push_back(parseIndex(t));

            // Fan triangulation si > 3 sommets
            for (size_t i = 1; i + 1 < polyIndices.size(); i++) {
                Face face;
                face.i[0] = polyIndices[0];
                face.i[1] = polyIndices[i];
                face.i[2] = polyIndices[i + 1];
                mesh.faces.push_back(face);
            }
        }
    }
    return mesh;
}

int main(){
	std::string path = "/home/hugz/Documents/PROJET_PERSO/HomeRenderer/spere.obj";
	loadOBJ(path);
	ObjMesh mesh = loadOBJ("model.obj");
}