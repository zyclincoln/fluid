#ifndef _RENDERER_HPP_
#define _RENDERER_HPP_

#include <vector>
#include <iostream>

#include "define.hpp"

void draw(std::vector<float>& field){
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glLoadIdentity();
    glOrtho(0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, -1.0, 1.0);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for(int y = 0; y < N; ++y){
        for(int x = 0; x < N; ++x){
            glColor4d(1.f, 1.f, 1.f, field[LOC(x, y)]);
            glRectf(x*WINDOW_WIDTH*1.f/N, y*WINDOW_HEIGHT*1.f/N, 
                    (x+1)*WINDOW_WIDTH*1.f/N, (y+1)*WINDOW_HEIGHT*1.f/N);
        }
    }
}

void draw(std::vector<float>& u, std::vector<float>& v){
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