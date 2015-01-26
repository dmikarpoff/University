#include <cstring>
#include <fstream>
#include <iostream>

#include "Normalizer.h"
#include "Solver.h"

int main(int argc, char** argv) {
    if (argc == 4) {
        if (strcmp(argv[1], "-n") == 0) {
            std::ifstream in(argv[2]);
            std::ofstream out(argv[3]);
            if (!in.is_open()) {
                std::cout << "Unable to open input file" << std::endl;
                return 0;
            }
            if (!out.is_open()) {
                std::cout << "Unable to open output file" << std::endl;
                return 0;
            }
            Normalizer norm(in, out);
            norm.load(true);
            norm.save();
            return 0;
        }
        if (strcmp(argv[1], "-x") == 0) {
            std::ifstream norm_in(argv[2]);
            std::ifstream name_source(argv[3]);
            if (!norm_in.is_open() || !name_source.is_open()) {
                std::cout << "Failed to open file";
                return 0;
            }
            Solver solver;
            solver.extractRules(name_source, norm_in);
            return 0;
        }
    }
    std::cout << "Illegal argument format" << std::endl;
    return 0;
}
