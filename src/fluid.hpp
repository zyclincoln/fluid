#ifndef _FLUID_HPP_
#define _FLUID_HPP_

#include <GL/gl.h>
#include <glm/glm.hpp>
#include <iostream>
#include <cstdlib>
#include <vector>

#include "define.hpp"

class Fluid{
public:
    Fluid();
    void simulate();
private:
    void den_step();
    void vel_step();
    void add_source(std::vector<double>& x, std::vector<double>& s, double dt);
    void diffuse(int boundary, std::vector<double>& s1, std::vector<double>& s0, double df, double dt);
    void project(std::vector<double>& u, std::vector<double>& v, std::vector<double>& p, std::vector<double>& div);
    void advect(int boundary, std::vector<double>& s1, std::vector<double>& s0, std::vector<double>& u, std::vector<double>& v, double dt);
    void set_boundary(int boundary, std::vector<double>& s);
    void dissipate(std::vector<double>& t, double dissipate);
private:
    double dt = 0.05;
    double visc = 0.0001;
    double df = 0.0001;
    double dissi = 0.005;
    // old velocity field x
    std::vector<double> u0;
    // new velocity field x
    std::vector<double> u1;
    // old velocity field y
    std::vector<double> v0;
    // new velocity field y
    std::vector<double> v1;
    // force field x
    std::vector<double> fx;
    // force field y
    std::vector<double> fy;
    // old density field
    std::vector<double> d0;
    // new density field
    std::vector<double> d1;
};

#endif