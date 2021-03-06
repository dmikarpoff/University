#include "BTree.hpp"
#include "ExternalReaderDammy.h"
#include "Interpreter.hpp"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <ctime>
#include <iostream>
#include <random>
#include <set>

#define MAX_KEY_VAL_TEST 1000000
#define REPEAT_RANDOM 25

void showHint() {
    std::cout << "Illegal function parameters" << std::endl;
    std::cout << "Use:" << std::endl;
    std::cout << "--insert to insert test" << std::endl;
    std::cout << "--find to find test" << std::endl;
    std::cout << "--delete to erase test" << std::endl;
    std::cout << "--interactive for interactive mode in console" << std::endl;
    std::cout << "--interactive <file name> for loading test from file" << std::endl;
}

void insertRandomTest() {
    float total_cost[MAX_KEY_VAL_TEST + 1];
    float amount[MAX_KEY_VAL_TEST + 1];
    for (size_t dim = 6; dim <= 16; dim += 2) {
        std::cout << "Order = " << dim << std::endl;
        for (size_t i = 0; i < MAX_KEY_VAL_TEST; ++i) {
            total_cost[i] = 0;
            amount[i] = 0;
        }
        for (size_t o = 0; o < REPEAT_RANDOM; ++o) {
            std::cout << "\tRandom iteration No " << o << std::endl;
            ExteranlBTree<int> tree(dim);
            Reader reader(sizeof(int) * (dim - 1) +
                          (dim + 1) * sizeof(size_t));
            tree.setReader(&reader);
            std::set<int> tree_sim;
            for (size_t i = 0; i < MAX_KEY_VAL_TEST; ++i)
                tree_sim.insert(i);
            for (int i = 0; i < 2 * MAX_KEY_VAL_TEST; ++i) {
                int val = std::abs(rand() % (MAX_KEY_VAL_TEST));
                bool is_in = tree_sim.find(val) == tree_sim.end();
                reader.dropCounter();
                assert(!is_in == tree.insert(val));
                size_t cost = reader.getCounter();
                {
                    size_t sz = MAX_KEY_VAL_TEST - tree_sim.size();
                    total_cost[sz] += cost;
                    amount[sz] += 1.0;
                    tree_sim.erase(val);
                }
            }
            size_t sz = MAX_KEY_VAL_TEST - tree_sim.size();
            for (std::set<int>::iterator i = tree_sim.begin();
                        i != tree_sim.end(); ++i) {
                reader.dropCounter();
                assert(tree.insert(*i));
                size_t cost = reader.getCounter();
                total_cost[sz] += cost;
                amount[sz] += 1.0;
                ++sz;
            }
        }
        std::fstream out(std::to_string(dim) + ".csv",
                         std::ios_base::out);
        for (size_t i = 0; i < MAX_KEY_VAL_TEST; ++i) {
            out << i << ";";
            if (amount[i] > 0)
                out << (float)total_cost[i] / amount[i];
            else
                out << "--";
            out << "\n";
        }

        std::cout << "Order " << dim << " is OK!" << std::endl;
    }
}

void findRandomTest() {
    float total_cost[MAX_KEY_VAL_TEST + 1];
    float amount[MAX_KEY_VAL_TEST + 1];
    for (size_t dim = 12; dim <= 16; dim += 2) {
        for (size_t i = 0; i < MAX_KEY_VAL_TEST; ++i) {
            total_cost[i] = 0;
            amount[i] = 0;
        }
        std::cout << "Tree order is " << dim << std::endl;
        for (size_t o = 0; o < REPEAT_RANDOM; ++o) {
            ExteranlBTree<int> tree(dim);
            Reader reader(sizeof(int) * (dim - 1) +
                          (dim + 2) * sizeof(size_t));
            tree.setReader(&reader);
            std::set<int> tree_sim;
            std::cout << "\tRandom step = " << o << std::endl;
            for (int i = 0; i < MAX_KEY_VAL_TEST; ++i) {
                int val = std::abs(rand() % (MAX_KEY_VAL_TEST));
                tree.insert(val);
                tree_sim.insert(val);\
                for (int j = 0; j < sqrt(MAX_KEY_VAL_TEST); ++j) {
                    int sval = rand() % (i + 1);
                    reader.dropCounter();
                    bool btree_in = tree.find(sval).second;
                    bool set_in = tree_sim.find(sval) != tree_sim.end();
                    assert(set_in == btree_in);
                    total_cost[tree_sim.size()] += reader.getCounter();
                    amount[tree_sim.size()] += 1.0;
                }
            }
        }
        std::fstream out(std::to_string(dim) + ".csv",
                         std::ios_base::out);
        for (size_t i = 0; i < MAX_KEY_VAL_TEST; ++i) {
            out << i << ";";
            if (amount[i] > 0)
                out << (float)total_cost[i] / amount[i];
            else
                out << "--";
            out << "\n";
        }

        std::cout << "Order " << dim << " is OK!" << std::endl;
    }
}

void deleteRandomTest() {
    size_t *total_cost = new size_t[MAX_KEY_VAL_TEST + 1];
    size_t *amount = new size_t[MAX_KEY_VAL_TEST + 1];
    for (size_t dim = 6; dim <= 16; dim += 2) {
        for (size_t i = 0; i < MAX_KEY_VAL_TEST + 1; ++i) {
            total_cost[i] = 0;
            amount[i] = 0;
        }
        std::cout << "Order = " << dim << std::endl;
        for (size_t o = 0; o < REPEAT_RANDOM; ++o) {
            ExteranlBTree<int> tree(dim);
            Reader reader(sizeof(int) * (dim - 1) +
                          (dim + 2) * sizeof(size_t));
            tree.setReader(&reader);
            std::set<int> tree_sim;
            std::cout << "\tRandom = " << o << std::endl;
            for (size_t i = 0; i < MAX_KEY_VAL_TEST; ++i) {
                tree.insert(i);
                tree_sim.insert(i);
            }
            for (int i = 0; i < MAX_KEY_VAL_TEST; ++i) {
                int val = std::abs(rand() % (MAX_KEY_VAL_TEST));
                reader.dropCounter();
                bool del_res = tree.erase(val);
                assert(del_res == (tree_sim.erase(val) == 1));
                total_cost[tree_sim.size() + 1] += reader.getCounter();
                amount[tree_sim.size() + 1]++;
            }
            std::set<int>::iterator i = tree_sim.begin();
            while (tree_sim.size() > 0) {
                reader.dropCounter();
                bool del_res = tree.erase(*i);
                assert(del_res == (tree_sim.erase(*i) == 1));
                total_cost[tree_sim.size() + 1] += reader.getCounter();
                amount[tree_sim.size() + 1]++;
                i = tree_sim.begin();
            }
        }
        std::fstream out(std::to_string(dim) + ".csv",
                         std::ios_base::out);
        for (size_t i = 0; i < MAX_KEY_VAL_TEST + 1; ++i) {
            out << i << ";";
            if (amount[i] > 0)
                out << (float)total_cost[i] / amount[i];
            else
                out << "--";
            out << "\n";
        }

        std::cout << "Order " << dim << " is OK!" << std::endl;
    }
    delete[] total_cost;
    delete[] amount;
}

void insertTest() {
    size_t total_cost[MAX_KEY_VAL_TEST];
    for (size_t dim = 6; dim <= 16; dim += 2) {
        int values[MAX_KEY_VAL_TEST];
        for (size_t i = 0; i < MAX_KEY_VAL_TEST + 1; ++i) {
            total_cost[i] = 0;
            values[i] = i;
        }
        ExteranlBTree<int> tree(dim);
        Reader reader(sizeof(int) * (dim - 1) +
                      (dim + 2) * sizeof(size_t));
        tree.setReader(&reader);
        std::set<int> tree_sim;
        std::random_shuffle(values, values + MAX_KEY_VAL_TEST);
        for (int i = 0; i < MAX_KEY_VAL_TEST; ++i) {
            if (i % 100 == 0)
                std::cout << "\tvalue " << i << std::endl;
            int val = values[i];
            reader.dropCounter();
            bool del_res = tree.insert(val);
            assert(del_res == (tree_sim.insert(val).second == 1));
            total_cost[tree_sim.size()] += reader.getCounter();
        }
        std::fstream out(std::to_string(dim) + ".csv",
                         std::ios_base::out);
        for (size_t i = 0; i < MAX_KEY_VAL_TEST + 1; ++i) {
            out << i << ";";
            out << (float)total_cost[i];
            out << "\n";
        }
        std::cout << "Order " << dim << " is OK!" << std::endl;
    }
}

void deleteTest() {
    size_t total_cost[MAX_KEY_VAL_TEST + 1];
    for (size_t dim = 6; dim <= 16; dim += 2) {
        int values[MAX_KEY_VAL_TEST];
        ExteranlBTree<int> tree(dim);
        Reader reader(sizeof(int) * (dim - 1) +
                      (dim + 2) * sizeof(size_t));
        tree.setReader(&reader);
        std::set<int> tree_sim;
        for (size_t i = 0; i < MAX_KEY_VAL_TEST + 1; ++i) {
            total_cost[i] = 0;
            values[i] = i;
            tree.insert(i);
            tree_sim.insert(i);
        }
        std::random_shuffle(values, values + MAX_KEY_VAL_TEST);
        for (int i = 0; i < MAX_KEY_VAL_TEST; ++i) {
            if (i % 100 == 0)
                std::cout << "\tvalue " << i << std::endl;
            int val = values[i];
            reader.dropCounter();
            bool del_res = tree.erase(val);
            assert(del_res);
            total_cost[tree_sim.size()] += reader.getCounter();
            tree_sim.erase(i);
        }
        std::fstream out(std::to_string(dim) + ".csv",
                         std::ios_base::out);
        for (size_t i = 0; i < MAX_KEY_VAL_TEST + 1; ++i) {
            out << i << ";";
            out << (float)total_cost[i];
            out << "\n";
        }
        std::cout << "Order " << dim << " is OK!" << std::endl;
    }
}

int main(int argc, char** argv) {
//    srand(time(NULL));
    std::fstream out("tree.dat", std::ios_base::out);
    out.close();
    if (argc == 2) {
        if (strcmp(argv[1], "--insert") == 0) {
            insertRandomTest();
            return 0;
        }
        if (strcmp(argv[1], "--find") == 0) {
            findRandomTest();
            return 0;
        }
        if (strcmp(argv[1], "--delete") == 0) {
            deleteRandomTest();
            return 0;
        }
        if (strcmp(argv[1], "--insert1") == 0) {
            insertTest();
            return 0;
        }
        if (strcmp(argv[1], "--delete1") == 0) {
            deleteTest();
            return 0;
        }
        if (strcmp(argv[1], "--interactive") == 0) {
            Interpreter<int> inter(std::cin, std::cout);
            inter.run();
            return 0;
        }
    }
    if (argc == 3) {
        if (strcmp(argv[1], "--interactive") == 0) {
            std::fstream in(argv[2]);
            if (!in.is_open()) {
                std::cout << "Unable to open file" << std::endl;
                return 0;
            }
            Interpreter<int> inter(in, std::cout);
            inter.run();
            return 0;
        }
    }
    showHint();
    return 0;
}
