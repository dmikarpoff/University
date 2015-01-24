#include <StreamHandler.h>
#include <fstream>

StreamHandler::StreamHandler(const std::string &input,
                             const std::string &output) {
    if (input.size() > 0) {
        in = new std::ifstream(input.c_str());
        in_file = true;
    } else {
        in = &std::cin;
        in_file = false;
    }
    if (output.size() > 0) {
        out = new std::ofstream(output.c_str());
        out_file = true;
    } else {
        out = &std::cout;
        out_file = false;
    }
}

StreamHandler::~StreamHandler() {
    if (in_file)
        delete in;
    if (out_file) {
        dynamic_cast<std::ofstream*>(out)->close();
        delete out;
    }
}
