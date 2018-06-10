#include <iostream>
#include <fstream>
#include <map>
#include <stack>
#include <vector>
#include <cassert>

using namespace std;

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

std::pair<std::string,std::pair<int,int>> loadAndEval(std::string fname, int plusdetail = 0)
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
	return std::make_pair(Lindenmayer(axiom, rules, N), std::make_pair(1 << N, angle));
}

#include <SDL2/SDL.h>

struct state{
	double xsv, ysv, xdsv, ydsv;
};

int draw(std::pair<std::string,std::pair<int,int>> data)
{
	std::vector<std::string> examples{"cantor.txt", "sierpinski_tr.txt", "sierpinski_crp.txt", "dragon.txt", "koch.txt", "plant.txt"};
    // variables
    std::string str = data.first;
    int N = data.second.first;
    int angleDeg = data.second.second;
    double angle = angleDeg*2*M_PI/360;
    std::cerr << str;
 
    bool quit = false;
    SDL_Event event;
 
    // init SDL
 
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window * window = SDL_CreateWindow("L-Systems",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1600, 1200, 0);
    SDL_Renderer * renderer = SDL_CreateRenderer(window, -1, 0);
 
    // handle events
 
    double size = 100;
    int act = 4;
    int plusdetail = 0;
    while (!quit)
    {
        SDL_Delay(10);
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
						break;
					case SDLK_p:
						++plusdetail;
						data = loadAndEval("examples/"+examples[act], plusdetail);
						break;
					case SDLK_m:
						--plusdetail;
						data = loadAndEval("examples/"+examples[act], plusdetail);
						break;
				}
				str = data.first;
				N = data.second.first;
				angleDeg = data.second.second;
				angle = angleDeg*2*M_PI/360;
				break;
        }
 
        // clear window
 
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        double x = 300, y = 700;
        double xd = 1, yd = 0;
        
        std::stack<state> s;
        state st;
        
        for(char c : str)
        {
			double a;
			switch(c)
			{
				case 'F':
				case 'G':
					SDL_RenderDrawLine(renderer, x, y, x+size*xd/N, y+size*yd/N);
					x += size*xd/N;
					y += size*yd/N;
					break;
				case 'B':
					x += size*xd/N;
					y += size*yd/N;
					break;
				case '+':
					a = xd;
					xd = xd * cos(angle) - yd * sin(angle);
					yd = a * sin(angle) + yd * cos(angle);
					break;
				case '-':
					a = xd;
					xd = xd * cos(angle) + yd * sin(angle);
					yd = -a * sin(angle) + yd * cos(angle);
					break;
				case '[':
					s.push(state{x,y,xd,yd});
					break;
				case ']':
					st = s.top();
					x = st.xsv; y = st.ysv; xd = st.xdsv; yd = st.ydsv;
					s.pop();
					break;
				case 'X':
				case 'Y':
					break;
				default:
					cerr << "Unhandled character" << endl;
			}
			//SDL_RenderPresent(renderer);
			//SDL_Delay(100);
		}
 
        // render window
 
        SDL_RenderPresent(renderer);
    }
 
    // cleanup SDL
 
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
 
    return 0;
}

int main(int argc, char ** argv)
{
	std::vector<std::string> examples{"cantor.txt", "sierpinski_tr.txt", "sierpinski_crp.txt", "dragon.txt", "koch.txt", "plant.txt"};
	
	return draw(loadAndEval("examples/"+examples[0]));
}
