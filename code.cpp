#include <SDL.h>
#include <emscripten.h>
#include <cmath>

#define WIDTH 800
#define HEIGHT 800
#define CUBE_SIZE 300

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
Uint32 lastTime = 0;

struct Point3D {
    double x, y, z;
};

struct Point2D {
    int x, y;
};

class Shape {
protected:
    virtual Point2D project(Point3D p) {
        Point2D projected;
        projected.x = (p.x * CUBE_SIZE) + (WIDTH / 2);
        projected.y = (-p.y * CUBE_SIZE) + (HEIGHT / 2);
        return projected;
    }

public:
    virtual void Update(float deltaTime) = 0;
    virtual void Render(SDL_Renderer* renderer) = 0;
};

class Cube : public Shape {
private:
    Point3D points[8] = {
        {-0.5, -0.5, -0.5},
        {0.5, -0.5, -0.5},
        {0.5, 0.5, -0.5},
        {-0.5, 0.5, -0.5},
        {-0.5, -0.5, 0.5},
        {0.5, -0.5, 0.5},
        {0.5, 0.5, 0.5},
        {-0.5, 0.5, 0.5}
    };
    double angle = 0.0;

    void rotateX(double rad) {
        double cosVal = cos(rad);
        double sinVal = sin(rad);
        for (int i = 0; i < 8; i++) {
            double y = points[i].y;
            points[i].y = y * cosVal - points[i].z * sinVal;
            points[i].z = y * sinVal + points[i].z * cosVal;
        }
    }

    void rotateY(double rad) {
        double cosVal = cos(rad);
        double sinVal = sin(rad);
        for (int i = 0; i < 8; i++) {
            double x = points[i].x;
            points[i].x = x * cosVal + points[i].z * sinVal;
            points[i].z = -x * sinVal + points[i].z * cosVal;
        }
    }

    void rotateZ(double rad) {
        double cosVal = cos(rad);
        double sinVal = sin(rad);
        for (int i = 0; i < 8; i++) {
            double x = points[i].x;
            points[i].x = x * cosVal - points[i].y * sinVal;
            points[i].y = x * sinVal + points[i].y * cosVal;
        }
    }

public:
    virtual void Update(float deltaTime) override {
        double rotationSpeed = 0.5; // 0.5 radians per second
        angle += rotationSpeed * deltaTime;
        double rad = rotationSpeed * deltaTime;
        rotateX(rad);
        rotateY(rad * 0.6);
        rotateZ(rad * 0.2);
    }

    virtual void Render(SDL_Renderer* renderer) override {
        for (int i = 0; i < 4; i++) {
            Point2D p1 = project(points[i]);
            Point2D p2 = project(points[(i + 1) % 4]);
            Point2D p3 = project(points[i + 4]);
            Point2D p4 = project(points[((i + 1) % 4) + 4]);

            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderDrawLine(renderer, p1.x, p1.y, p2.x, p2.y);
            SDL_RenderDrawLine(renderer, p3.x, p3.y, p4.x, p4.y);
            SDL_RenderDrawLine(renderer, p1.x, p1.y, p3.x, p3.y);
        }
    }
};

void RenderShape(Shape* shape) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    shape->Render(renderer);

    SDL_RenderPresent(renderer);
}

void UpdateShape(Shape* shape) {
    Uint32 currentTime = SDL_GetTicks(); // current time in ms
    float deltaTime = (currentTime - lastTime) / 1000.0f; // time passed since the last frame in seconds
    lastTime = currentTime;

    shape->Update(deltaTime);
}

void mainloop() {
    static Cube cube;
    UpdateShape(&cube);
    RenderShape(&cube);
}

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("3D Shape Demo", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, 0);

    emscripten_set_main_loop(mainloop, 0, 1);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}