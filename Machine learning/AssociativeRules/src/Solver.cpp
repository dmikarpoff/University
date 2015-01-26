#include "Solver.h"

#include <iostream>
#include <set>

const int Solver::supp_bound[] = {MINSUP_PROD, MINSUP_CAT, MINSUP_DEP};

StringNode::~StringNode() {
    for (std::map<size_t, StringNode*>::iterator
            i = child.begin(); i != child.end(); ++i)
        delete i->second;
}

void StringTree::insert(std::set<int> val) {
    StringNode* node = &root;
    for (std::set<int>::iterator
                i = val.begin(); i != val.end(); ++i) {
        if (node->child.find(*i) == node->child.end()) {
            node->child[*i] = new StringNode();
        }
        node = node->child[*i];
    }
    node->is_leaf = true;
    node->key = val;
}

void StringTree::find(std::set<int> key, std::map<std::set<int>, size_t> &support) {
    findStep(&root, key, support);
}

void StringTree::findStep(StringNode *node, std::set<int> key,
                          std::map<std::set<int>, size_t> &support) {
    if (node->is_leaf) {
        support[node->key]++;
        return;
    }
    while (key.size() > 0) {
        std::map<size_t, StringNode*>::iterator i =
                node->child.find(*(key.begin()));
        if (i != node->child.end()) {
            key.erase(key.begin());
            findStep(i->second, key, support);
        } else {
            key.erase(key.begin());
        }
    }
}

void Solver::extractRules(std::ifstream &data, std::ifstream &norm) {
    std::ofstream temp("temp");
    normalizer = new Normalizer(data, temp);
    normalizer->load(false);
    std::map<std::set<int>, size_t> support[3];
    for (size_t i = 0; i < 3; ++i) {
        norm.clear();
        norm.seekg(0);
        size_t sz = normalizer->descr_size[i];
        std::map<std::set<int>, size_t> temp_support;
        std::set<std::set<int> > last_set;
        for (size_t j = 0; j < sz; ++j) {
            norm.clear();
            norm.seekg(0);
            std::set<int> s;
            s.insert(j);
            size_t val = 0;
            int delim_cnt = 0;
            std::string line;
            std::getline(norm, line);
            while (!norm.eof()) {
                if (line == "%%%") {
                    delim_cnt++;
                } else if (delim_cnt == i) {
                    if (line[j] == '1')
                        val++;
                }
                std::getline(norm, line);
            }
            if (val >= supp_bound[i]) {
                temp_support[s] = val;
                last_set.insert(s);
            }
        }
        while (last_set.size() > 0) {
            std::vector<bool> available(normalizer->descr_size[i], false);
            for (std::set<std::set<int> >::iterator
                    j = last_set.begin(); j != last_set.end(); ++j)
                for (std::set<int>::iterator h = j->begin();
                                h != j->end(); ++h)
                    available[*h] = true;
            std::map<std::set<int>, size_t> new_set;
            for (std::set<std::set<int> >::iterator
                    j = last_set.begin(); j != last_set.end(); ++j) {
                for (size_t h = 0; h < normalizer->descr_size[i]; ++h) {
                    if (available[h] && j->find(h) == j->end()) {
                        std::set<int> s = *j;
                        s.insert(h);
                        new_set[s] = 0;
                    }
                }
            }
            for (std::map<std::set<int>, size_t>::iterator
                    j = new_set.begin(); j != new_set.end(); ++j) {
                bool to_del = false;
                for (std::set<int>::iterator
                            h = j->first.begin(); h != j->first.end(); ++h) {
                        std::set<int> s = j->first;
                        s.erase(*h);
                        if (last_set.find(s) == last_set.end()) {
                            to_del = true;
                            break;
                        }
                    }
                if (to_del) {
                    new_set.erase(j);
                    ++j;
                }
            }
            norm.clear();
            norm.seekg(0);
            std::string line;
            std::getline(norm, line);
            size_t delim_cnt = 0;
            StringTree tree;
            for (std::map<std::set<int>, size_t>::iterator
                    j = new_set.begin(); j != new_set.end(); ++j) {
                tree.insert(j->first);
            }
            while(!norm.eof()) {
                if (line == "%%%") {
                    delim_cnt++;
                } else if (delim_cnt == i) {
                    std::set<int> trans;
                    for (size_t j = 0; j < line.size(); ++j)
                        if (line[j] == '1')
                            trans.insert(j);
                    tree.find(trans, new_set);
                }
                std::getline(norm, line);
            }
            last_set.clear();
            for (std::map<std::set<int>, size_t >::iterator j =
                        new_set.begin(); j != new_set.end(); ++j)
                if (j->second >= supp_bound[i]) {
                    temp_support[j->first] = j->second;
                    last_set.insert(j->first);
                }
        }
        support[i] = temp_support;
    }
    for (int i = 0; i < 3; ++i) {
        if (i == 0) {
            std::cout << "Product rules" << std::endl;
        } else if (i == 1) {
            std::cout << "Category rules" << std::endl;
        } else if (i == 2) {
            std::cout << "Department rules" << std::endl;
        }
        for (std::map<std::set<int>, size_t>::iterator
                    j = support[i].begin(); j != support[i].end(); ++j)
            extractStep(i, j->first, std::set<int>(), support[i]);
        std::cout << "-------------------------" << std::endl;
    }
    delete normalizer;
}

void Solver::extractStep(size_t idx, std::set<int> left, std::set<int> right,
                        const std::map<std::set<int>, size_t> &support) {
    if (left.size() == 0)
        return;
    {
        std::set<int> uni = left;
        uni.insert(right.begin(), right.end());
        double conf = 1.1;
        if (right.size() > 0) {
            conf = (double)support.at(uni) / support.at(left);
            if (conf >= MIN_CONF) {
                for (std::set<int>::iterator i = left.begin();
                                i != left.end(); ++i)
                    std::cout << normalizer->id_to_name[idx][*i] << " ";
                std::cout << "=> ";
                for (std::set<int>::iterator i = right.begin();
                                i != right.end(); ++i)
                    std::cout << normalizer->id_to_name[idx][*i] << " ";
                std::cout << "( supp = " << support.at(uni) << ", "
                          << "conf = " << conf << ")" << std::endl;
            } else {
                return;
            }
        }
    }
    for (std::set<int>::iterator i = left.begin(); i != left.end(); ++i) {
        std::set<int> temp_left = left, temp_right = right;
        temp_left.erase(*i);
        temp_right.insert(*i);
        extractStep(idx, temp_left, temp_right, support);
    }
}
