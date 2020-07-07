#include <iostream>
#include <numeric>

#include "fluid.hpp"
#include "renderer.hpp"

using namespace std;

Fluid::Fluid(){
    u0.resize(ARRAYSIZE(), 0.f);
    u1.resize(ARRAYSIZE(), 0.f);
    v0.resize(ARRAYSIZE(), 0.f);
    v1.resize(ARRAYSIZE(), 0.f);
    d0.resize(ARRAYSIZE(), 0.f);
    d1.resize(ARRAYSIZE(), 0.f);
    fx.resize(ARRAYSIZE(), 0.f);
    fy.resize(ARRAYSIZE(), 0.f);

    for(int x = 100; x < 200; ++x){
        for(int y = 100; y < 200; ++y){
            d0[LOC(x, y)] = 1000.f;
            // fx[LOC(x, y)] = 10.f;
        }
    }

    for(int x = 1; x < N-1; ++x){
        for(int y = 1; y < N-1; ++y){
            u1[LOC(x, y)] = 1;
            v1[LOC(x, y)] = 1;
        }
    }
}

void Fluid::simulate(){
    // velocity update
    vel_step();
    // density update
    den_step();
    draw(d1);
}

void Fluid::vel_step(){
    add_source(u1, fx, dt); add_source(v1, fy, dt);
    std::cout << "1" << std::endl;
    std::cout << u1[LOC(1, 1)] << std::endl;
    std::swap(u0, u1);
    std::swap(v0, v1);
    diffuse(1, u1, u0, visc, dt); 
    diffuse(2, v1, v0, visc, dt);
    std::cout << u1[LOC(1, 1)] << std::endl;
    std::cout << "2" << std::endl;
    project(u1, v1, u0, v0);
    std::cout << u1[LOC(1, 1)] << std::endl;
    std::cout << "3" << std::endl;
    std::swap(u0, u1);
    std::swap(v0, v1);
    advect(1, u1, u0, u0, v0, dt); 
    advect(2, v1, v0, u0, v0, dt);
    std::cout << "4" << std::endl;
    project(u1, v1, u0, v0);
    std::cout << "5" << std::endl;
}

void Fluid::den_step(){
    add_source(d1, d0, dt);
    std::swap(d0, d1); diffuse(0, d1, d0, df, dt);
    std::swap(d0, d1); advect(0, d1, d0, u1, v1, dt);
}

void Fluid::add_source(vector<float>& x, vector<float>& s, float dt){
    for(int i = 0; i < x.size(); ++i){
        x[i] += s[i]*dt;
    }
}

void Fluid::set_boundary(int boundary, vector<float>& s){
    for(int i = 1; i < N-1; ++i){
        s[LOC(0,  i)] = boundary == 1? -s[LOC(1, i)] : s[LOC(1, i)];
        s[LOC(N-1,i)] = boundary == 1? -s[LOC(N-1, i)] : s[LOC(N-1, i)];
        s[LOC(i,  0)] = boundary == 2? -s[LOC(i, 1)] : s[LOC(i, 1)];
        s[LOC(i,N-1)] = boundary == 2? -s[LOC(i, N-1)] : s[LOC(i, N-1)];
    }
    s[LOC(0, 0)] = 0.5 * (s[LOC(1, 0)]+s[LOC(0, 1)]);
    s[LOC(0, N-1)] = 0.5 * (s[LOC(1, N-1)] + s[LOC(0, N-2)]);
    s[LOC(N-1, 0)] = 0.5 * (s[LOC(N-2, 0)] + s[LOC(N-1, 1)]);
    s[LOC(N-1, N-1)] = 0.5 * (s[LOC(N-1, N-2)] + s[LOC(N-2, N-1)]);
}

void Fluid::diffuse(int boundary, vector<float>& s1, vector<float>& s0, float df, float dt){
    float alpha = dt * df * (N-2) * (N-2);
    for(int i = 0; i < 20; ++i){
        for(int x = 1; x < N-1; ++x){
            for(int y = 1; y < N-1; ++y){
                s1[LOC(x, y)] = (s0[LOC(x, y)]+alpha*(s1[LOC(x-1, y)] + s1[LOC(x+1, y)] + s1[LOC(x, y-1)] + s1[LOC(x, y+1)]))/(1+4*alpha);
            }
        }
        set_boundary(boundary, s1);
    }
}

void Fluid::advect(int boundary, std::vector<float>& s1, std::vector<float>& s0, std::vector<float>& u, std::vector<float>& v, float dt){
    int i0, i1, j0, j1;
    float rx1, rx0, ry0, ry1;
    float newx, newy;
    float dt0 = dt*(N-2);
    for(int x = 1; x < N-1; ++x){
        for(int y = 1; y < N-1; ++y){
            std::cout << x << " - " << y << std::endl;
            newx = x - dt0*u[LOC(x, y)]; newy = y - dt0*u[LOC(x, y)];
            std::cout << u[LOC(x, y)] << std::endl;
            std::cout << newx << " " << newy << endl;
            if(newx < 0.5) newx = 0.5; if(newx > N-1.5) newx = N - 1.5;
            if(newy < 0.5) newy = 0.5; if(newy > N-1.5) newy = N - 1.5;
            i0 = (int)newx; i1 = i0+1;
            j0 = (int)newy; j1 = j0+1;
            // std::cout<< i0 << " " << i1 << " " << j0 << " " << j1 << " " << N-1 << std::endl;
            rx1 = newx - i0; rx0 = 1 - rx1;
            ry1 = newy - j0; ry0 = 1 - ry1;
            s1[LOC(x, y)] = rx0*(ry0*s0[LOC(i0,j0)] + ry1*s0[LOC(i0, j1)]) +
                            rx1*(ry0*s0[LOC(i1,j0)] + ry1*s0[LOC(i1, j1)]);
        }
    }
    set_boundary(boundary, s1);
}

void Fluid::project(vector<float>& u, vector<float>& v, vector<float>& p, vector<float>& div){
    float h = 1.f/(N-2);
    for(int x = 1; x < N-1; ++x){
        for(int y = 1; y < N-1; ++y){
            div[LOC(x, y)] = -0.5*h*(u[LOC(x+1, y)]-u[LOC(x-1, y)] + v[LOC(x, y+1)]-v[LOC(x, y-1)]);
            p[LOC(x, y)] = 0;
        }
    }
    set_boundary(0, div); set_boundary(0, p);

    for(int i = 0; i < 20; ++i){
        for(int x = 1; x < N-1; ++x){
            for(int y = 1; y < N-1; ++y){
                p[LOC(x, y)] = (div[LOC(x, y)]+p[LOC(x-1, y)] + p[LOC(x+1, y)] + p[LOC(x, y-1)] + p[LOC(x, y+1)])/4;
            }
        }
        set_boundary(0, p);
    }

    for(int x = 1; x < N-1; ++x){
        for(int y = 1; y < N-1; ++y){
            u[LOC(x, y)] -= 0.5 * (p[LOC(x+1, y)] - p[LOC(x-1, y)])/h;
            v[LOC(x, y)] -= 0.5 * (p[LOC(x, y+1)] - p[LOC(x, y-1)])/h;
        }
    }
    set_boundary(1, u); set_boundary(2, v);
}