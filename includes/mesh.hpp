
#ifndef MESH_HPP
#define MESH_HPP

#include "HomeRenderer.hpp"
#include <iostream>


class Cam;
class DataGlobal;

struct Index {
    int v = -1, vt = -1, vn = -1;
};

struct Face {
    Index i[3];
	Vec3 LocalNormal;
};

class Mesh : public Node3D
{
private:
	std::vector<Vec3>	_verticesLocal;
	std::vector<Vec3>	_verticesWorld;
	std::vector<Vec3>	_localVN;
	std::vector<Vec3>	_WorldVN;
	std::vector<Vec2>	_uvs;
	std::vector<Face>	_faces;
	std::vector<float> 	_dot;
	std::vector<Face>	_culledFace;
	std::string 		_pathObj;
	Vec3				_center;
	Vec3 				_scale;
	Texture				_txt;
	Texture				_normalTxt;

public:
	Mesh(std::string path);
	~Mesh();

	//getter
	const Vec3&	getMeshScale();

	std::vector<Vec3>* CalculateWorldVertices();
	Vec3 CalculateWorldVertex(Vec3 vertex);
	void	setScale(Vec3 scale);
	void	setOriginCenter();
	void	setCenter(Vec3 center);
	void	setNormalFace(Vec3 normal, int id);
	void 	setFaces(std::vector<Face> faces);
	void 	setCulledFaces(std::vector<Face> faces);
	// dans mesh.hpp
	void addVertexLocal(const Vec3& v) { _verticesLocal.push_back(v); }
	void	loadMesh();
	void updateMeshData();
	// dans mesh.hpp
	const std::vector<Face>& getFaces() const;
	const std::vector<Face>& getCulledFaces() const;
	const std::vector<Vec3>& getVerticesLocal() const;
	const std::vector<Vec3>& getNormalsLocal() const;
	const std::vector<Vec3>& getWorldVertices() const;
	const std::vector<Vec2>& getUV() const;
	const std::vector<float>& getDotFace()const;
	const Texture& getTexture()const;
	const Texture& getNormalTxt()const;
	void setDotFace(std::vector<float> dotlist);
	const Vec3& getCenter()const;
	std::string getPath() const;
};

Vec3 rotatedVertex(const Mat3& M, const Vec3& v);
Vec3 translatedVertex(const Vec3& MeshPos, const Vec3& Vpos);
void renderWireframe(Mesh mesh, Cam camera, int W, int H);
void renderAmbiant(Mesh mesh, Cam camera , int W, int H);
std::vector<Vec2> ProjectVertices(std::vector<Vec3> Vertices, Cam camera);
Vec2 toNDC(Vec2 p, float width, float height);
Vec2 projectVertex(Vec3 v,Cam camera);
std::vector<Face> sortFacesByDepth(std::vector<Face> faces, Cam camera, std::vector<Vec3> worldVertices);
Mat3 buildRotationMatrix(const Vec3& rot);
Mat4 modelMatrix(Vec3 pos, Vec3 rot, Vec3 scale);
Vec4 mulMat4Vec4(const Mat4& M, const Vec4& v);
Mat4 multiplyMat4(const Mat4& A, const Mat4& B);
Mat4 projectionMatrix(float fov, float aspect, float near, float far);
Mat4 viewMatrix(Vec3 camPos);
void processInput(GLFWwindow *window, std::vector<Mesh> &meshes, int *displaymode, DataGlobal &data);
Vec2 project(const Vec3& vertex, const Mat4& MVP);
Mat4 rotationZ4(float a);
Mat4 rotationY4(float a);
Mat4 rotationX4(float a);
void renderAmbiant1(Mesh mesh, Cam &camera , int W, int H, Mat4 MVP);

#endif