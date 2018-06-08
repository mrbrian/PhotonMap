#pragma once
#define _USE_MATH_DEFINES

#include "algebra.h"

using namespace std;

class photon
{
public:
    float x, y, z;              // position
    Color color;         // power packed as 4 chars
    double power;
    unsigned char phi, theta;   // compressed incident direction

    photon(){
        x = y = z = 0;
    }

    photon(Point3D p, Vector3D d, Color c, double energy){
        set_position(p);
        set_direction(d);
        set_color(c);
        power = energy;
    }

    void set_position(Point3D power);
    void set_direction(Vector3D dir);
    void set_color(Color dir);
    Color *get_color();
    Point3D get_position();
    Vector3D get_direction();

    float dist2(const photon &n)const{
       float _x = n.x - x;
       float _y = n.y - y;
       float _z = n.z - z;
       return _x*_x + _y*_y + _z*_z;
    }
};

