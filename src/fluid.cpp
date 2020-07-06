#include "fluid.hpp"

using namespace std;

void Fluid::draw(){
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glLoadIdentity();
    glOrtho(0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, -1.0, 1.0);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for(int y = 0; y < N; ++y){
        for(int x = 0; x < N; ++x){
            glColor4d(1.f, 1.f, 1.f, d1[LOC(x, y)]);
            glRectf(x*WINDOW_WIDTH*1.f/N, y*WINDOW_HEIGHT*1.f/N, 
                    (x+1)*WINDOW_WIDTH*1.f/N, (y+1)*WINDOW_HEIGHT*1.f/N);
        }
    }
}

void Fluid::simulate(){
    // velocity update
    vel_step();
    // density update
    den_step();
}

void Fluid::vel_steup(){
    add_source(u1, u0, dt); add_source(v1, v0, dt);
    std::swap(u0, u1);
    std::swap(v0, v1);
    diffuse(1, u1, u0, visc, dt); 
    diffuse(2, v1, v0, visc, dt);
    project(u, v, u0, v0);
    std::swap(u0, u1);
    std::swap(v0, v1);
    advect(1, u1, u0, u0, v0, dt); 
    advect(2, v1, v0, u0, v0, dt);
    project(u, v, u0, v0);
}

void Fluid::den_step(){
    add_source(d1, d0, dt);
    std::swap(d0, d1); diffuse(0, d1, d0, df, dt);
    std::swap(d0, d1); advert(0, d1, d0, u, v, dt);
}

void Fluid::add_source(vector<float>& x, vector<float>& s, float dt){
    for(int i = 0; i < x.size(); ++i){
        x[i] += s[i]*dt;
    }
}

void Fluid::set_boundary(int boundary, vector<float>& s){
    for(int i = 1; i < N-2; ++i){
        // s[LOC(0,i)] = boundary == 1? 
    }
}

void Fluid::diffuse(int boundary, vector<float>& s1, vector<float>& s0, float df, float dt){
    float alpha = dt * df * N * N;
    for(int i = 0; i < 20; ++i){
        for(int x = 1; x < N-1; ++x){
            for(int y = 1; y < N-1; ++y){
                s1[LOC(x, y)] = (s0[LOC(x, y)]+alpha*(s1[LOC(x-1, y)] + s1[LOC(x+1, y)] + s1[LOC(x, y-1)] + s1[LOC(x, y+1)]))/(1+4*a);
            }
        }
        set_boundary(boundary, s1);
    }
}

void Fluid::advert(int boundary, std::vector<float>& s1, std::vector<float>& s0, std::vector<float>& u, std::vector<float>& v, float dt){
    int i0, i1, j0, j1;
    float rx1, rx0, ry0, ry1;
    float newx, newy;
    float dt0 = dt*(N-2);
    for(int x = 1; x < N-1; ++x){
        for(int y = 1; y < N-1; ++y){
            newx = x - dt0*u[LOC(x, y)]; newy = y - dt0*u[LOC(x, y)];
            if(newx < 0.5) newx = 0.5; if(newx > N-0.5) newx = N - 0.5;
            if(newy < 0.5) newy = 0.5; if(newy > N-0.5) newy = N - 0.5;
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

void Fluid::project(vector<float>& u, vector<float>& v, vector<float>& p, vector<float>& div){
    float h = 1.f/(N-2);
    for(int x = 1; x < N-1; ++x){
        for(int y = 0; y < N-1; ++y){
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