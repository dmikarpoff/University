#ifndef INCLUDE_STREAMHANDLER_H_
#define INCLUDE_STREAMHANDLER_H_

#include <string>
#include <iostream>

class StreamHandler {
 public:
    StreamHandler(const std::string& input,
                         const std::string& output);
    std::istream& getInput() const {
        return *in;
    }
    std::ostream& getOutput() const {
        return *out;
    }
    ~StreamHandler();
 private:
    std::istream* in;
    std::ostream* out;
    bool in_file, out_file;
};

#endif  // INCLUDE_STREAMHANDLER_H_
