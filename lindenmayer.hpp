#pragma once

#include <iostream>
#include <fstream>
#include <map>
#include <vector>

#include "geometry.hpp"

std::string Lindenmayer(std::string axiom, std::map<std::string, std::string> rules, size_t detail);

std::pair<std::string,int> loadAndEval(std::string fname, int plusdetail = 0);

std::vector<Vertex> drawFigure(std::pair<std::string,int> data, glm::vec2 start);