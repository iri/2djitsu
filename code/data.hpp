#include <iostream>
#include <cmath>
#include <vector>
#include <memory>
#include <string>
#include <utility>

#include <fstream>


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

    void display() const;

    void render(SDL_Renderer *renderer) const;
};


class Vehicle {
private:
    int id;
    float x, y, w, h;
    uint8_t r,g,b,a;
    Model& M;
public:
    Vehicle() = default;

    Vehicle(float x, float y, float w, float h, uint8_t r, uint8_t g, uint8_t b, uint8_t a, Model& mref) : x(x), y(y), w(w), h(h), r(r), g(g), b(b), a(a), M(mref) { 
        id = M.vehicles.size();
    }

    void display(int view_id) const;

    void render(SDL_Renderer *renderer, int view_id) const;
};


class View {
public:
    int id;
    float x, y;   // center (in meters)
    float w, h;     // width height (in meters)
    float x0, y0; // left top (in meters)
    int scr_w, scr_h;  // screen width height (in px)
    int sx, sy;   // left top (in px)
    float scale;          // px per meter
    Model& M;

    View() = default;

    View(float x, float y, float w, float h, int scr_w, int scr_h, Model& mref) : x(x), y(y), w(w), h(h), scr_w(scr_w), scr_h(scr_h), M(mref) {
        id = M.views.size();

        scale = scr_w/w;
        if (scr_h/h < scale) {
            scale = scr_h/h;
        }

        x0 = x-w/2;
        y0 = y-h/2;

        sx = scr_w/2 - w/2*scale;
        sy = scr_h/2 - h/2*scale;

        std::cout << sx << " " << sy << std::endl;

    }

    void display() const {
        std::cout << "View:  id=" << id << "    x:" << x << " y:" << y << " w:" << w << " h:" << h << std::endl;
    }

    void test() const {
        display();
        for (int i=0; i < M.vehicles.size(); i++ ) {
            M.vehicles[i]->display(id);
        }
    }

    void render(SDL_Renderer *renderer) const {
        SDL_Rect rect;
        SDL_SetRenderDrawColor( renderer, 200, 200, 200, 255 );
        rect = {    sx,
                    sy,
                    (int)(w*scale),
                    (int)(h*scale)   };
        SDL_RenderDrawRect( renderer, &rect );

        for (int i=0; i < M.vehicles.size(); i++ ) {
            M.vehicles[i]->render(renderer, id);
        }

    }
};


void Vehicle::display(int view_id) const {
    std::unique_ptr<View>& V = M.views[view_id];

    std::cout
        << V->sx + (int)((x-V->x0)*V->scale) << " "
        << V->sy + (int)((y-V->y0)*V->scale) << " "
        << (int)(w*V->scale) << " "
        << (int)(h*V->scale) << " "
        << std::endl;

    V->display();
    std::cout << "  Vehicle:  id=" << id 
                << "    x:" << x << " y:" << y << " w:" << w << " h:" 
                << h << " r:" << unsigned(r) << " g:" << unsigned(g) << " b:" << unsigned(b) << " a:" << unsigned(a) 
                << std::endl;
}

void Vehicle::render(SDL_Renderer *renderer, int view_id) const {
    std::unique_ptr<View>& V = M.views[view_id];
    SDL_Rect rect;
    SDL_SetRenderDrawColor( renderer, r, g, b, a );
    rect = {    V->sx + (int)((x-V->x0)*V->scale),
                V->sy + (int)((y-V->y0)*V->scale),
                (int)(w*V->scale),
                (int)(h*V->scale)   };
    SDL_RenderDrawRect( renderer, &rect );
}



void Model::display() const {
    if (views.size() > 0) {
        views[0]->test();
    }
}

void Model::render(SDL_Renderer *renderer) const {
    if (views.size() > 0) {
        views[0]->render(renderer);
    }
}

