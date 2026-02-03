CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++11

INCLUDES = -Iincludes -Iimgui

IMGUI_SRC = \
    imgui/imgui.cpp \
    imgui/imgui_draw.cpp \
    imgui/imgui_tables.cpp \
    imgui/imgui_widgets.cpp \
    imgui/backends/imgui_impl_glfw.cpp \
    imgui/backends/imgui_impl_opengl3.cpp

SRC = srcs/test.cpp
OBJ = $(SRC:.cpp=.o) $(IMGUI_SRC:.cpp=.o)

NAME = HomeRenderer

LIBS = -lglfw -lGL

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) $(LIBS) -o $(NAME)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
