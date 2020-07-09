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
            glColor4d(1.f, 1.f, 1.f, field[LOC(x, y)]);
            glRectf(x*WINDOW_WIDTH*1.f/(N-2), y*WINDOW_HEIGHT*1.f/(N-2), 
                    (x+1)*WINDOW_WIDTH*1.f/(N-2), (y+1)*WINDOW_HEIGHT*1.f/(N-2));
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

    // for(int y = 0; y < N; ++y){
    //     for(int x = 0; x < N; ++x){
    //         glColor4d
    //     }
    // }
}

#endif