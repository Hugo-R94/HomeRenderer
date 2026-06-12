#pragma once  // add this as the very first line

#include "HomeRenderer.hpp"

class Cam : public Node3D
{
	private :
		int _fov;
		// std::vector<Vec3> _framebuffer;
		std::vector<Pixel> _framebuffer;
		// bool			_framebuffer;
		std::vector<float> _zbuffer;
		int					_cam_H;
		int					_cam_W;

	public :
		Cam(int fov);
		~Cam();

		int getFov() const;
		int getCamH() const;
		int getCamW() const;
		void setFov(int fov);
		void setCamH(int camH);
		void setCamW(int camW);		
		void setZbufferID(int id, float zbuffer);
		std::vector<Pixel> &getFramebuffer();
		std::vector<float>& getZbuffer(); // référence, pas const
};

