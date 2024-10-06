#include <iostream>
#include <cmath>
#include <vector>
#include <memory>
#include <string>
#include <utility>
#include <cmath>
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

    int addVehicle(std::unique_ptr<Vehicle> new_vehicle) {
        vehicles.push_back(std::move(new_vehicle));
        return vehicles.size() - 1;
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

    void Model::setVehicle( int id, float x, float y, float theta, float phi, float v, float a );

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
    float t, dt;
    Model& M;

    float _x,_y, _xf,_yf;
    float _xfr,_yfr;
    float _xfl,_yfl;
    float _xr,_yr;
    float _xl,_yl;

    inline void rotatePoint(float& x, float& y, float theta, float cx, float cy) {
        x -= cx;
        y -= cy;
        float cosTheta = std::cos(theta);
        float sinTheta = std::sin(theta);
        float xNew = x * cosTheta - y * sinTheta;
        float yNew = x * sinTheta + y * cosTheta;
        x = xNew + cx;
        y = yNew + cy;
    }

    inline void translatePoint(float& x, float& y, float dx, float dy) {
        x += dx;
        y += dy;
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
        Model& mref) : w(w), h(h), f(f), b(b), red(red), green(green), blue(blue), alpha(alpha), M(mref) {
            id = M.vehicles.size();
            L = h - f - b;
        }

    void set( float x, float y, float theta, float phi, float v, float a )
    {
        this->x = x;
        this->y = y;
        this->theta = theta*M_PI/180;
        this->phi = phi*M_PI/180;
        this->v = v;
        this->a = a;
    }

    // int car_nextstate( float phi, float v )
    // {
    //     v = v / 3.6;
    //     phi = phi / 180.0 * 3.1415926;
    //     dx = v * cos(theta);
    //     dy = v * sin(theta);
    //     dtheta = v/L * tan(phi);
    //     x = x + dx*dt;
    //     y = y + dy*dt;
    //     theta = theta + dtheta*dt;
    //     t = t + dt;
    //     v = v;
    //     return 0;
    // }

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

    V->display();

    std::cout << "  Vehicle:  id=" << id 
                << "    x:" << x << " y:" << y << " w:" << w
                << "    theta:" << theta*180/M_PI << "   phi:" << phi*180/M_PI 
                << "    h:" << h << " r:" << unsigned(red) << " g:" << unsigned(green) << " b:" << unsigned(blue) << " a:" << unsigned(alpha) 
                << std::endl;
}

void Vehicle::render(SDL_Renderer *renderer, int view_id) {
    std::unique_ptr<View>& V = M.views[view_id];
    SDL_Rect rect;
    SDL_SetRenderDrawColor( renderer, red, green, blue, alpha );
    rect = { to_scr_x(x,view_id), to_scr_y(y,view_id), to_scr_d(w,view_id), to_scr_d(h,view_id) };
    SDL_RenderDrawRect( renderer, &rect );

    // Base
    _x = this->x;
    _y = this->y;
    _xf = _x;
    _yf = _y;
    translatePoint( _xf, _yf, L, 0 );
    rotatePoint( _xf, _yf, this->theta, _x, _y );
    SDL_RenderDrawLine(renderer, to_scr_x(_x,view_id), to_scr_y(_y,view_id), to_scr_x(_xf,view_id), to_scr_y(_yf,view_id));

    // Front


    // Front right
    _xfr = _xf;
    _yfr = _yf;
    translatePoint( _xfr, _yfr, this->w/2, 0 );
    rotatePoint( _xfr, _yfr, this->theta-M_PI/2, _xf, _yf );
    SDL_RenderDrawLine(renderer, to_scr_x(_xf,view_id), to_scr_y(_yf,view_id), to_scr_x(_xfr,view_id), to_scr_y(_yfr,view_id));

    // Front right wheel


    // Front left
    _xfl = _xf;
    _yfl = _yf;
    translatePoint( _xfl, _yfl, this->w/2, 0 );
    rotatePoint( _xfl, _yfl, this->theta+M_PI/2, _xf, _yf );
    SDL_RenderDrawLine(renderer, to_scr_x(_xf,view_id), to_scr_y(_yf,view_id), to_scr_x(_xfl,view_id), to_scr_y(_yfl,view_id));

    // Back
    
    // Back right
    _xr = _x;
    _yr = _y;
    translatePoint( _xr, _yr, this->w/2, 0 );
    rotatePoint( _xr, _yr, this->theta-M_PI/2, _x, _y );
    SDL_RenderDrawLine(renderer, to_scr_x(_x,view_id), to_scr_y(_y,view_id), to_scr_x(_xr,view_id), to_scr_y(_yr,view_id));

    // Back left
    _xl = _x;
    _yl = _y;
    translatePoint( _xl, _yl, this->w/2, 0 );
    rotatePoint( _xl, _yl, this->theta+M_PI/2, _x, _y );
    SDL_RenderDrawLine(renderer, to_scr_x(_x,view_id), to_scr_y(_y,view_id), to_scr_x(_xl,view_id), to_scr_y(_yl,view_id));
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


