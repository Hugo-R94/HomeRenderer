#include "Data.hpp"
#include "HomeRenderer.hpp"

Data::Data()
{
	const GLFWvidmode* screen = glfwGetVideoMode(glfwGetPrimaryMonitor());
	_res_x  = screen->width;
	_res_y = screen->height;
	is_outliner_visible = true;
}

Data::~Data()
{
}

int	Data::GetResx(){
	return _res_x;
}

int	Data::GetResy(){
	return _res_y;
}

void	Data::setResx(int resx){
	_res_x = resx;
}

void	Data::setResy(int resy){
	_res_x = resy;
}