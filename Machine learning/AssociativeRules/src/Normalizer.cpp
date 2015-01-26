#include "Normalizer.h"

#include <boost/algorithm/string.hpp>

#include <map>
#include <vector>

void Normalizer::load(bool with_data) {
    for (int i = 0; i < 3; ++i) {
        descr_size[i] = 0;
    }
    while (!in_.eof()) {
        std::string str;
        in_ >> str;
        if (str == "@attribute") {
            in_ >> str;
            Groups gr = GROUPS_SIZE;
            if (str == "product_name")
                gr = PRODUCT_NAME;
            else if (str == "product_category")
                gr = PRODUCT_CAT;
            else if (str == "product_department")
                gr = PRODUCT_DEPART;
            if (gr != GROUPS_SIZE) {
                char symb;
                in_ >> symb;
                while (symb != '{' && !in_.eof()) {
                    in_ >> symb;
                }
                in_ >> symb;
                str = "";
                size_t counter = 0;
                while (symb != '}' && !in_.eof()) {
                    if (symb == ',') {
                        name_to_id[gr][str] = counter;
                        id_to_name[gr][counter] = str;
                        descr_size[gr]++;
                        ++counter;
                        str = "";
                    } else {
                        str += symb;
                    }
                    symb = in_.get();
                }
                name_to_id[gr][str] = counter;
                descr_size[gr]++;
            }
        }
        if (str == "@data" && with_data) {
            std::string temp;
            std::getline(in_, temp);
            std::getline(in_, temp);
            std::string old_id = "";
            while (!in_.eof()) {
                std::vector<std::string> tokens;
                boost::split(tokens, temp, boost::is_any_of(","));
                if (old_id != tokens[3]) {
                    transact[0].push_back("");
                    transact[1].push_back("");
                    transact[2].push_back("");
                    transact[0].back().resize(descr_size[0], '0');
                    transact[1].back().resize(descr_size[1], '0');
                    transact[2].back().resize(descr_size[2], '0');
                    old_id = tokens[3];
                }
                for (int i = 0; i < 3; ++i)
                    transact[i].back()[name_to_id[i][tokens[i]]] = '1';
                std::getline(in_, temp);
            }
        }
    }

}

void Normalizer::save() {
    for (size_t i = 0; i < 3; ++i) {
        std::cout << transact[i].size() << std::endl;
        for (size_t j = 0; j < transact[i].size(); ++j) {
            out_ << transact[i][j] << std::endl;
        }
        out_ << "%%%" << std::endl;
    }
}
