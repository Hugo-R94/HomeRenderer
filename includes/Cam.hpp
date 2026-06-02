#pragma once  // add this as the very first line

#include "HomeRenderer.hpp"

class Cam : public Node3D
{
	private :
		int _fov;

	public :
		Cam(int fov);
		~Cam();

		int getFov() const;
		void setFov(int fov);
};