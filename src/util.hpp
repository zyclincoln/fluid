#ifndef _UTIL_HPP_
#define _UTIL_HPP_

#include <vector>
#include <cmath>

#include "define.hpp"

bool check_horizontal_symmetric(std::vector<double>& field){
    double error = 0;
    for(int x = 1; x < (N-1)>>1; ++x){
        for(int y = 1; y < N-1; ++y){
            error += fabs(field[LOC(x, y)] - field[LOC(N-1-x, y)]);
        }
    }
    std::cout << "symmetric error: " << error << std::endl;
    return (error < 1e-6);
}

void horizontal_symmetrify(std::vector<double>& field){
    for(int x = 1; x < (N-1)>>1; ++x){
        for(int y = 1; y < N-1; ++y){
            field[LOC(N-1-x, y)] = field[LOC(x, y)] = (field[LOC(x, y)]+field[LOC(N-1-x, y)])*0.5;
        }
    }
}


#endif