#include "lindenmayer.hpp"
#include "graphics.hpp"

std::tuple<GLuint, GLuint, size_t> genBuffer(std::string fname, int plusdetail = 0, glm::vec2 start = glm::vec2(0))
{
    return bufferFromVector(drawFigure(loadAndEval(fname, plusdetail), start));
}

int main(int argc, char** argv)
{
    // files containing the grammar rules
    std::vector<std::string> examples{"cantor.txt", "sierpinski_tr.txt", "sierpinski_crp.txt", "dragon.txt", "koch.txt", "plant.txt"};
    // default positioning and scale
    std::vector<glm::vec3> defaultSettings{{-0.78, -0.16, 0.65},{-0.46, -0.4, 1.8},{-0.62, 0, 1.25},{0.02, 0.24, 1.2},{-0.78, -0.16, 0.64},{-0.78, -0.16, 0.6}};
    // scale factor between detail levels
    std::vector<double> scalefactors{3, 2, 3, 1.2, 3, 2};

    // creates SDL window, etc.
    UI ui;
    ui.init();

    // OpenGL buffer
    GLuint vao, vbo;
    size_t N;
    std::tie(vao, vbo, N) = genBuffer("examples/"+examples[0]);

    // selected fractal id
    int act = 0;
    // detail level increase/decrease
    int plusdetail = 0;
    SDL_Event event;
    // window width and height multiplier
    double win_x = 1, win_y = 1;
    // translation parameters
    double start_x = defaultSettings[act].x, start_y = defaultSettings[act].y;
    // scale parameter
    double size = defaultSettings[act].z;
    bool quit = false;
    while (!quit)
    {
        while(SDL_PollEvent(&event))
        {
            bool bufferChange = false;
            switch (event.type)
            {
                case SDL_QUIT:
                    quit = true;
                    break;
                case SDL_MOUSEWHEEL:
                    if(event.wheel.y > 0)
                        size *= 1.1;
                    else if(event.wheel.y < 0)
                        size /= 1.1;
                    break;
                case SDL_KEYDOWN:
                    switch(event.key.keysym.sym)
                    {
                        case SDLK_1:
                        case SDLK_2:
                        case SDLK_3:
                        case SDLK_4:
                        case SDLK_5:
                        case SDLK_6:
                            act = event.key.keysym.sym - '1';
                            plusdetail = 0;
                            start_x = defaultSettings[act].x;
                            start_y = defaultSettings[act].y;
                            size = defaultSettings[act].z;
                            bufferChange = true;
                            break;
                        case SDLK_p:
                            ++plusdetail;
                            size /= scalefactors[act];
                            bufferChange = true;
                            break;
                        case SDLK_m:
                            --plusdetail;
                            size *= scalefactors[act];
                            bufferChange = true;
                            break;
                        case SDLK_UP:
                            start_y += 0.02;
                            break;
                        case SDLK_DOWN:
                            start_y -= 0.02;
                            break;
                        case SDLK_RIGHT:
                            start_x += 0.02;
                            break;
                        case SDLK_LEFT:
                            start_x -= 0.02;
                            break;
                    }
                    if(bufferChange){
                        deleteBuffer({vao, vbo});
                        std::tie(vao, vbo, N) = genBuffer("examples/"+examples[act], plusdetail, glm::vec2(0,0));
                    }
                    break;
                case SDL_WINDOWEVENT:
                    if ( event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED )
                    {
                        glViewport(0, 0, event.window.data1, event.window.data2);
                        win_x = event.window.data1 / 800.0;
                        win_y = event.window.data2 / 600.0;
                    }
                    break;
            }
        }
        
        glClearColor(0.f, 0.f, 0.f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindVertexArray(vao);

        glPushMatrix(); 
        glTranslatef(start_x,start_y,0);
        glScalef (size/win_x, size/win_y, size);
        glDrawArrays(GL_LINES, 0, N);
        glPopMatrix();

        glBindVertexArray(0);

        ui.swap();
    }
    ui.destroy();
    
    deleteBuffer({vao, vbo});

    return 0;
}