#include <iostream>
#include <cmath>
#include <vector>
#include <memory>
#include <string>
#include <utility>

#include <fstream>
#include <exception>
#include <yaml-cpp/yaml.h>


class Model {
private:
public:
    Model() = default;
};


class Vehicle {
private:
    float x, y, w, h;
    uint8_t r,g,b,a;
public:
    Vehicle() = default;

    Vehicle(float x, float y, float w, float h, uint8_t r, uint8_t g, uint8_t b, uint8_t a) : x(x), y(y), w(w), h(h), r(r), g(g), b(b), a(a) { }

    void display() const {
        std::cout << "Vehicle: " << "    x:" << x << " y:" << y << " w:" << w << " h:" << h << " r:" << unsigned(r) << " g:" << unsigned(g) << " b:" << unsigned(b) << " a:" << unsigned(a) << std::endl;
    }

    void render(SDL_Renderer *renderer) const {
        SDL_Rect rect;
        SDL_SetRenderDrawColor( renderer, r, g, b, a );		
        rect = { (int)x,(int)y,(int)w,(int)h };
        SDL_RenderDrawRect( renderer, &rect );
    }
};


class View {
private:
    float x, y, w, h;  // meters
    float scale;   // meters/px
    int scr_w, scr_h;
public:
    View() = default;
    View(float x, float y, float w, float h, int scr_w, int scr_h) : x(x), y(y), w(w), h(h), scr_w(scr_w), scr_h(scr_h) {

    }
};

