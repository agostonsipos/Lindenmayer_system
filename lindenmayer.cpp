#include "lindenmayer.hpp"

#include <stack>

// substitutes Lindenmayer grammar rules a given number of times
// returns result string
std::string Lindenmayer(std::string axiom, std::map<std::string, std::string> rules, size_t detail)
{
    std::string word = axiom;
    for(size_t i=0;i<detail;++i)
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

// loads a file with rules, and calls above function to evaluate
std::pair<std::string,int> loadAndEval(std::string fname, int plusdetail)
{
    std::ifstream in(fname.c_str());
    int rulenum, detail, angle;
    std::map<std::string, std::string> rules;
    std::string axiom;
    in >> rulenum;
    for(int i=0;i<rulenum;++i)
    {
        std::string a,b;
        in >> a >> b;
        rules[a] = b;
    }
    in >> axiom;
    in >> detail;
    in >> angle;
    detail += plusdetail;
    if(detail >= 0)
        return std::make_pair(Lindenmayer(axiom, rules, detail), angle);
    else
        return std::make_pair("", angle);
}

struct state{
    glm::vec2 pos;
    glm::vec2 dir;
};

// calculates point positions based on L-system string
std::vector<Vertex> drawFigure(std::pair<std::string,int> data, glm::vec2 start)
{
    std::string str = data.first;
    int angleDeg = data.second;
    double angle = angleDeg*2*M_PI/360;

    glm::vec2 xy = start;
    glm::vec2 dir = {0.03, 0};
    
    glm::mat2 rot = {cos(angle), -sin(angle), sin(angle), cos(angle)};
    
    std::stack<state> s;
    state st;
    
    std::vector<Vertex> vertices;
    
    try{
        for(char c : str)
        {
            switch(c)
            {
                case 'F':
                case 'G':
                    // move forward and draw
                    vertices.push_back({glm::vec3(xy, 0), glm::vec3(0,1,0)});
                    vertices.push_back({glm::vec3(xy+dir, 0), glm::vec3(0,1,0)});
                    xy += dir;
                    break;
                case 'A':
                case 'B':
                    // move forward, no draw
                    xy += dir;
                    break;
                case '-':
                    // rotate, negative angle
                    dir = glm::transpose(rot) * dir;
                    break;
                case '+':
                    // rotate, positive angle
                    dir = rot * dir;
                    break;
                case '[':
                    // save state
                    s.push(state{xy, dir});
                    break;
                case ']':
                    // restore state
                    st = s.top();
                    xy = st.pos; dir = st.dir;
                    s.pop();
                    break;
                case 'X':
                case 'Y':
                    // skip
                    break;
                default:
                    throw("Unhandled character");
                    //std::cerr << "Unhandled character" << std::endl;
            }
        }
    } catch(const char* e) {
        std::cerr << "Error while processing: " << e << std::endl;
        return std::vector<Vertex>();
    }
    return vertices;
}