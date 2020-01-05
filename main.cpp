#include <iostream>
#include <fstream>
#include <map>
#include <stack>
#include <vector>
#include <cassert>

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>
#include <glm/glm.hpp>

std::string Lindenmayer(std::string axiom, std::map<std::string, std::string> rules, size_t N)
{
    std::string word = axiom;
    for(size_t i=0;i<N;++i)
    {
        for(size_t k=0;k<word.size();++k)
        {
            for(size_t j=k;j<word.size();++j)
            {
                if( rules.find( word.substr(k,j-k+1) ) != rules.end() )
                {
                    std::string s = rules[word.substr(k,j-k+1)];
                    word.replace(k, j-k+1, s);
                    k = k+s.length()-1;
                    break;
                }
            }
        }
    }
    return word;
}

std::pair<std::string,int> loadAndEval(std::string fname, int plusdetail = 0)
{
    std::ifstream in(fname.c_str());
    int n, N, angle;
    std::map<std::string, std::string> rules;
    std::string axiom;
    in >> n;
    for(int i=0;i<n;++i)
    {
        std::string a,b;
        in >> a >> b;
        rules[a] = b;
    }
    in >> axiom;
    in >> N;
    in >> angle;
    N += plusdetail;
    assert(N >= 0);
    return std::make_pair(Lindenmayer(axiom, rules, N), angle);
}

struct state{
    glm::vec2 pos;
    glm::vec2 dir;
};


struct Vertex
{
    glm::vec3 p;
    glm::vec3 c;
};

std::tuple<GLuint, GLuint, size_t> genBuffer(std::pair<std::string,int> data, glm::vec2 start)
{
    std::string str = data.first;
    int angleDeg = data.second;
    double angle = angleDeg*2*M_PI/360;

    double x = start.x, y = start.y;
    double xd = 0.03, yd = 0;
    glm::vec2 xy = start;
    glm::vec2 dir = {0.03, 0};
    
    glm::mat2 rot = {cos(angle), -sin(angle), sin(angle), cos(angle)};
    
    std::stack<state> s;
    state st;
    
    std::vector<Vertex> vertices;
    
    for(char c : str)
    {
        double a;
        switch(c)
        {
            case 'F':
            case 'G':
                vertices.push_back({glm::vec3(xy, 0), glm::vec3(0,1,0)});
                vertices.push_back({glm::vec3(xy+dir, 0), glm::vec3(0,1,0)});
                xy += dir;
                break;
            case 'B':
                xy += dir;
                break;
            case '-':
				dir = glm::transpose(rot) * dir;
                break;
            case '+':
				dir = rot * dir;
                break;
            case '[':
                s.push(state{xy, dir});
                break;
            case ']':
                st = s.top();
                xy = st.pos; dir = st.dir;
                s.pop();
                break;
            case 'X':
            case 'Y':
                break;
            default:
                std::cerr << "Unhandled character" << std::endl;
        }
    }
    
    GLuint m_vaoID, m_vboID;
    
    glGenVertexArrays(1, &m_vaoID);
    glBindVertexArray(m_vaoID);
    
    glGenBuffers(1, &m_vboID); 
    glBindBuffer(GL_ARRAY_BUFFER, m_vboID);

    glBufferData( GL_ARRAY_BUFFER,
                  vertices.size() * sizeof(Vertex),
                  vertices.data(),
                  GL_STATIC_DRAW);
    

    glEnableVertexAttribArray(0); 
    glVertexAttribPointer(
        (GLuint)0,              
        3,              
        GL_FLOAT,       
        GL_FALSE,       
        sizeof(Vertex), 
        0               
    ); 

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(
        (GLuint)3,
        3, 
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
        (void*)(sizeof(glm::vec3)) );

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return {m_vaoID, m_vboID, vertices.size()};
}

int draw(std::pair<std::string,int> data)
{
    std::vector<std::string> examples{"cantor.txt", "sierpinski_tr.txt", "sierpinski_crp.txt", "dragon.txt", "koch.txt", "plant.txt"};
    std::vector<glm::vec3> defaultSettings{{-0.78, -0.16, 0.65},{-0.46, -0.4, 1.8},{-0.62, 0, 1.25},{0.02, 0.24, 1.2},{-0.78, -0.16, 0.64},{-0.78, -0.16, 0.6}};
    std::vector<double> scalefactors{3, 2, 3, 1.2, 3, 2};

    std::string str = data.first;
    int angleDeg = data.second;
    double angle = angleDeg*2*M_PI/360;

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

    SDL_Window * window = SDL_CreateWindow("L-Systems",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    SDL_GLContext context = SDL_GL_CreateContext(window);
    if (context == nullptr)
    {
        std::cout << "[OGL context létrehozása]Hiba az SDL inicializálása közben: " << SDL_GetError() << std::endl;
        return 1;
    }   
    SDL_GL_SetSwapInterval(1);

    GLenum error = glewInit();
    if ( error != GLEW_OK )
    {
        std::cout << "[GLEW] Hiba az inicializálás során!" << std::endl;
        return 1;
    }

    GLuint vao, vbo;
    size_t N;

    std::tie(vao, vbo, N) = genBuffer(data, glm::vec2(0,0));

    int act = 0;
    int plusdetail = 0;
    bool quit = false;
    SDL_Event event;
    double win_x = 1, win_y = 1;
    double start_x = defaultSettings[act].x, start_y = defaultSettings[act].y;
    double size = defaultSettings[act].z;
    while (!quit)
    {
        while(SDL_PollEvent(&event))
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
                            act = event.key.keysym.sym - 49;
                            plusdetail = 0;
                            data = loadAndEval("examples/"+examples[act], plusdetail);
                            start_x = defaultSettings[act].x;
                            start_y = defaultSettings[act].y;
                            size = defaultSettings[act].z;
                            break;
                        case SDLK_p:
                            ++plusdetail;
                            data = loadAndEval("examples/"+examples[act], plusdetail);
                            size /= scalefactors[act];
                            break;
                        case SDLK_m:
                            --plusdetail;
                            data = loadAndEval("examples/"+examples[act], plusdetail);
                            size *= scalefactors[act];
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
                    str = data.first;
                    angleDeg = data.second;
                    angle = angleDeg*2*M_PI/360;
                    
                    glDeleteBuffers(1, &vao);
                    glDeleteVertexArrays(1, &vbo);
                    
                    std::tie(vao, vbo, N) = genBuffer(data, glm::vec2(0,0));
                    break;
                case SDL_WINDOWEVENT:
                    if ( event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED )
                    {
                        glViewport(0, 0, event.window.data1, event.window.data2);
                        win_x = event.window.data1 / 1000.0;
                        win_y = event.window.data2 / 1000.0;
                    }
                    break;
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

        SDL_GL_SwapWindow(window);
    }
 
    SDL_DestroyWindow(window);
    SDL_Quit();

    glDeleteBuffers(1, &vao);
    glDeleteVertexArrays(1, &vbo);

    return 0;
}

int main(int argc, char** argv)
{
    std::vector<std::string> examples{"cantor.txt", "sierpinski_tr.txt", "sierpinski_crp.txt", "dragon.txt", "koch.txt", "plant.txt"};
    return draw(loadAndEval("examples/"+examples[0]));
}
