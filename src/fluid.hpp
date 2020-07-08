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
    void add_source(std::vector<float>& x, std::vector<float>& s, float dt);
    void diffuse(int boundary, std::vector<float>& s1, std::vector<float>& s0, float df, float dt);
    void project(std::vector<float>& u, std::vector<float>& v, std::vector<float>& p, std::vector<float>& div);
    void advect(int boundary, std::vector<float>& s1, std::vector<float>& s0, std::vector<float>& u, std::vector<float>& v, float dt);
    void set_boundary(int boundary, std::vector<float>& s);
    void dissipate(std::vector<float>& t, float dissipate);
private:
    float dt = 0.05;
    float visc = 0.001;
    float df = 0.001;
    float dissi = 0.001;
    // old velocity field x
    std::vector<float> u0;
    // new velocity field x
    std::vector<float> u1;
    // old velocity field y
    std::vector<float> v0;
    // new velocity field y
    std::vector<float> v1;
    // force field x
    std::vector<float> fx;
    // force field y
    std::vector<float> fy;
    // old density field
    std::vector<float> d0;
    // new density field
    std::vector<float> d1;
};

#endif