#include "graphics.hpp"

#include <iostream>

// allocates and returns an opengl buffer from a given vector of vertex data
std::tuple<GLuint, GLuint, size_t> bufferFromVector(const std::vector<Vertex>& vertices)
{
    GLuint m_vaoID, m_vboID;
    
    glGenVertexArrays(1, &m_vaoID);
    glBindVertexArray(m_vaoID);
    
    glGenBuffers(1, &m_vboID); 
    glBindBuffer(GL_ARRAY_BUFFER, m_vboID);

    glBufferData( GL_ARRAY_BUFFER,
                  vertices.size() * sizeof(Vertex),
                  vertices.data(),
                  GL_STATIC_DRAW);
    
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(Vertex), (void*)NULL);

	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(3, GL_FLOAT, sizeof(Vertex), (void*)sizeof(glm::vec3));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return {m_vaoID, m_vboID, vertices.size()};
}

void deleteBuffer(std::tuple<GLuint, GLuint> vaovbo)
{
    GLuint vao, vbo;
    std::tie(vao, vbo) = vaovbo;
    glDeleteBuffers(1, &vao);
    glDeleteVertexArrays(1, &vbo);
}

// creates SDL window, etc.
void UI::init()
{
    SDL_Init(SDL_INIT_VIDEO);


    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE,         32);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE,            8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,          8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,           8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE,          8);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,        1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,          24);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS,  1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES,  2);

    window = SDL_CreateWindow("L-Systems",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    SDL_GLContext context = SDL_GL_CreateContext(window);
    if (context == nullptr)
    {
        std::cout << "[OGL context létrehozása]Hiba az SDL inicializálása közben: " << SDL_GetError() << std::endl;
        exit(1);
    }   
    SDL_GL_SetSwapInterval(1);

    GLenum error = glewInit();
    if ( error != GLEW_OK )
    {
        std::cout << "[GLEW] Hiba az inicializálás során!" << std::endl;
        exit(1);
    }
}

void UI::destroy()
{
    SDL_DestroyWindow(window);
    SDL_Quit();
}