#include <iostream>
#include <numeric>
#include <iomanip>
#include "util.hpp"
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
}

void Fluid::simulate(){
    // velocity update
    vel_step();
    // density update
    den_step();
    draw(d1);
    // draw(u1, v1);
}

void Fluid::vel_step(){
    add_source(u1, fx, dt); 
    add_source(v1, fy, dt);
    vorticity_confinement(u1, v1, epsilon);
    std::swap(u0, u1);
    std::swap(v0, v1);
    diffuse(1, u1, u0, visc, dt);
    diffuse(2, v1, v0, visc, dt);
    std::swap(u0, u1);
    std::swap(v0, v1);
    advect(1, u1, u0, u0, v0, dt); 
    advect(2, v1, v0, u0, v0, dt);
    project(u1, v1, u0, v0);
    u0 = vector<double>(ARRAYSIZE(), 0);
    v0 = vector<double>(ARRAYSIZE(), 0);
}

void Fluid::add_source(){
    for(int x = 60; x < 70; ++x){
        for(int y = 100; y < 110; ++y){
            fx[LOC(x, y)] = 0.f;
            fy[LOC(x, y)] = -0.2f;
            d0[LOC(x, y)] = 0.8f;
        }
    }
}

void Fluid::den_step(){
    add_source(d1, d0, dt);
    swap(d0, d1);
    diffuse(0, d1, d0, df, dt);
    swap(d0, d1);
    advect(0, d1, d0, u1, v1, dt);
    dissipate(d1, dissi);
    d0 = vector<double>(ARRAYSIZE(), 0);
}

void Fluid::vorticity_confinement(std::vector<double>& u, std::vector<double>& v, double epsilon){
    double delta = 1.0/(N-1);
    vector<double> w(u.size(), 0);
    for(int i = 1; i < N-1; ++i){
        for(int j = 1; j < N-1; ++j){
            w[LOC(i, j)] = ((v[LOC(i+1, j)] - v[LOC(i-1, j)]) 
            - (u[LOC(i, j+1)]-u[LOC(i, j-1)])) / (2*delta); 
        }
    }

    vector<double> eta_x(u.size(), 0), eta_y(u.size(), 0);
    for(int i = 1; i < N-1; ++i){
        for(int j = 1; j < N-1; ++j){
            eta_x[LOC(i, j)] = (fabs(w[LOC(i+1, j)]) - fabs(w[LOC(i-1, j)])) * 0.5 / delta;
            eta_y[LOC(i, j)] = (fabs(w[LOC(i, j+1)]) - fabs(w[LOC(i, j-1)])) * 0.5 / delta;
            double norm = sqrt(eta_x[LOC(i, j)]*eta_x[LOC(i, j)] + eta_y[LOC(i, j)]*eta_y[LOC(i, j)]);
            if(norm < 1e-12){
                eta_x[LOC(i, j)] = 0;
                eta_y[LOC(i, j)] = 0;
            }
            else{
                eta_x[LOC(i, j)] /= norm;
                eta_y[LOC(i, j)] /= norm;
            }
        }
    }

    // add force to u
    for(int i = 1; i < N-1; ++i){
        for(int j = 1; j < N-1; ++j){
            u1[LOC(i, j)] += epsilon * delta * eta_y[LOC(i, j)] * w[LOC(i, j)];
            v1[LOC(i, j)] += -epsilon * delta * eta_x[LOC(i, j)] * w[LOC(i, j)];
        }
    }
}

void Fluid::add_source(vector<double>& x, vector<double>& s, double dt){
    for(int i = 0; i < x.size(); ++i){
        x[i] += s[i]*dt;
    }
}

void Fluid::dissipate(vector<double>& t, double a){
    double s = dt * a;
    for(int x = 0; x < N-1; ++x){
        for(int y = 0; y < N-1; ++y){
            t[LOC(x, y)] = t[LOC(x, y)]/(1+s);
        }
    }
}

void Fluid::set_boundary(int boundary, vector<double>& s){
    if(boundary == 1){
        for(int i = 1; i < N-1; ++i){
            s[LOC(0,  i)] = -s[LOC(1, i)];
            s[LOC(N-1,i)] = -s[LOC(N-2, i)];
            s[LOC(i,  0)] = s[LOC(i, 1)];
            s[LOC(i,N-1)] = s[LOC(i, N-2)];
        }
        s[LOC(0, 0)] = 0.5 * (s[LOC(1, 0)]+s[LOC(0, 1)]);
        s[LOC(0, N-1)] = 0.5 * (s[LOC(1, N-1)] + s[LOC(0, N-2)]);
        s[LOC(N-1, 0)] = 0.5 * (s[LOC(N-2, 0)] + s[LOC(N-1, 1)]);
        s[LOC(N-1, N-1)] = 0.5 * (s[LOC(N-1, N-2)] + s[LOC(N-2, N-1)]);
    }
    else if(boundary == 2){
        for(int i = 1; i < N-1; ++i){
            s[LOC(0,  i)] = s[LOC(1, i)];
            s[LOC(N-1,i)] = s[LOC(N-2, i)];
            s[LOC(i,  0)] = -s[LOC(i, 1)];
            s[LOC(i,N-1)] = -s[LOC(i, N-2)];
        }
        s[LOC(0, 0)] = 0.5 * (s[LOC(1, 0)]+s[LOC(0, 1)]);
        s[LOC(0, N-1)] = 0.5 * (s[LOC(1, N-1)] + s[LOC(0, N-2)]);
        s[LOC(N-1, 0)] = 0.5 * (s[LOC(N-2, 0)] + s[LOC(N-1, 1)]);
        s[LOC(N-1, N-1)] = 0.5 * (s[LOC(N-1, N-2)] + s[LOC(N-2, N-1)]);
    }
    else if(boundary == 3){
        for(int i = 1; i < N-1; ++i){
            s[LOC(0,  i)] = s[LOC(1, i)];
            s[LOC(N-1,i)] = s[LOC(N-2, i)];
            s[LOC(i,  0)] = s[LOC(i, 1)];
            s[LOC(i,N-1)] = s[LOC(i, N-2)];
        }
        s[LOC(0, 0)] = 0.5 * (s[LOC(1, 0)]+s[LOC(0, 1)]);
        s[LOC(0, N-1)] = 0.5 * (s[LOC(1, N-1)] + s[LOC(0, N-2)]);
        s[LOC(N-1, 0)] = 0.5 * (s[LOC(N-2, 0)] + s[LOC(N-1, 1)]);
        s[LOC(N-1, N-1)] = 0.5 * (s[LOC(N-1, N-2)] + s[LOC(N-2, N-1)]);
    }
    else if(boundary == 0){
        for(int i = 1; i < N-1; ++i){
            // if(s[LOC(0, i)] != 0.f){
            //     swap(s[LOC(0, i)], s[LOC(1, i)]);
            // }
            // else{
                s[LOC(0, i)] = s[LOC(1, i)];
            // }
            
            // if(s[LOC(N-1, i)] != 0.f){
            //     swap(s[LOC(N-1, i)], s[LOC(N-2, i)]);            
            // }
            // else{
                s[LOC(N-1, i)] = s[LOC(N-2, i)];
            // }

            // if(s[LOC(i, 0)] != 0.f){
            //     swap(s[LOC(i, 0)], s[LOC(i, 1)]);
            // }
            // else{
                s[LOC(i, 0)] = s[LOC(i, 1)];
            // }

            // if(s[LOC(i, N-1)] != 0.f){
            //     swap(s[LOC(i, N-2)], s[LOC(i, N-1)]);            
            // }
            // else{
                s[LOC(i, N-1)] = s[LOC(i, N-2)];
            // }
        }
        s[LOC(0, 0)] = 0.5 * (s[LOC(1, 0)]+s[LOC(0, 1)]);
        s[LOC(0, N-1)] = 0.5 * (s[LOC(1, N-1)] + s[LOC(0, N-2)]);
        s[LOC(N-1, 0)] = 0.5 * (s[LOC(N-2, 0)] + s[LOC(N-1, 1)]);
        s[LOC(N-1, N-1)] = 0.5 * (s[LOC(N-1, N-2)] + s[LOC(N-2, N-1)]);
    }
}

void Fluid::diffuse(int boundary, vector<double>& s1, vector<double>& s0, double df, double dt){
    double alpha = dt * df * N * N;
    for(int i = 0; i < 30; ++i){
        for(int x = 1; x < N-1; ++x){
            for(int y = 1; y < N-1; ++y){
                s1[LOC(x, y)] = (s0[LOC(x, y)]+alpha*(s1[LOC(x-1, y)] + s1[LOC(x+1, y)] + s1[LOC(x, y-1)] + s1[LOC(x, y+1)]))/(1+4*alpha);     
            }
        }
        set_boundary(boundary, s1);
    }
}

void Fluid::advect(int boundary, std::vector<double>& s1, std::vector<double>& s0, std::vector<double>& u, std::vector<double>& v, double dt){
    int i0, i1, j0, j1;
    double rx1, rx0, ry0, ry1;
    double newx, newy;
    dt = dt * N;
    for(int x = 1; x < N-1; ++x){
        for(int y = 1; y < N-1; ++y){
            newx = x - dt*u[LOC(x, y)]; newy = y - dt*v[LOC(x, y)];
            if(newx < 0.5) newx = 0.5; if(newx > N-1.5) newx = N - 1.5;
            if(newy < 0.5) newy = 0.5; if(newy > N-1.5) newy = N - 1.5;
            i0 = (int)newx; i1 = i0+1;
            j0 = (int)newy; j1 = j0+1;
            rx1 = newx - i0; rx0 = 1 - rx1;
            ry1 = newy - j0; ry0 = 1 - ry1;
            
            s1[LOC(x, y)] = rx0*(ry0*s0[LOC(i0,j0)] + ry1*s0[LOC(i0, j1)]) +
                            rx1*(ry0*s0[LOC(i1,j0)] + ry1*s0[LOC(i1, j1)]);
        }
    }
    set_boundary(boundary, s1);
}

void Fluid::project(vector<double>& u, vector<double>& v, vector<double>& p, vector<double>& div){
    double h = 1.f/(N-2);
    p = vector<double>(ARRAYSIZE(), 0.f);
    div = vector<double>(ARRAYSIZE(), 0.f);
    for(int x = 1; x < N-1; ++x){
        for(int y = 1; y < N-1; ++y){
            div[LOC(x, y)] = -0.5*h*(u[LOC(x+1, y)]-u[LOC(x-1, y)] + v[LOC(x, y+1)]-v[LOC(x, y-1)]);
        }
    }
    set_boundary(3, div); set_boundary(3, p);

    for(int i = 0; i < 30; ++i){
        for(int x = 1; x < N-1; ++x){
            for(int y = 1; y < N-1; ++y){
                p[LOC(x, y)] = (div[LOC(x, y)]+p[LOC(x-1, y)] + p[LOC(x+1, y)] + p[LOC(x, y-1)] + p[LOC(x, y+1)])/4;
            }
        }
        set_boundary(3, p);
    }

    for(int x = 1; x < N-1; ++x){
        for(int y = 1; y < N-1; ++y){
            u[LOC(x, y)] -= 0.5 * (p[LOC(x+1, y)] - p[LOC(x-1, y)])/h;
            v[LOC(x, y)] -= 0.5 * (p[LOC(x, y+1)] - p[LOC(x, y-1)])/h;
        }
    }
}