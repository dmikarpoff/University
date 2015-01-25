#include <IClassifier.h>
#include <StreamHandler.h>
#include <LogitSolver.h>
#include <sstream>

#include <iostream>

void showErrorMsg() {
    std::cout << "Illegal argument format\n";
    std::cout << "Usage: \t" << "-t <TRAIN_SET_FILE> <FILE_TO_SAVE>" << std::endl;
    std::cout << "\t-s <TRAIN_SET_FILE> [<INPUT_DATA_FILE>";
    std::cout << " [<OUTPUT_FILE>]]" << std::endl;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        showErrorMsg();
        return 0;
    }
    std::string key = argv[1];
    if (key == "-t") {
        if (argc != 4) {
            showErrorMsg();
            return 0;
        }
        std::ifstream in(argv[2]);
        std::ofstream out(argv[3]);
        if (!in.good()) {
            std::cout << "Error: Unable open file " << argv[2] << std::endl;
            return 0;
        }
        if (!out.is_open()) {
            std::cout << "Error: Unable open file " << argv[3] << std::endl;
            return 0;
        }
        IProbClassifier<logit::pair_dd, bool>* solver =
                                new logit::LogitSolver();
        double x, y;
        bool label;
        char c;
        std::vector<std::pair<logit::pair_dd, bool> > samples;
        while (in.good()) {
            in >> x >> c >> y >> c >> label;
            samples.push_back(std::make_pair(std::make_pair(x, y), label));
        }
        if (!solver->train(samples)) {
            std::cout << "Error: unable train on train set" << std::endl;
        } else {
            solver->save(out);
            double error_int = 0.0;
            for (size_t i = 0; i < samples.size(); ++i) {
                logit::pair_bd res = solver->classify(samples[i].first);
                error_int += static_cast<double>(res.first != samples[i].second);
            }
            std::cout << "Internal error = " << error_int / samples.size() << std::endl;
        }
        delete solver;
        return 0;
    }
    if (key == "-s") {
        IProbClassifier<logit::pair_dd, bool>* solver =
                new logit::LogitSolver();
        std::string train_set(argv[2]);
        std::ifstream in_train;
        in_train.open(train_set.c_str());
        if (!in_train.good()) {
            std::cout << "Unable open file " << train_set << std::endl;
            delete solver;
            return 0;
        }
        solver->load(in_train);
        StreamHandler io(argc > 3 ? argv[3] : "",
                         argc > 4 ? argv[4] : "");

        if (!io.getInput().good()) {
            std::cout << "Error: Unable open file " << argv[3] << std::endl;
            delete solver;
            return 0;
        }
        if (!io.getOutput().good()) {
            std::cout << "Error: Unable open file " << argv[4] << std::endl;
            delete solver;
            return 0;
        }

        while (io.getInput().good()) {
            std::string line;
            std::getline(io.getInput(), line);
            if (line == "exit") {
                delete solver;
                return 0;
            }
            char c;
            double x, y;
            std::stringstream ss(line);
            ss >> x >> c >> y;
            logit::pair_bd label = solver->classify(std::make_pair(x, y));
            io.getOutput() << label.second << std::endl;
        }
        delete solver;
        return 0;
    }
    showErrorMsg();
    return 0;
}
