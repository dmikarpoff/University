#ifndef INCLUDE_SOLVER_H_
#define INCLUDE_SOLVER_H_

#include "Normalizer.h"
#include <set>

#define MINSUP_PROD 3
#define MINSUP_CAT 3
#define MINSUP_DEP 3
#define MIN_CONF 0.4

struct StringNode {
    StringNode():
        is_leaf(false) {}
    std::map<size_t, StringNode*> child;
    bool is_leaf;
    std::set<int> key;
    ~StringNode();
};

struct StringTree {
    StringNode root;
    void insert(std::set<int> val);
    void find(std::set<int> key, std::map<std::set<int>, size_t>& support);
    void findStep(StringNode* node, std::set<int> key,
                  std::map<std::set<int>, size_t>& support);
};

class Solver {
public:
    void extractRules(std::ifstream& data, std::ifstream& norm);
private:
    void extractStep(size_t idx, std::set<int> left, std::set<int> right,
                    const std::map<std::set<int>, size_t>& support);
    Normalizer* normalizer;
    static const int supp_bound[];
};

#endif  // INCLUDE_SOLVER_H_
