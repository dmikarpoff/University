#include <IClassfier.h>
#include <KNNClassifier.h>
#include <StreamHandler.h>
#include <sstream>

#include <iostream>

void showErrorMsg() {
    std::cout << "Illegal argument format\n";
    std::cout << "Usage: <TRAIN_SET_FILE> [<INPUT_DATA_FILE>";
    std::cout << " [<OUTPUT_FILE>]] <ALOGRITHM_PARAMETER>" << std::endl;
}

int main(int argc, char** argv) {
    if (argc < 3 || argc > 5) {
        showErrorMsg();
        return 0;
    }
    std::string algo_param = argv[argc - 1];
    for (size_t i = 0; i < algo_param.size(); ++i)
        if (algo_param[i] < '0' || algo_param[i] > '9') {
            std::cout << "Error: last argument should be positive integer";
            return 0;
        }
    std::stringstream ss(algo_param);
    int k;
    ss >> k;
    if (k < 1) {
        std::cout << "Error: last argument should be positive integer";
        return 0;
    }
    IClassifier<KNN::point2d, int>* solver = new KNN::KNNClassifier(k);
    std::string train_set(argv[1]);
    std::ifstream in_train;
    in_train.open(train_set.c_str());
    if (!in_train.good()) {
        std::cout << "Unable open file " << train_set << std::endl;
        return 0;
    }

    std::vector<std::pair<KNN::point2d, int> > samples;
    while (in_train.good()) {
        double x, y;
        int label;
        char c;
        in_train >> x >> c >> y >> c >> label;
        samples.push_back(std::make_pair(KNN::point2d(x, y), label));
    }

    double int_average = 0.0, ext_average = 0.0;
    for (size_t i = 0; i < samples.size(); ++i) {
        std::vector<std::pair<KNN::point2d, int> > train_samp = samples;
        train_samp.erase(train_samp.begin() + i);
        solver->train(train_samp);
        double temp_internal = 0.0;
        for (size_t j = 0; j < samples.size(); ++j) {
            int res = solver->classify(samples[j].first);
            if (j == i)
                ext_average += (res != samples[j].second);
            else
                temp_internal += (res != samples[j].second);
        }
        int_average += temp_internal / train_samp.size();
    }

    std::cout << "Average internal error = " << int_average / samples.size()
              << std::endl;
    std::cout << "Average external error = " << ext_average / samples.size()
              << std::endl;

    StreamHandler io(argc > 3 ? argv[2] : "",
                     argc > 4 ? argv[3] : "");

    if (!io.getInput().good()) {
        std::cout << "File " << argv[2] << "doesn't exist!" << std::endl;
        return 0;
    }
    if (!io.getOutput().good()) {
        std::cout << "File " << argv[3] << "doesn't exist!" << std::endl;
        return 0;
    }

    while (io.getInput().good()) {
        std::string line;
        std::getline(io.getInput(), line);
        if (line == "exit")
            return 0;
        char c;
        double x, y;
        std::stringstream ss(line);
        ss >> x >> c >> y;
        int label = solver->classify(KNN::point2d(x, y));
        io.getOutput() << label << std::endl;
    }
    return 0;
}
