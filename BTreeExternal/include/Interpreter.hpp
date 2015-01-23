#ifndef INCLUDE_INTERPRETER_H_
#define INCLUDE_INTERPRETER_H_

#include <iostream>
#include <set>
#include <sstream>

#include "BTree.hpp"
#include "ExternalReaderDammy.h"

template <typename Entry>
class Interpreter {
 public:
    Interpreter(std::istream& in, std::ostream& out);
    void run();
    ~Interpreter();
 private:
    std::istream& in_;
    std::ostream& out_;
    ExteranlBTree<Entry>* tree_;
    Reader* reader_;
    std::set<Entry> sim_tree_;
};

template <typename Entry>
Interpreter<Entry>::Interpreter(std::istream& in, std::ostream& out):
    in_(in), out_(out), tree_(NULL), reader_(NULL) {}

template <typename Entry>
void Interpreter<Entry>::run() {
    out_ << "Input tree order:" << std::endl;
    size_t order = -1;
    in_ >> order;
    if (order % 2 == 1 || order < 6 || order == -1) {
        out_ << "Illegal tree order. Order should be even integer greater then 4"
             << std::endl;
        return;
    }
    tree_ = new ExteranlBTree<Entry>(order);
    size_t block_size = (order - 1) * sizeof(Entry) + (order + 1) * sizeof(size_t);
    reader_ = new Reader(block_size);
    tree_->setReader(reader_);
    char temp[100];
    in_.getline(temp, 100);
    while (!in_.eof()) {
        char buf[100];
        in_.getline(buf, 100);
        std::string line(buf);
        if (line == "exit")
            return;
        std::stringstream ss;
        ss.str(line);
        char c;
        int val;
        ss >> c >> val;
        if (c == 'i') {
            reader_->dropCounter();
            bool res = tree_->insert(val);
            assert(res == sim_tree_.insert(val).second);
            out_ << (res ? "true" : "false") << " " << reader_->getCounter() <<
                    std::endl;
            continue;
        }
        if (c == 'f') {
            reader_->dropCounter();
            std::pair<size_t, bool> res = tree_->find(val);
            assert(res.second == (sim_tree_.find(val) != sim_tree_.end()));
            out_ << (res.second ? "true" : "false") << " " << reader_->getCounter() <<
                    std::endl;
            continue;
        }
        if (c == 'd') {
            reader_->dropCounter();
            bool res = tree_->erase(val);
            assert(res == (sim_tree_.erase(val) == 1));
            out_ << (res ? "true" : "false") << " " << reader_->getCounter() <<
                    std::endl;
            continue;
        }
        out_ << "Illegal format. Use:\n" <<
                "i <value> to insert\n" << "f <value> to find\n" <<
                "d <value> to delete\n" << "exit to stop" << std::endl;
    }
}

template <typename Entry>
Interpreter<Entry>::~Interpreter() {
    if (tree_ != NULL)
        delete tree_;
    if (reader_ != NULL)
        delete reader_;
}

#endif  // INCLUDE_INTERPRETER_H_
