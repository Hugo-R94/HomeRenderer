#include "HomeRenderer.hpp"
#include <cmath>

Mesh::Mesh(std::string path): Node3D(), _pathObj(path){
    if (!path.empty())
        loadMesh();
}

Mesh::~Mesh(){}

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

const std::vector<Face>& Mesh::getFaces() const { return _faces; }
const std::vector<Face>& Mesh::getCulledFaces() const { return _faces; }

const std::vector<Vec3>& Mesh::getVerticesLocal() const { return _verticesLocal; }
const std::vector<Vec3>& Mesh::getWorldVertices() const { return _verticesWorld; }
const std::vector<float>& Mesh::getDotFace() const {return _dot;}

void Mesh::loadMesh()
{
    std::ifstream file(_pathObj);
    if (!file.is_open())
        std::cout << "ERREUR : impossible d'ouvrir " << _pathObj << std::endl;
    std::string line;
    while (std::getline(file, line))
    {
        
        std::istringstream ss(line);
        std::string token;
        ss >> token;
        if (token == "v")
        {
            Vec3 v;
            ss >> v.x >> v.y >> v.z;
            _verticesLocal.push_back(v); // local, pas world
        }
        else if (token == "vn")
        {
            Vec3 n;
            ss >> n.x >> n.y >> n.z;
            _normals.push_back(n);
        }
        else if (token == "vt")
        {
            Vec2 uv;
            ss >> uv.x >> uv.y;
            _uvs.push_back(uv);
        }
        else if (token == "f")
        {
            std::vector<Index> polyIndices;
            std::string t;
            while (ss >> t)
                polyIndices.push_back(parseIndex(t));
            for (size_t i = 1; i + 1 < polyIndices.size(); i++)
            {
                Face face;
                face.i[0] = polyIndices[0];
                face.i[1] = polyIndices[i];
                face.i[2] = polyIndices[i + 1];

                // calcul normale locale
                Vec3 a = _verticesLocal[face.i[0].v];
                Vec3 b = _verticesLocal[face.i[1].v];
                Vec3 c = _verticesLocal[face.i[2].v];

                Vec3 ab = {b.x-a.x, b.y-a.y, b.z-a.z};
                Vec3 ac = {c.x-a.x, c.y-a.y, c.z-a.z};

                // ab × ac (et non ac × ab)
                face.LocalNormal = {
                    ab.y*ac.z - ab.z*ac.y,
                    ab.z*ac.x - ab.x*ac.z,
                    ab.x*ac.y - ab.y*ac.x
                };

                _faces.push_back(face);
            }
        }
    }
    setOriginCenter();
    setPos({0.0f, 0.0f, 5.0f}); //spawn en face de la camera
    setRot({0.0f, 3.5f, 0.0f});
    setScale({3.0f, 3.0f, 3.0f});//size visible
}

//getter
const Vec3& Mesh::getMeshScale()
{
	return _scale;
}

std::string Mesh::getPath()const{return _pathObj;}

const Vec3& Mesh::getCenter()const{
    return  _center;
}

void Mesh::setCenter(Vec3 center)
{
    _center = center;
}

void Mesh::setScale(Vec3 scale){
	_scale = scale;
}

Vec3 Mesh::CalculateWorldVertex(Vec3 vertex)
{
    Mat4 M = modelMatrix(getPos(), getRot(), getMeshScale());
    Vec4 v = {vertex.x, vertex.y, vertex.z, 1.0f};
    Vec4 result = mulMat4Vec4(M, v);
    return {result.x, result.y, result.z};
}

void Mesh::setFaces(std::vector<Face> faces){_faces = faces;}
void Mesh::setCulledFaces(std::vector<Face> faces){_culledFace = faces;}

std::vector<Vec3> *Mesh::CalculateWorldVertices()
{
    _verticesWorld = _verticesLocal;
    for (size_t i = 0; i < _verticesLocal.size(); i++)
        _verticesWorld[i] = CalculateWorldVertex(_verticesLocal[i]);
    return &_verticesWorld;
}

void Mesh::setDotFace(std::vector<float> dotlist){_dot = dotlist;}

void Mesh::setNormalFace(Vec3 Normals, int id){_normals[id] = Normals;}

void Mesh::setOriginCenter()
{
    if (_verticesLocal.empty()) return;
    
    Vec3 min = _verticesLocal[0];
    Vec3 max = _verticesLocal[0];
    
    for (const Vec3& v : _verticesLocal)
    {
        if (v.x < min.x) min.x = v.x;
        if (v.y < min.y) min.y = v.y;
        if (v.z < min.z) min.z = v.z;
        if (v.x > max.x) max.x = v.x;
        if (v.y > max.y) max.y = v.y;
        if (v.z > max.z) max.z = v.z;
    }
    _center = {(min.x + max.x) / 2.0f,
               (min.y + max.y) / 2.0f,
               (min.z + max.z) / 2.0f};

    std::vector<Vec3> NewLocalVertices = _verticesLocal;
    for (size_t i = 0; i < _verticesLocal.size(); i++)
    {
        NewLocalVertices[i].x = _verticesLocal[i].x - _center.x;
        NewLocalVertices[i].y = _verticesLocal[i].y - _center.y;
        NewLocalVertices[i].z = _verticesLocal[i].z - _center.z;
    }
    _verticesLocal = NewLocalVertices;
    _center = {0.0f, 0.0f, 0.0f};
}   


std::vector<Face> sortFacesByDepth(std::vector<Face> faces, Cam camera, std::vector<Vec3> worldVertices)
{
    std::vector<std::pair<float , Face>> FacesAndDepth;
    FacesAndDepth.resize(faces.size()); 
    
    for (size_t i = 0; i < faces.size(); i++)
    {
        FacesAndDepth[i].second = faces[i];

        int a = faces[i].i[0].v;
        int b = faces[i].i[1].v;
        int c = faces[i].i[2].v;

        float x1 = camera.getPos().x;
        float y1 = camera.getPos().y;
        float z1 = camera.getPos().z;

        float x2 = (worldVertices[a].x + worldVertices[b].x + worldVertices[c].x) / 2;
        float y2 = (worldVertices[a].y + worldVertices[b].y + worldVertices[c].y) / 2;
        float z2 = (worldVertices[a].z + worldVertices[b].z + worldVertices[c].z) / 2;

        float depth = sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2) + pow(z1 - z2, 2));
        FacesAndDepth[i].first = depth;
    }
    std::sort(FacesAndDepth.begin(), FacesAndDepth.end(),
        [](const std::pair<float, Face>& a, const std::pair<float, Face>& b) {
            return a.first > b.first; // '>' pour décroissant
        });
    std::vector<Face> SortedFaces;
    for (const auto& pair : FacesAndDepth)
        SortedFaces.push_back(pair.second);
    return SortedFaces;

}

void Mesh::updateMeshData()
{
    CalculateWorldVertices();
}

Mat3 rotationX(float a){
	return {1, 0, 0,
			0, (float)cos(a), (float)-sin(a),
			0, (float)sin(a), (float)cos(a)};
}

Mat3 rotationY(float a){
	return {(float)cos(a), 0, (float)sin(a),
			0, 1, 0,
			-(float)sin(a), 0, (float)cos(a)};
}

Mat3 rotationZ(float a){
	return { (float)cos(a), (float)-sin(a), 0,
			(float)sin(a), (float)cos(a), 0,
			0, 0, 1 };
}

Mat3 multiply(const Mat3& A, const Mat3& B)
{
    Mat3 R;


    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            R.m[i][j] =
                A.m[i][0] * B.m[0][j] +
                A.m[i][1] * B.m[1][j] +
                A.m[i][2] * B.m[2][j];
        }
    }


    return R;
}


Mat3 buildRotationMatrix(const Vec3& rot)
{
    Mat3 Rx = rotationX(rot.x);
    Mat3 Ry = rotationY(rot.y);
    Mat3 Rz = rotationZ(rot.z);


    // ordre : Rz * Ry * Rx
    return multiply(Rz, multiply(Ry, Rx));
}

// Helpers pour afficher
void printVec3(const std::string& label, const Vec3& v) {
    std::cout << label << " : (" << v.x << ", " << v.y << ", " << v.z << ")\n";
}

Vec2 projectVertex(Vec3 v,Cam camera)
{
        // position relative a la camera
        float x = v.x - camera.getPos().x;
        float y = v.y - camera.getPos().y;
        float z = v.z - camera.getPos().z;

            
        // weak perspective projection
        Vec2 projected;
        if (z <= 0)
        {
            projected.x = 99999;
            projected.y = 99999;
        }
        else
        {
            projected.x = (x * camera.getFov()) / z;
            projected.y = (y * camera.getFov()) / z;
        }

        return projected;

}

std::vector<Vec2> ProjectVertices(std::vector<Vec3> Vertices, Cam camera)
{
    std::vector<Vec2> projectedVertices;

    for (size_t i = 0; i < Vertices.size(); i++)
    {
        Vec3 v = Vertices[i];

        Vec2 projected = projectVertex(v, camera);

        projectedVertices.push_back(projected);
    }
    return projectedVertices;
}