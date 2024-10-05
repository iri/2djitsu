#include <iostream>
#include <cmath>
#include <vector>
#include <memory>
#include <string>
#include <utility>

#include <fstream>
// #include <exception>


class Vehicle;
class View;


class Model {
    enum modelState {
        IDLE = 1,
        STARTED = 2
    };
    enum modelState state = IDLE;
public:
    std::vector<std::unique_ptr<Vehicle>> vehicles;
    std::vector<std::unique_ptr<View>> views;

    Model() = default;

    void addVehicle(std::unique_ptr<Vehicle> new_vehicle) {
        vehicles.push_back(std::move(new_vehicle));
    }

    void addView(std::unique_ptr<View> new_view) {
        views.push_back(std::move(new_view));
    }

    void start() {
        state = STARTED;
    }
    
    void stop() {
        state = IDLE;
    }
};


class Vehicle {
private:
    float x, y, w, h;
    uint8_t r,g,b,a;
    Model& M;
public:
    Vehicle() = default;

    Vehicle(float x, float y, float w, float h, uint8_t r, uint8_t g, uint8_t b, uint8_t a, Model& mref) : x(x), y(y), w(w), h(h), r(r), g(g), b(b), a(a), M(mref) { }

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
    Model& M;
public:
    View() = default;
    View(float x, float y, float w, float h, int scr_w, int scr_h, Model& mref) : x(x), y(y), w(w), h(h), scr_w(scr_w), scr_h(scr_h), M(mref) { }

    void render(SDL_Renderer *renderer) const {
        for (const auto& vehicle : M.vehicles) {
            vehicle->render(renderer);
        }
    }
};


