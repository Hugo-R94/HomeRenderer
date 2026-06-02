#include "HomeRenderer.hpp"



class Node3D
{
private:
	Vec3 _position;
	Vec3 _rotation;

public:
	Node3D();
	virtual ~Node3D() = 0;

	//getter
	const Vec3 getPos()const;
	const Vec3 getRot()const;

	//setter
	void	setPos(Vec3 position);
	void	setRot(Vec3 rotation);
};
