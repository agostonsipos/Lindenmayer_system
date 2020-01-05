#pragma once

#include <vector>
#include <tuple>

#include <GL/glew.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include "geometry.hpp"

std::tuple<GLuint, GLuint, size_t> bufferFromVector(const std::vector<Vertex>&);

void deleteBuffer(std::tuple<GLuint, GLuint>);

class UI{
public:
    void init();
    void swap(){ SDL_GL_SwapWindow(window); }
    void destroy();
private:
    SDL_Window* window;
};