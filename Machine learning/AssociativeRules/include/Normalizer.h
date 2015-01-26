#ifndef INCLUDE_NORMALIZER_H_
#define INCLUDE_NORMALIZER_H_

#include <fstream>
#include <map>
#include <vector>

enum Groups {
    PRODUCT_NAME = 0,
    PRODUCT_CAT,
    PRODUCT_DEPART,
    GROUPS_SIZE
};

class Normalizer {
 public:
    Normalizer(std::ifstream& in, std::ofstream& out):
        in_(in), out_(out) {}
    void load(bool with_data);
    void save();

    size_t descr_size[3];
    std::map<std::string, size_t> name_to_id[3];
    std::map<size_t, std::string> id_to_name[3];
 private:
    std::ifstream& in_;
    std::ofstream& out_;
    std::vector<std::string> transact[3];
};

#endif  // INCLUDE_NORMALIZER_H_
