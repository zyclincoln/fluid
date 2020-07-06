#define GLFW_INCLUDE_GLU
#include <iostream>
#include <GLFW/glfw3.h>

#include "define.hpp"
#include "fluid.hpp"

using namespace std;

void mouseEvent(GLFWwindow* window, int button, int action, int mods){

}

void cursorEvent(GLFWwindow* window, double xpos, double ypos){

}

int main(int argc, char** argv){
    if(!glfwInit()){
        cerr << "Window initializeation failed." << endl;
        exit(-1);
    }

    GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "stable fluid", nullptr, nullptr);
    if(!window){
        cerr << "Window creation failed." << endl;
        glfwTerminate();
        exit(-1);
    }

    glfwMakeContextCurrent(window);

    glfwSetMouseButtonCallback(window, mouseEvent);
    glfwSetCursorPosCallback(window, cursorEvent);
    glClearColor(0.f, 0.f, 0.f, 1.f);

    Fluid fluid;

    while(!glfwWindowShouldClose(window)){
        fluid.draw();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}