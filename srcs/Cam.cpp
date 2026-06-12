#include "../includes/Cam.hpp"

Cam::Cam(int fov) : Node3D(), _fov(fov){}

Cam::~Cam(){
	_cam_H = 800;
	_cam_W = 1200;
}

int Cam::getFov()const{return _fov;}

void Cam::setFov(int fov){_fov = fov;}

void Cam::setCamH(int camH){_cam_H = camH;}
void Cam::setCamW(int camW){_cam_W = camW;}

void Cam::setZbufferID(int id, float zbuffer){_zbuffer[id] = zbuffer;}

std::vector<float>& Cam::getZbuffer() { return _zbuffer; }

int  Cam::getCamH() const{return _cam_H;}

int  Cam::getCamW() const{return _cam_W;}

std::vector<Pixel> &Cam::getFramebuffer(){return _framebuffer;}