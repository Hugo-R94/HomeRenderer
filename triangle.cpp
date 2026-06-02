#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <sys/time.h>


struct Vec2
{
    float x;
    float y;
} ;

struct carre
{
    Vec2 vertex1;
    Vec2 vertex2;
    Vec2 vertex3;
    Vec2 vertex4;
} ;
struct tri
{
    Vec2 vertex1;
    Vec2 vertex2;
    Vec2 vertex3;
} ;



// void rotate_triangle(Tri triangle)
// {

// }

void rotate_around_point(Vec2& v, float speed, const Vec2& center)
{
    // translation vers le centre
    float x = v.x - center.x;
    float y = v.y - center.y;

    float cosA = std::cos(speed);
    float sinA = std::sin(speed);

    // rotation
    float rx = x * cosA - y * sinA;
    float ry = x * sinA + y * cosA;

    // translation inverse
    v.x = rx + center.x;
    v.y = ry + center.y;
}




void draw_center_point(GLFWwindow* window)
{
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    // Conversion pixels → coordonnées OpenGL (-1 à 1)
    float x = 0.0f;
    float y = 0.0f;

    glPointSize(10.0f);   // taille du point en pixels
    glBegin(GL_POINTS);

    glColor3f(1.0f, 0.0f, 0.0f); // rouge
    glVertex2f(x, y);           // centre exact

    glEnd();
}

void draw_point( Vec2 center)
{
    glPointSize(10.0f);   // taille du point en pixels
    glBegin(GL_POINTS);

    glColor3f(1.0f, 0.0f, 0.0f); // rouge
    glVertex2f(center.x, center.y);           // centre exact

    glEnd();
}

void draw_square_rotate(carre &square, Vec2 center)
{
    glBegin(GL_QUADS);

    glColor3f(1.0f, 0.4f, 0.2f);
    glVertex2f(square.vertex1.x , square.vertex1.y );
   
    glColor3f(1.0f, 0.4f, 0.2f);
    glVertex2f(square.vertex2.x, square.vertex2.y);

    glColor3f(1.0f, 0.4f, 0.2f);
    glVertex2f(square.vertex3.x, square.vertex3.y);

    glColor3f(1.0f, 0.4f, 0.2f);
    glVertex2f(square.vertex4.x , square.vertex4.y );

    rotate_around_point(square.vertex1, 0.02f, center);
    rotate_around_point(square.vertex2, 0.02f, center);
    rotate_around_point(square.vertex3, 0.02f, center);
    rotate_around_point(square.vertex4, 0.02f, center);

    glPointSize(10.0f);   // taille du point en pixels        draw_point({0.0f, 0.0f});

    glBegin(GL_POINTS);


    glEnd();
    draw_point(center);
    
}


float distance(Vec2 v1, Vec2 v2)
{
    float dx;
    float dy;

    dx = v2.x - v1.x;
    dy = v2.y - v1.y;
    return (sqrtf(dx * dx + dy * dy));
}

void draw_rotating_triangle(tri *triangle)
{
    glBegin(GL_TRIANGLES);
    static float angle = 4;
    angle = angle + 0.01;
    Vec2 center_of_rotation = {(triangle->vertex1.x + triangle->vertex2.x + triangle->vertex3.x) / 3.0f, (triangle->vertex1.y + triangle->vertex2.y + triangle->vertex3.y) / 3.0f};
    // Rotation des sommets du triangle autour du centre de rotation
    float sin_theta = sin(angle);
    float cos_theta = cos(angle);
    // printf("center = %f %f\n", center_of_rotation.x, center_of_rotation.y);

    Vec2 rotated_vertex1;
    rotated_vertex1.x = center_of_rotation.x + (triangle->vertex1.x - center_of_rotation.x)*cos_theta - (triangle->vertex1.y - center_of_rotation.y)*sin_theta;
    rotated_vertex1.y = center_of_rotation.y + (triangle->vertex1.x - center_of_rotation.x)*sin_theta + (triangle->vertex1.y - center_of_rotation.y)*cos_theta;

    Vec2 rotated_vertex2;
    rotated_vertex2.x = center_of_rotation.x + (triangle->vertex2.x - center_of_rotation.x)*cos_theta - (triangle->vertex2.y - center_of_rotation.y)*sin_theta;
    rotated_vertex2.y = center_of_rotation.y + (triangle->vertex2.x - center_of_rotation.x)*sin_theta + (triangle->vertex2.y - center_of_rotation.y)*cos_theta;

    Vec2 rotated_vertex3;
    rotated_vertex3.x = center_of_rotation.x + (triangle->vertex3.x - center_of_rotation.x)*cos_theta - (triangle->vertex3.y - center_of_rotation.y)*sin_theta;
    rotated_vertex3.y = center_of_rotation.y + (triangle->vertex3.x - center_of_rotation.x)*sin_theta + (triangle->vertex3.y - center_of_rotation.y)*cos_theta;

    // Dessin du triangle
    glColor3f(1.0f, 0.4f, 0.2f);
    glVertex2f(rotated_vertex1.x, rotated_vertex1.y);

    glColor3f(0.2f, 1.0f, 0.4f);
    glVertex2f(rotated_vertex2.x, rotated_vertex2.y);

    glColor3f(0.4f, 0.2f, 1.0f);
    glVertex2f(rotated_vertex3.x, rotated_vertex3.y);

    // float d1 = distance(rotated_vertex1, rotated_vertex2);
    // float d2 = distance(rotated_vertex2, rotated_vertex3);
    // float d3 = distance(rotated_vertex3, rotated_vertex1);

    // printf("%f %f %f\n", d1, d2, d3);
    Vec2 new_center = {
        (rotated_vertex1.x + rotated_vertex2.x + rotated_vertex3.x) / 3.0f,
        (rotated_vertex1.y + rotated_vertex2.y + rotated_vertex3.y) / 3.0f
    };

    // printf("new_center = %f %f\n", new_center.x, new_center.y);

    glEnd();
}

void draw_triangle()
{
    glBegin(GL_TRIANGLES);

    static Vec2 v1 = {-0.5f, -0.5f};
    static Vec2 v2 = { 0.5f, -0.5f};
    static Vec2 v3 = { 0.0f,  0.707f};

    // Calcul du centre du triangle
    Vec2 center = {(v1.x + v2.x + v3.x) / 3.0f, (v1.y + v2.y + v3.y) / 3.0f};

    // Rotation rigide
    rotate_around_point(v1, 0.01f, center);
    rotate_around_point(v2, 0.01f, center);
    rotate_around_point(v3, 0.01f, center);

    // Dessin du triangle
    glColor3f(1.0f, 0.4f, 0.2f);
    glVertex2f(v1.x, v1.y);

    glColor3f(0.2f, 1.0f, 0.4f);
    glVertex2f(v2.x, v2.y);

    glColor3f(0.4f, 0.2f, 1.0f);
    glVertex2f(v3.x, v3.y);

    glEnd();

    // Dessin du centre
    draw_point(center);
}

#include <GL/gl.h>
#include <math.h>

void    rotate_triangle_opengl(tri triangle)
{
    static float angle = 0.0f;

    // centre du triangle (centroïde)
    float cx = (triangle.vertex1.x +
                triangle.vertex2.x +
                triangle.vertex3.x) / 3.0f;

    float cy = (triangle.vertex1.y +
                triangle.vertex2.y +
                triangle.vertex3.y) / 3.0f;

    angle += 0.5f; // en degrés (IMPORTANT : glRotatef = degrés)

    // sauvegarde la matrice actuelle
    glPushMatrix();

    // 1) on amène le centre du triangle à l'origine
    glTranslatef(cx, cy, 0.0f);

    // 2) on tourne autour de Z (rotation 2D)
    glRotatef(angle, 0.0f, 0.0f, 1.0f);

    // 3) on remet le triangle à sa position initiale
    glTranslatef(-cx, -cy, 0.0f);

    // dessin (avec les coordonnées ORIGINALES !)
    glBegin(GL_TRIANGLES);

    glColor3f(1.0f, 0.4f, 0.2f);
    glVertex2f(triangle.vertex1.x, triangle.vertex1.y);

    glColor3f(0.2f, 1.0f, 0.4f);
    glVertex2f(triangle.vertex2.x, triangle.vertex2.y);

    glColor3f(0.4f, 0.2f, 1.0f);
    glVertex2f(triangle.vertex3.x, triangle.vertex3.y);

    glEnd();

    // restaure la matrice précédente (ULTRA IMPORTANT)
    glPopMatrix();
}

void get_size(GLFWwindow *window)
{
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    std::cout << "widht = " << width << " | height = " << height << std::endl;
    float ratio_x, ratio_y;

    ratio_x = (float)height / (float)width;
    ratio_y = (float)width / (float)height;
    std::cout << "ratiox = " << ratio_x << " | ratioy = " << ratio_y << std::endl;
}

carre *create_square(float  size, Vec2 center)
{
    carre *square;
    square = (carre *)malloc(sizeof(carre));
    square->vertex1 = {(-size/2 + center.x ) , (-size/2 + center.y )};
    square->vertex2 = {(-size/2 + center.x ), (size/2 + center.y ) };
    square->vertex3 = {(size/2 + center.x ) , (size/2 + center.y ) };
    square->vertex4 = {(size/2 + center.x ) , (-size/2 + center.y )};

    return square;
}
#include <chrono>
#include <thread>

long	get_time_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}
#include <sys/time.h>
#include <unistd.h>

void	cap_framerate(long elapsed)
{
	long	target_time;

	target_time = 1000 / 60;
    std::cout << "fps = " <<  elapsed << std::endl;
	if (elapsed < target_time)
		usleep((target_time - elapsed) * 1000);
}


int main()
{
    if (!glfwInit())
    {
        std::cerr << "Erreur GLFW\n";
        return -1;
    }
    tri* triangle;
    triangle = (tri*) malloc(sizeof(tri));

    triangle->vertex1 = {-0.5f, -0.5f};
    triangle->vertex2 = {0.5f, -0.5f};
    triangle->vertex3 = {0.0f, 0.707f};
    carre *square;
    square =create_square(1.0f, {0.5f, 0.5f});
    // square->vertex1 = {-0.5f, -0.5f};
    // square->vertex2 = {-0.5f, 0.5f};
    // square->vertex3 = {0.5f, 0.5f};
    // square->vertex4 = {0.5f, -0.5f};
    GLFWwindow* window = glfwCreateWindow(1920, 1080, "Triangle sans GLAD", NULL, NULL);

    struct timeval start;
    struct timeval finish;
    long    time[2];

    if (!window)
    {
        std::cerr << "Impossible de créer la fenêtre\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    get_size(window);
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    float aspect = (float)width / (float)height;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    std::cout << "aspect = " << aspect << std::endl;
    if (aspect >= 1.0f)
        glOrtho(-aspect, aspect, -1.0f, 1.0f, -1.0f, 1.0f);
    else
        glOrtho(-1.0f, 1.0f, -1.0f, 1.0f/aspect, 1.0f/aspect, -1.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();


    while (!glfwWindowShouldClose(window))
    {
        // gettimeofday(&start,NULL);
        time[0] = get_time_ms();
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        draw_square_rotate(*square, {0.0f, 0.2f});
        // draw_rotating_triangle(triangle);
        // draw_point({0.0f, 0.0f});

        glfwSwapBuffers(window);
        glfwPollEvents();
        time[1] = get_time_ms();
        cap_framerate(time[1] - time[0]);
    }
    free(triangle);
    glfwTerminate();
    return 0;
}

void create_and_display_cube()
{
    carre *square;
    square =create_square(1.0f, {0.5f, 0.5f});
}