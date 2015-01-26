#ifndef INCLUDE_DECISIONTREE_H
#define INCLUDE_DECISIONTREE_H

#include <cstdlib>
#include <vector>

struct DecisionNode {
    DecisionNode(const std::vector<std::vector<int> >& samples,
                 const std::vector<int>& labels, int m);
    double val;
    DecisionNode* left, *right;
    int lbl;
    int item;
    ~DecisionNode() {}
};

class DecisionTree {
 public:
    DecisionTree(const std::vector<std::vector<int> >& samples,
                 const std::vector<int>& labels, int m);
    int vote(const std::vector<int>& sample);
    int voteStep(DecisionNode* node, const std::vector<int>& sample);
 private:
    DecisionNode root;
};


class DecisionForest {
 public:
    DecisionForest(size_t amount):
        train_size(0), dim(0) {
        tree_amount_ = amount;
    }
    void loadTrain();
    void check();
    void train();
    int classify(const std::vector<int> sample);

 private:
    size_t tree_amount_;
    size_t train_size;
    std::vector<std::vector<int> > samples;
    std::vector<int> label;
    size_t dim;
    size_t item_sample;
    std::vector<DecisionTree> tree_;
};

#endif  // INCLUDE_DECISIONTREE_H
