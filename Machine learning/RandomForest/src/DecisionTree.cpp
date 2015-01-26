#include "DecisionTree.h"

#include <fstream>
#include <boost/algorithm/string.hpp>
#include <cmath>

double sqr(double x) {
    return x*x;
}

DecisionNode::DecisionNode(const std::vector<std::vector<int> > &samples,
                           const std::vector<int> &labels, int m):
            left(NULL), right(NULL) {
    if (samples.size() == 0) {
        lbl = rand() % 2 * 2 - 1;
        return;
    }
    std::vector<int> items;
    int has_plus = 0, has_minus = 0;
    for (size_t i = 0; i < labels.size(); ++i)
        if (labels[i] == 1)
            has_plus++;
        else if (labels[i] == -1)
            has_minus++;
    lbl = (has_plus != 0) * 1 + (has_minus != 0) * (-1);
    if (lbl == 0) {
        for (size_t i = 0; i < m; ++i) {
            items.push_back(rand() % samples[0].size());
        }
        double best_gini = 3.0;
        double best_val;
        int best_item;
        for (size_t i = 0; i < items.size(); ++i) {
            std::vector<std::pair<int, int> > pairs;
            for (size_t j = 0; j < samples.size(); ++j) {
                pairs.push_back(std::make_pair(samples[j][items[i]], labels[j]));
            }
            std::sort(pairs.begin(), pairs.end());
            int left_minus = 0;
            int left_plus = 0;
            int right_minus = has_minus;
            int right_plus = has_plus;
            double t_gini = 3.0;
            double t_val = 3.0;
            int t_item;
            for (size_t j = 0; j < pairs.size() - 1; ++j) {
                if (pairs[j].second == -1) {
                    left_minus++;
                    right_minus--;
                } else {
                    left_plus++;
                    right_plus--;
                }
                double gini = (j + 1) * (1 - sqr((double)(left_minus) /
                                                  (left_minus + left_plus)) -
                        sqr((double)(left_plus) / (left_minus + left_plus))) +
                        (pairs.size() - j - 1) * (1 - sqr((double)(right_minus) /
                                                         (right_minus + right_plus)) -
                            sqr((double)(right_plus) / (right_minus + right_plus)));
                gini /= pairs.size();
                if (gini < t_gini) {
                    t_gini = gini;
                    t_item = items[i];
                    t_val = 0.5 * (pairs[j].first + pairs[j + 1].first);
                }
            }
            left_minus = 0, left_plus = 0, right_minus = 0, right_plus = 0;
            double gini;
            for (size_t j = 0; j < pairs.size(); ++j) {
                if (pairs[j].first < t_val) {
                    if (pairs[j].second == -1)
                        left_minus++;
                    else
                        left_plus++;
                }
                if (pairs[j].first >= t_val) {
                    if (pairs[j].second == -1)
                        right_minus++;
                    else
                        right_plus++;
                }
            }
            gini = (left_minus + left_plus) * (1 - sqr((double)(left_minus) /
                                      (left_minus + left_plus)) -
                sqr((double)(left_plus) / (left_minus + left_plus))) +
                (right_minus + right_plus) * (1 - sqr((double)(right_minus) /
                                             (right_minus + right_plus)) -
                sqr((double)(right_plus) / (right_minus + right_plus)));
            gini /= pairs.size();
            if (gini < best_gini) {
                best_gini = gini;
                best_val = t_val;
                best_item = t_item;
            }
        }
        std::vector<std::vector<int> > left_samp, right_samp;
        std::vector<int> left_lbl, right_lbl;
        val = best_val;
        item = best_item;
        if (best_gini < 2.0) {
            for (size_t i = 0; i < samples.size(); ++i) {
                if (samples[i][best_item] < best_val) {
                    left_samp.push_back(samples[i]);
                    left_lbl.push_back(labels[i]);
                } else {
                    right_samp.push_back(samples[i]);
                    right_lbl.push_back(labels[i]);
                }
            }
        } else {
            size_t posit = 0;
            for (size_t i = 0; i < labels.size(); ++i)
                if (labels[i] > 0)
                    posit++;
            if (2 * posit > labels.size())
                lbl = 1;
            else
                lbl = -1;
            return;
        }
        left = new DecisionNode(left_samp, left_lbl, m);
        right = new DecisionNode(right_samp, right_lbl, m);
    }
}

DecisionTree::DecisionTree(const std::vector<std::vector<int> > &samples,
                           const std::vector<int> &labels, int m):
    root(samples, labels, m) {}

int DecisionTree::vote(const std::vector<int> &sample) {
    DecisionNode* node = &root;
    voteStep(node, sample);
}

int DecisionTree::voteStep(DecisionNode *node, const std::vector<int> &sample) {
    if (node->lbl != 0)
        return node->lbl;
    if (sample[node->item] < node->val)
        return voteStep(node->left, sample);
    return voteStep(node->right, sample);
}

void DecisionForest::loadTrain() {
    std::ifstream data("../data/arcene_train.data");
    std::ifstream labels("../data/arcene_train.labels");
    int lbl;
    labels >> lbl;
    while (!labels.eof()) {
        label.push_back(lbl);
        labels >> lbl;
    }
    train_size = label.size();
    for (size_t i = 0; i < train_size; ++i) {
        std::string line;
        std::getline(data, line);
        std::vector<std::string> str;
        boost::split(str, line, boost::is_any_of(" "));
        std::vector<int> vec;
        for (size_t i = 0; i < str.size(); ++i) {
            if (str[i] != "")
                vec.push_back(atoi(str[i].c_str()));
        }
        samples.push_back(vec);
    }
    dim = samples[0].size();
    item_sample = 2 * sqrt(dim);
}

void DecisionForest::check() {
    std::ifstream data("../data/arcene_valid.data");
    std::ifstream labels("../data/arcene_valid.labels");
    std::vector<std::vector<int> > samp;
    std::vector<int> lbl;
    while (!labels.eof()) {
        int tl;
        labels >> tl;
        lbl.push_back(tl);
        std::vector<int> ts;
        std::vector<std::string> str;
        std::string line;
        std::getline(data, line);
        boost::split(str, line, boost::is_any_of(" "));
        for (size_t i = 0; i < str.size(); ++i) {
            ts.push_back(atoi(str[i].c_str()));
        }
        samp.push_back(ts);
    }
    size_t error = 0;
    for (size_t i = 0; i < samp.size(); ++i) {
        int res = classify(samp[i]);
        if (res != lbl[i]) {
            error++;
        }
    }
    std::cout << "Error rate = " << (double)error / samp.size() << std::endl;
}

void DecisionForest::train() {
    for (size_t i = 0; i < tree_amount_; ++i) {
        std::vector<std::vector<int> > subset;
        std::vector<int> lbl;
        for (size_t j = 0; j < train_size; ++j) {
            size_t pos = rand() % train_size;
            subset.push_back(samples[pos]);
            lbl.push_back(label[pos]);
        }
        tree_.push_back(DecisionTree(subset, lbl, item_sample));
    }
}

int DecisionForest::classify(const std::vector<int> sample) {
    size_t vote_plus = 0, vote_minus = 0;
    for (size_t i = 0; i < tree_.size(); ++i) {
        if (tree_[i].vote(sample) == 1)
            vote_plus++;
        else
            vote_minus++;
    }
    if (vote_plus > vote_minus)
        return 1;
    return -1;
}
