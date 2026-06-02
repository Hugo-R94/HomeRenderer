#include "HomeRenderer.hpp"

Node3D::Node3D(): _position({0, 0, 0}),
	  _rotation({0, 0, 0})
{}

Node3D::~Node3D(){}

const Vec3 Node3D::getPos()const{
 return _position;
}

const Vec3 Node3D::getRot()const{
	return _rotation;
}

void Node3D::setPos(Vec3 pos){
	_position = pos;
}

void Node3D::setRot(Vec3 rot){
	_rotation = rot;
}