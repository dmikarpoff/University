#include "DataHolder.h"

#include <cmath>
#include <fstream>
#include <sstream>

using namespace boost;

int main() {
    DataHolder data_holder;
    data_holder.loadData("../data/Bayes");
    std::string prefix = "../data/Bayes/pu1/part";
    for (int i = 1; i <= 10; ++i) {
        std::string hold_out_name = prefix + std::to_string(i);
        filesystem::path bpath(hold_out_name);
        filesystem::directory_iterator end_it;
        for (filesystem::directory_iterator i(bpath); i != end_it; ++i) {
            if (!filesystem::is_directory(i->status()))
                data_holder.removeFile(i->path());
        }
        size_t total = 0;
        size_t valid = 0;
        for (filesystem::directory_iterator j(bpath); j != end_it; ++j) {
            double pspam = 0.0;
            double plegit = 0.0;
            std::string file_name = j->path().string();
            std::ifstream in(file_name.c_str());
            bool spam = (file_name.find("spmsg") != std::string::npos);
            std::string subj;
            std::getline(in, subj);
            std::stringstream ss;
            ss.str(subj);
            ss >> subj;
            while (!ss.eof()) {
                int word;
                ss >> word;
                pspam += log(data_holder.subject[word].spam + 1) -
                                log(data_holder.total_subj.spam +
                                    data_holder.subject.size());
                plegit += log(data_holder.subject[word].legit + 1) -
                                log(data_holder.total_subj.legit +
                                    data_holder.subject.size());
            }
            while (!in.eof()) {
                int word;
                in >> word;
                pspam += log(data_holder.text[word].spam + 1) -
                                log(data_holder.total_text.spam +
                                    data_holder.text.size());
                plegit += log(data_holder.text[word].legit + 1) -
                                log(data_holder.total_text.legit +
                                    data_holder.text.size());
            }
            if ((pspam > plegit && spam) || (pspam <= plegit && !spam))
                valid++;
            total++;
        }
        std::cout << "Validation on " << i << "-th fold: valid "
                  << (double)valid/total * 100.0 << "%" << std::endl;
        for (filesystem::directory_iterator i(bpath); i != end_it; ++i) {
            if (!filesystem::is_directory(i->status()))
                data_holder.readFile(i->path());
        }
    }
    return 0;
}
