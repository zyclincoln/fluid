#ifndef _DEFINE_HPP_
#define _DEFINE_HPP_

constexpr int WINDOW_WIDTH=800;
constexpr int WINDOW_HEIGHT=800;
constexpr int DIM = 2;
constexpr int N = 130;
constexpr int ARRAYSIZE(){ return N*N;}

#define LOC(X, Y) ((X)+(Y)*N)

#endif