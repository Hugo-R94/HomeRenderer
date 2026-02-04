#ifndef HOMERENDERER_HPP
#define HOMERENDERER_HPP

/* GLFW */

/* ImGui core */
#include "colors.hpp"
#include "Data.hpp"
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <math.h>

void	setup_bar_menu(GLFWwindow *window);
void draw_outliner(Data& data);
void draw_proprieties(Data &data);
void	handle_input(Data *data);
void display_viewport(Data &data);
void display_double_viewport(Data &data);
#endif
