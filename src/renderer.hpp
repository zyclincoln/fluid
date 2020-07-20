#ifndef _RENDERER_HPP_
#define _RENDERER_HPP_

#include <vector>
#include <iostream>

#include "define.hpp"

void draw(std::vector<double>& field){
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glLoadIdentity();
    glOrtho(0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, -1.0, 1.0);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for(int y = 1; y < N-1; ++y){
        for(int x = 1; x < N-1; ++x){
            glColor4d(0.f, field[LOC(x, y)], field[LOC(x, y)], 1.f);
            glRectf((x-1)*WINDOW_WIDTH*1.f/(N-2), (y-1)*WINDOW_HEIGHT*1.f/(N-2), 
                    x*WINDOW_WIDTH*1.f/(N-2), y*WINDOW_HEIGHT*1.f/(N-2));
        }
    }
}

void draw(std::vector<double>& u, std::vector<double>& v){
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glLoadIdentity();
    glOrtho(0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, -1.0, 1.0);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor4d(0.f, 0.f, 1.f, 1.f);

    int window_w_pixels = WINDOW_WIDTH*1.f/(N-2);
    int window_h_pixels = WINDOW_HEIGHT*1.f/(N-2);
    glBegin(GL_LINES);
    for(int y = 1; y < N-1; ++y){
        for(int x = 1; x < N-1; ++x){
            if(u[LOC(x, y)] == 0.f && v[LOC(x, y)] == 0.f){
                continue;
            }
            double m = std::max(fabs(u[LOC(x, y)]), fabs(v[LOC(x, y)]))*2;
            glVertex2f(window_w_pixels*(0.5+x), window_h_pixels*(0.5+y));
            glVertex2f(window_w_pixels*(0.5+x+u[LOC(x, y)]/m), 
                       window_h_pixels*(0.5+y+v[LOC(x, y)]/m));
        }
    }
    glEnd();
}

#endif