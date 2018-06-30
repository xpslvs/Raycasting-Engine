/* main.cc */
#include <cstring>
#include <cstddef>
#include <cassert>
#include <cstdint>
#include <cmath>
extern "C" {
# include <SDL2/SDL.h>
}
#include "vector.hh"
#include "graphics.hh"
#include "state.hh"

#define PI 3.14
#define RENDER_DISTANCE 100
#define WIDTH 800
#define HEIGHT 600

#define DEG_TO_RAD(d) (((d) * PI) / 180)
#define RAD_TO_DEG(r) (((r) * 180) / PI)

bool quadratic_roots(float, float, float, float *, float *);
void poll(void);
void rotate_axis_x(float, Vector3f&);
void rotate_axis_y(float, Vector3f&);
uint32_t calculate_pixel(int, int);

struct Color
{
    uint8_t r, g, b, a;
    
    Color(void) : Color(0, 0, 0) {}
    Color(uint32_t c)
    {
        this->r = (c & (0xff << 24)) >> 24;
        this->g = (c & (0xff << 16)) >> 16;
        this->b = (c & (0xff << 8)) >> 8;
        this->a = (c & (0xff));
    }

    Color(uint8_t r, uint8_t g, uint8_t b) : Color(r, g, b, 0xff) {}

    Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        this->r = r;
        this->g = g;
        this->b = b;
        this->a = a;
    }

    operator int(void) const
    {
        return (this->r<<24)|(this->g<<16)|(this->b<<8)|(this->a);
    }
};

struct Line
{
    Vector3f point, delta;

    Line(const Vector3f& point, const Vector3f& delta)
    {
        this->point = point;
        this->delta = delta.normalize();
    }
};

struct Sphere
{
    Sphere(){}

    Vector3f center;
    float    radius;
    Color    color;

    bool intersects(const Line& line,
                          Vector3f *point1 = nullptr,
                          Vector3f *point2 = nullptr) const
    {
        Vector3f Q = line.point - this->center;
        float b = 2 * line.delta.dot(Q);
        float c = Q.norm() - this->radius * this->radius;
        float t1, t2;
        if(quadratic_roots(1.0f, b, c, &t1, &t2))
        {
            if(t1 >= 0 && point1 != nullptr)
                *point1 = line.point + (line.delta * t1);
            if(t2 >= 0 && point2 != nullptr)
                *point2 = line.point + (line.delta * t2);
            return t1 >= 0 && t2 >= 0;
        }
        else
            return false;
    }
};

struct Camera
{
    Vector3f dir, pos;
    float    fov_x, fov_y;
    float    angle_x, angle_y;
};

Line get_coord_line(int, int, const Camera&);
void move(const Vector3i&);
void direct(const Vector2i&);

State state;
Camera camera;
Sphere object;

int main(int argc, char *argv[])
{
    camera.dir = {0, 0, 1};
    camera.pos = {0, 0, 0};
    camera.fov_x = DEG_TO_RAD(90);
    camera.fov_y = DEG_TO_RAD(75);
    camera.angle_x = 0.0f;
    camera.angle_y = 0.0f;

    object.center = {0, 0, 10};
    object.radius = 4;
    object.color  = Color(255, 0, 0);

    state.running = true;
    graphics::init(WIDTH, HEIGHT);

    while(state.running)
    {
        poll();
        
        for(int y = 0; y < HEIGHT; ++y)
        {
            for(int x = 0; x < WIDTH; ++x)
            {
                graphics::set_pixel(x, y, calculate_pixel(x, y));
            }
        }
        printf("POSITION: {%f, %f, %f}\n", camera.pos[0], 
                camera.pos[1], camera.pos[2]);
        printf("ANGLE (DEG) (X, Y): (%f, %f)\n", RAD_TO_DEG(camera.angle_x),
                RAD_TO_DEG(camera.angle_y));

        graphics::clear();
        graphics::load();
        graphics::refresh();
    }

    graphics::quit();
    return 0;
}

uint32_t calculate_pixel(int x, int y)
{
    Line line = get_coord_line(x, y, camera);
    Vector3f entr, exit, point;

    if(object.intersects(line, &entr, &exit))
    {
        return object.color;
    }
    return Color(255, 255, 255);
}

void move(const Vector3i& v)
{
    camera.pos += (Vector3f)v / 4;
}

void direct(const Vector2i& v)
{
    camera.angle_x += v[0] / 4;
    camera.angle_y += v[1] / 4;
}

void poll(void)
{
    SDL_Event event;
    Vector3i delta{};
    Vector2i angle{};
    while(SDL_PollEvent(&event))
    {
        switch(event.type)
        {
        case SDL_QUIT:
            state.running = false;
            break;
        case SDL_KEYDOWN:
            switch(event.key.keysym.sym)
            {
            case SDLK_w:
                delta[0]++;
                break;
            case SDLK_s:
                delta[0]--;
                break;
            case SDLK_a:
                delta[2]--;
                break;
            case SDLK_d:
                delta[2]++;
                break;
            case SDLK_SPACE:
                delta[1]++;
                break;
            case SDLK_LSHIFT:
                delta[1]--;
                break;
            case SDLK_UP:
                angle[1]++;
                break;
            case SDLK_DOWN:
                angle[1]--;
                break;
            case SDLK_LEFT:
                angle[0]--;
                break;
            case SDLK_RIGHT:
                angle[0]++;
                break;
            }
            move(delta);
            direct(angle);
            break;
        case SDL_MOUSEMOTION:
            break;
        }
    }
}

bool quadratic_roots(float a, float b, float c, float *x1, float *x2)
{
    float y = -(b / a) / 2;
    float z = y * y - c;
    if(z < 0)
        return false;
    z = std::sqrt(z);
    *x1 = y + z;
    *x2 = y - z;

    return true;
}

Line get_coord_line(int x, int y, const Camera& cam)
{
    const float rad_x = ((x / WIDTH) - 0.5) * cam.fov_x;
    const float rad_y = -((y / HEIGHT) - 0.5) * cam.fov_y;

    Vector3f dir = cam.dir;
    rotate_axis_x(rad_y, dir);
    rotate_axis_y(rad_x, dir);

    return Line(cam.pos, dir);
}

void rotate_axis_x(float rad, Vector3f& v)
{
    const float s = std::sin(rad);
    const float c = std::cos(rad);
    v = {
        (v[0]),
        (v[1] * c - v[2] * s),
        (v[1] * s + v[2] * c)
    };
}

void rotate_axis_y(float rad, Vector3f& v)
{
    const float s = std::sin(rad);
    const float c = std::cos(rad);
    v = {
        (v[0] * c + v[2] * s),
        (v[1]),
        (-v[0] * s + v[2] * c)
    };
}
