#include <iostream>
#include <cmath>
#include <vector>
#include <memory>
#include <string>
#include <utility>
#include <fstream>
#include <chrono>
#include <thread>
#include <SDL.h>


class Vehicle;
class View;


class Model {
public:
    enum modelState {
        IDLE = 1,
        RUNNING = 2,
        FINISHED = 3
    };
    enum modelState state = IDLE;
    std::vector<std::unique_ptr<Vehicle>> vehicles;
    std::vector<std::unique_ptr<View>> views;
    float dt = 10;  // ms

    Model() = default;

    int addVehicle(std::unique_ptr<Vehicle> new_vehicle) {
        vehicles.push_back(std::move(new_vehicle));
        return vehicles.size() - 1;
    }

    void addView(std::unique_ptr<View> new_view) {
        views.push_back(std::move(new_view));
    }

    void start() {
        state = RUNNING;
    }
    
    void stop() {
        state = IDLE;
    }

    void exit() {
        state = FINISHED;
    }

    void display() const;

    void render(SDL_Renderer *renderer) const;

    void setVehicle( int id, float x, float y, float theta, float phi, float v, float a );

    void readConfig(char *config_file);

};


class Vehicle {
private:
    int id;
    float x, y;
    float w,  h; 
    float f, b, L;
    float theta, phi;
    float v, a;
    uint8_t red, green, blue, alpha;
    float dx, dy, dtheta;
    std::thread Thread;
    Model& M;

    inline void translatePoint(SDL_FPoint *p, SDL_FPoint *T) {
        p->x += T->x;
        p->y += T->y;
    }

    inline void rotatePoint(SDL_FPoint *p, float theta, SDL_FPoint *p0) {
        p->x -= p0->x;
        p->y -= p0->y;
        float cosTheta = std::cos(theta);
        float sinTheta = std::sin(theta);
        float xNew = p->x * cosTheta - p->y * sinTheta;
        float yNew = p->x * sinTheta + p->y * cosTheta;
        p->x = xNew + p0->x;
        p->y = yNew + p0->y;
    }

    inline void drawRectangle(SDL_Renderer *renderer, SDL_FPoint *p, float w, float h, float theta, int view_id) {
        SDL_FPoint p1,p2,p3,p4;
        float w2 = w/2, h2 = h/2;
        p1 = *p; p1.x -= w2; p1.y -= h2;
        p2 = *p; p2.x += w2; p2.y -= h2;
        p3 = *p; p3.x += w2; p3.y += h2;
        p4 = *p; p4.x -= w2; p4.y += h2;
        rotatePoint(&p1, theta, p);
        rotatePoint(&p2, theta, p);
        rotatePoint(&p3, theta, p);
        rotatePoint(&p4, theta, p);
        SDL_Vertex vertices[4] = {
            {{to_scr_x(p1.x,view_id), to_scr_y(p1.y,view_id)}, {0, 0, 0, 255}, {0, 0}},
            {{to_scr_x(p2.x,view_id), to_scr_y(p2.y,view_id)}, {0, 0, 0, 255}, {0, 0}},
            {{to_scr_x(p3.x,view_id), to_scr_y(p3.y,view_id)}, {0, 0, 0, 255}, {0, 0}},
            {{to_scr_x(p4.x,view_id), to_scr_y(p4.y,view_id)}, {0, 0, 0, 255}, {0, 0}}
        };
        int indices[6] = {
            0, 1, 2, 
            0, 2, 3  
        };
        SDL_RenderGeometry(renderer, NULL, vertices, 4, indices, 6);
    }

    inline void drawBody(SDL_Renderer *renderer, SDL_FPoint *p, float w, float h, float theta, int view_id) {
        SDL_FPoint p1,p2,p3,p4;
        float w2 = w/2, h2 = h/2;
        p1 = *p;
        p2 = *p;
        p3 = *p;
        p4 = *p;
        p1.x -= this->b;
        p1.y -= h2;
        p2.x += 2 * w2 - this->b;
        p2.y -= h2;
        p3.x += 2 * w2 - this->b;
        p3.y += h2;
        p4.x -= this->b;
        p4.y += h2;
        rotatePoint(&p1, theta, p);
        rotatePoint(&p2, theta, p);
        rotatePoint(&p3, theta, p);
        rotatePoint(&p4, theta, p);
        SDL_Vertex vertices[4] = {
            {{to_scr_x(p1.x,view_id), to_scr_y(p1.y,view_id)}, {this->red, this->green, this->blue, this->alpha}, {0, 0}},
            {{to_scr_x(p2.x,view_id), to_scr_y(p2.y,view_id)}, {this->red, this->green, this->blue, this->alpha}, {0, 0}},
            {{to_scr_x(p3.x,view_id), to_scr_y(p3.y,view_id)}, {this->red, this->green, this->blue, this->alpha}, {0, 0}},
            {{to_scr_x(p4.x,view_id), to_scr_y(p4.y,view_id)}, {this->red, this->green, this->blue, this->alpha}, {0, 0}}
        };
        int indices[6] = {
            0, 1, 2, 
            0, 2, 3  
        };
        SDL_RenderGeometry(renderer, NULL, vertices, 4, indices, 6);
    }

    inline int to_scr_x(float x, int view_id);
    inline int to_scr_y(float y, int view_id);
    inline int to_scr_d(float d, int view_id);

public:
    Vehicle() = default;

    Vehicle(
        float w, float h, 
        float f, float b,
        uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha,
        Model& mref) : w(w), h(h), f(f), b(b), red(red), green(green), blue(blue), alpha(alpha), M(mref), Thread() {
            id = M.vehicles.size();
            L = h - f - b;
            startTask();
        }

    ~Vehicle() {
        // Ensure thread is joined before destruction
        if (Thread.joinable()) {
            Thread.join();
        }
    }

    void set( float x, float y, float theta, float phi, float v, float a )
    {
        this->x = x;
        this->y = y;
        this->theta = theta*M_PI/180;
        this->phi = phi*M_PI/180;
        this->v = v/3.6;
        this->a = a;
    }

    void next_state()
    {
        // compute differences
        this->dx = this->v * std::cos(this->theta);
        this->dy = this->v * std::sin(this->theta);
        this->dtheta = this->v/this->L * std::tan(this->phi);

        // update state
        // Convert dt from milliseconds to seconds
        float dt_seconds = this->M.dt / 1000.0f;
        this->x = this->x + this->dx * dt_seconds;
        this->y = this->y + this->dy * dt_seconds;
        this->theta = this->theta + this->dtheta * dt_seconds;
        // this->v = this->v + this->a * dt_seconds;

        // std::cout 
        //     << "  " << this->M.dt 
        //     << "  " << this->v 
        // << std::endl;
    }

    void task(int id) {
        std::chrono::steady_clock::time_point start,end;
        long long elapsed;

        std::cout << "Vehicle " << id << " thread started." << std::endl;
        
        while (M.state != M.FINISHED) {

            start = std::chrono::steady_clock::now();

            if (M.state==M.RUNNING) {
                next_state();
            }

            do {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                end = std::chrono::steady_clock::now();
                elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
            } while ( elapsed < this->M.dt );

            // std::cout << "Milliseconds: " << elapsed << std::endl;
        }

        std::cout << "Vehicle " << id << " thread finished." << std::endl;
    }

    void startTask() {
        this->Thread = std::thread(&Vehicle::task, this, this->id);
    }

    void display(int view_id) const;

    void render(SDL_Renderer *renderer, int view_id);

    // // Control vehicle (1 level)
    // void set_throttle( float val );
    // void set_brake( float val );
    // void set_steer( float val );

    // // Control vehicle (2 level)
    // void set_speed( float val );


// /*
// point_rotate({X,Y},{Ox,Oy},Alfa) ->
//     { Ox + (X-Ox) * math:cos(Alfa) - (Y-Oy) * math:sin(Alfa),
//       Oy + (X-Ox) * math:sin(Alfa) + (Y-Oy) * math:cos(Alfa) }.
// */
// void point_rotate( float x, float y, float ox, float oy, float alfa, float *x1, float *y1 ) {
//   *x1 = ox + (x-ox) * cos(alfa) - (y-oy) * sin(alfa);
//   *y1 = oy + (x-ox) * sin(alfa) + (y-oy) * cos(alfa);
//   return;
// }


// /*
// vangle({X1,Y1},{X2,Y2}) ->  % angle between two vectors
//     Dot = X1*X2 + Y1*Y2,
//     Det = X1*Y2 - Y1*X2,
//     math:atan2(Det,Dot).
// */
// // Angle between two vectors
// float vec_angle( float alfa1, float alfa2 ) {
//   float x1,y1,x2,y2,dot,det;
//   x1 = cos(alfa1);
//   y1 = sin(alfa1);
//   x2 = cos(alfa2);
//   y2 = sin(alfa2);
//   dot = x1*x2 + y1*y2;
//   det = x1*y2 - y1*x2;
//   return atan2(det,dot);
// }

// float distance( float x1, float y1, float x2, float y2 ) {
//   return sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1));
// }



// void set_throttle( car_state_t *s, float val )   // 0..1
// {
//   if ( s->v >= 10.0/3.6 ) {
//     s->a = 0.0;
//     s->v = 10.0/3.6;
//   } else {
//     s->a = CAR_MAX_ACC * val;
//   }
//   timer_start(&api_timer);
//   return;
// }

// void set_brake( car_state_t *s, float val )   // 0..1
// {
//   if ( s->v <= 0.01 ) {
//     s->a = 0.0;
//     s->v = 0.0;
//   } else {
//     s->a = CAR_MAX_BR_ACC * val;
//   }
//   timer_start(&api_timer);
//   return;
// }
// void set_steer( car_state_t *s, float phi )   // -pi .. +pi
// {
//   if (phi > CAR_MAX_ST_ANGLE) {   // 145km/h
//     phi = CAR_MAX_ST_ANGLE;
//   } else if (phi < -CAR_MAX_ST_ANGLE) {
//     phi = -CAR_MAX_ST_ANGLE;
//   }
//   s->phi = phi;
//   timer_start(&api_timer);
//   return;
// }
// void set_speed( car_state_t *s, float v )   // -277.7778..+277.7778
// {
//   if (v > CAR_MAX_SPEED) {   // 145km/h
//     v = CAR_MAX_SPEED;
//   } else if (v < -CAR_MAX_SPEED) {
//     v = -CAR_MAX_SPEED;
//   }
//   s->v = v;
//   timer_start(&api_timer);
//   return;
// }




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


inline int Vehicle::to_scr_x(float x, int view_id) {
    std::unique_ptr<View>& V = M.views[view_id];
    return V->sx + (int)((x-V->x0)*V->scale);
}
inline int Vehicle::to_scr_y(float y, int view_id) {
    std::unique_ptr<View>& V = M.views[view_id];
    return V->sy + (int)((-y-V->y0)*V->scale);
}
inline int Vehicle::to_scr_d(float d, int view_id) {
    std::unique_ptr<View>& V = M.views[view_id];
    return (int)(d*V->scale);
}


void Vehicle::display(int view_id) const {
    std::unique_ptr<View>& V = M.views[view_id];

    // V->display();

    std::cout << "  Vehicle:  id=" << id 
                << "    x:" << x << " y:" << y << " w:" << w
                << "    theta:" << theta*180/M_PI << "   phi:" << phi*180/M_PI 
                << "    h:" << h << " r:" << unsigned(red) << " g:" << unsigned(green) << " b:" << unsigned(blue) << " a:" << unsigned(alpha) 
                << std::endl;
}


void Vehicle::render(SDL_Renderer *renderer, int view_id) {
    std::unique_ptr<View>& V = M.views[view_id];
    SDL_Rect rect;
    SDL_FPoint p1,p2,p3,T;

    // Color
    SDL_SetRenderDrawColor( renderer, red, green, blue, alpha );

    // Base
    p1 = { this->x, this->y };

    p2 = p1;
    T = {L,0};
    translatePoint( &p2, &T );
    rotatePoint( &p2, this->theta, &p1 );
    // SDL_RenderDrawLine(renderer, to_scr_x(p1.x,view_id), to_scr_y(p1.y,view_id), to_scr_x(p2.x,view_id), to_scr_y(p2.y,view_id));

    // Front

    // Front right wheel
    p3 = p2;
    T = {this->w/2,0};
    translatePoint( &p3, &T );
    rotatePoint( &p3, this->theta-M_PI/2, &p2 );
    // SDL_RenderDrawLine(renderer, to_scr_x(p2.x,view_id), to_scr_y(p2.y,view_id), to_scr_x(p3.x,view_id), to_scr_y(p3.y,view_id));

    drawRectangle( renderer, &p3, this->f*2, 0.20, this->theta+this->phi, view_id );

    // Front left wheel
    p3 = p2;
    T = {this->w/2,0};
    translatePoint( &p3, &T );
    rotatePoint( &p3, this->theta+M_PI/2, &p2 );
    // SDL_RenderDrawLine(renderer, to_scr_x(p2.x,view_id), to_scr_y(p2.y,view_id), to_scr_x(p3.x,view_id), to_scr_y(p3.y,view_id));

    drawRectangle( renderer, &p3, this->f*2, 0.20, this->theta+this->phi, view_id );


    // Back right wheel
    p2 = p1;
    T = {this->w/2,0};
    translatePoint( &p2, &T );
    rotatePoint( &p2, this->theta-M_PI/2, &p1 );
    // SDL_RenderDrawLine(renderer, to_scr_x(p1.x,view_id), to_scr_y(p1.y,view_id), to_scr_x(p2.x,view_id), to_scr_y(p2.y,view_id));

    drawRectangle( renderer, &p2, this->f*2, 0.20, this->theta, view_id );

    // Back left wheel
    p2 = p1;
    T = {this->w/2,0};
    translatePoint( &p2, &T );
    rotatePoint( &p2, this->theta+M_PI/2, &p1 );
    // SDL_RenderDrawLine(renderer, to_scr_x(p1.x,view_id), to_scr_y(p1.y,view_id), to_scr_x(p2.x,view_id), to_scr_y(p2.y,view_id));

    drawRectangle( renderer, &p2, this->f*2, 0.20, this->theta, view_id );


    p1 = {this->x, this->y};
    drawBody( renderer, &p1, this->h, this->w, this->theta, view_id );

    // p1 = {this->x, this->y};
    // drawRectangle( renderer, &p1, 0.2, 0.2, 0, view_id );
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

void Model::setVehicle( int id, float x, float y, float theta, float phi, float v, float a ) {
    vehicles[id]->set( x, y, theta, phi, v, a );
}

void Model::readConfig(char *config_file) {
    std::string block;
    int indent = 0;

    std::ifstream file(config_file); // Open the file for reading
    if (!file.is_open()) {
        std::cerr << "Failed to open the file." << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) { // Read the file line by line
        if (line.size()) {
            // Check if the line ends with a colon
            if (line.back() == ':') {
                // Print the line without the ending colon
                block = line.substr(0, line.size() - 1);
            }

            // Count spaces until the first non-space character
            size_t space_count = 0;
            while (space_count < line.size() && line[space_count] == ' ') {
                ++space_count;
            }

            // Check if the first non-space character is '-'
            bool is_dash = (space_count < line.size() && line[space_count] == '-');

            // Output the results
            std::cout << space_count << " " << is_dash << " " << block << "\t|";
            std::cout << line << std::endl;
        }
    }

    file.close(); // Close the file
    return;
}
