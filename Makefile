CXX = c++
CXXFLAGS = -Wall -Wextra -std=c++14 -g

# Includes directories
INCLUDES = -Iincludes -Iimgui -Iglad/include

# ImGui source files
IMGUI_SRC = \
    imgui/imgui.cpp \
    imgui/imgui_draw.cpp \
    imgui/imgui_tables.cpp \
    imgui/imgui_widgets.cpp \
    imgui/backends/imgui_impl_glfw.cpp \
    imgui/backends/imgui_impl_opengl3.cpp

# GLAD source files
GLAD_SRC = glad/src/glad.c

# Project source files
SRC = srcs/main2.cpp\
    srcs/Data.cpp\
    srcs/test.cpp\
    srcs/inputs.cpp\
    srcs/UI/RenderUI.cpp\
    srcs/UI/BarMenu.cpp\
    srcs/UI/Outliner.cpp\
    srcs/UI/viewport.cpp\
    srcs/VertexOperation.cpp\
	srcs/mesh.cpp\
	srcs/Node3D.cpp\
    srcs/renderWireframe.cpp\
    srcs/renderAmbiant.cpp\
    srcs/Cam.cpp\
    srcs/mat4.cpp\
	srcs/rasterize.cpp

# Object files
OBJ = $(SRC:.cpp=.o) $(IMGUI_SRC:.cpp=.o) $(GLAD_SRC:.c=.o)

# Output executable
NAME = HomeRenderer

# Libraries
LIBS = -lglfw -lGL

# Compilation targets
all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) $(LIBS) -o $(NAME)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

%.o: %.c
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
