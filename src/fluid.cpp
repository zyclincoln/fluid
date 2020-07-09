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

    for(int x = 61; x < 69; ++x){
        for(int y = N-11; y < N; ++y){
            d0[LOC(x, y)] = 100.f;
        }
    }

    for(int x = 61; x < 69; ++x){
        for(int y = N-11; y < N; ++y){
            u0[LOC(x, y)] = 0;
            v0[LOC(x, y)] = -1000;
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
    add_source(u1, u0, dt); add_source(v1, v0, dt);
    std::cout << "u1 ";
    check_horizontal_symmetric(u1);
    std::cout << "v1 ";
    check_horizontal_symmetric(v1);
    std::swap(u0, u1);
    std::swap(v0, v1);
    diffuse(1, u1, u0, visc, dt); 
    diffuse(2, v1, v0, visc, dt);
    // horizontal_symmetrify(v1);
    std::cout << "after diffuse u1 ";
    check_horizontal_symmetric(u1);
    std::cout << "after diffuse v1 ";
    check_horizontal_symmetric(v1);
    // project(u1, v1, u0, v0);
    std::swap(u0, u1);
    std::swap(v0, v1);
    advect(1, u1, u0, u0, v0, dt); 
    advect(2, v1, v0, u0, v0, dt);
    project(u1, v1, u0, v0);
    u0 = vector<double>(ARRAYSIZE(), 0.f);
    v0 = vector<double>(ARRAYSIZE(), 0.f);
}

void Fluid::den_step(){
    add_source(d1, d0, dt);
    std::swap(d0, d1); diffuse(0, d1, d0, df, dt);
    std::swap(d0, d1); advect(0, d1, d0, u1, v1, dt);
    dissipate(d1, dissi);
    d0 = vector<double>(ARRAYSIZE(), 0.f);
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
        // for(int i = 1; i < N-1; ++i){
        //     s[LOC(0,  i)] = s[LOC(1, i)];
        //     s[LOC(N-1,i)] = s[LOC(N-2, i)];
        //     s[LOC(i,  0)] = s[LOC(i, 1)];
        //     s[LOC(i,N-1)] = s[LOC(i, N-2)];
        // }
        for(int i = 1; i < N-1; ++i){
            if(s[LOC(0, i)] != 0.f){
                swap(s[LOC(0, i)], s[LOC(1, i)]);
            }
            else{
                s[LOC(0, i)] = s[LOC(1, i)];
            }
            
            if(s[LOC(N-1, i)] != 0.f){
                swap(s[LOC(N-1, i)], s[LOC(N-2, i)]);            
            }
            else{
                s[LOC(N-1, i)] = s[LOC(N-2, i)];
            }

            if(s[LOC(i, 0)] != 0.f){
                swap(s[LOC(i, 0)], s[LOC(i, 1)]);
            }
            else{
                s[LOC(i, 0)] = s[LOC(i, 1)];
            }

            if(s[LOC(i, N-1)] != 0.f){
                swap(s[LOC(i, N-2)], s[LOC(i, N-1)]);            
            }
            else{
                s[LOC(i, N-1)] = s[LOC(i, N-2)];
            }
        }
        // {
        //     double temp = s[LOC(1, 1)];
        //     s[LOC(1, 1)] += s[LOC(0, 1)] + s[LOC(1, 0)];
        //     s[LOC(1, 0)] = s[LOC(0, 1)] = 0.5*temp;
        // }
        // {
        //     double temp = s[LOC(1, N-2)];
        //     s[LOC(1, N-2)] += s[LOC(0, N-2)] + s[LOC(1, N-1)];
        //     s[LOC(0, N-2)] = s[LOC(1, N-1)] = 0.5*temp;
        // }
        // {
        //     double temp = s[LOC(N-2, 1)];
        //     s[LOC(N-2, 1)] += s[LOC(N-1, 1)] + s[LOC(N-2, 0)];
        //     s[LOC(N-1, 1)] = s[LOC(N-2, 0)] = 0.5*temp;
        // }
        // {
        //     double temp = s[LOC(N-2, N-2)];
        //     s[LOC(N-2, N-2)] += s[LOC(N-2, N-1)] + s[LOC(N-1, N-2)];
        //     s[LOC(N-2, N-1)] = s[LOC(N-1, N-2)] = 0.5*temp;
        // }
        s[LOC(0, 0)] = 0.5 * (s[LOC(1, 0)]+s[LOC(0, 1)]);
        s[LOC(0, N-1)] = 0.5 * (s[LOC(1, N-1)] + s[LOC(0, N-2)]);
        s[LOC(N-1, 0)] = 0.5 * (s[LOC(N-2, 0)] + s[LOC(N-1, 1)]);
        s[LOC(N-1, N-1)] = 0.5 * (s[LOC(N-1, N-2)] + s[LOC(N-2, N-1)]);
    }
}

void Fluid::diffuse(int boundary, vector<double>& s1, vector<double>& s0, double df, double dt){
    double alpha = dt * df * N * N;
    for(int i = 0; i < 20; ++i){
        for(int x = 1; x < N-1; ++x){
            for(int y = 1; y < N-1; ++y){
                s1[LOC(x, y)] = (s0[LOC(x, y)]+alpha*(s1[LOC(x-1, y)] + s1[LOC(x+1, y)] + s1[LOC(x, y-1)] + s1[LOC(x, y+1)]))/(1+4*alpha);     
            }
        }
        // std::cout << "i: " << i << std::endl;
        // check_horizontal_symmetric(s1);
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
            // std::cout<< i0 << " " << i1 << " " << j0 << " " << j1 << " " << N-1 << std::endl;
            rx1 = newx - i0; rx0 = 1 - rx1;
            ry1 = newy - j0; ry0 = 1 - ry1;
            // std::cout << rx0 << " " << rx1 << " " << ry0 << " " << ry1 << std::endl;
            // std::cout << i0 << " " << i1 << " " << j0 << " " << j1 << std::endl;
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
            // p[LOC(x, y)] = 0;
        }
    }
    set_boundary(3, div); set_boundary(3, p);
    std::cout << "h: " << h << std::endl;
    std::cout << "div: ";
    check_horizontal_symmetric(div);

    for(int i = 0; i < 20; ++i){
        for(int x = 1; x < N-1; ++x){
            for(int y = 1; y < N-1; ++y){
                p[LOC(x, y)] = (div[LOC(x, y)]+p[LOC(x-1, y)] + p[LOC(x+1, y)] + p[LOC(x, y-1)] + p[LOC(x, y+1)])/4;
            }
        }
        set_boundary(3, p);
    }
    std::cout << "p: ";
    // horizontal_symmetrify(p);
    check_horizontal_symmetric(p);
    std::cout << "u: ";
    check_horizontal_symmetric(u);
    std::cout << "v: ";
    check_horizontal_symmetric(v);

    for(int x = 1; x < N-1; ++x){
        for(int y = 1; y < N-1; ++y){
            u[LOC(x, y)] -= 0.5 * (p[LOC(x+1, y)] - p[LOC(x-1, y)])/h;
            v[LOC(x, y)] -= 0.5 * (p[LOC(x, y+1)] - p[LOC(x, y-1)])/h;
        }
    }
    // std::cout << p[LOC(66, 3)] << std::endl;
    // draw(p);
    std::cout << "after project u: ";
    check_horizontal_symmetric(u);
    std::cout << "after project v: ";
    check_horizontal_symmetric(v);
    set_boundary(1, u); set_boundary(2, v);
}