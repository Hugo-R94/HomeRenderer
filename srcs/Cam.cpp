#include "../includes/Cam.hpp"

Cam::Cam(int fov) : Node3D(), _fov(fov){}

Cam::~Cam(){}

int Cam::getFov()const{return _fov;}

void Cam::setFov(int fov){_fov = fov;}