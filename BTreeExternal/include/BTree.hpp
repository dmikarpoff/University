#ifndef INCLUDE_BTREE_H_
#define INCLUDE_BTREE_H_

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <vector>

#include "IExternalReader.h"

template <typename Entry>
struct BTreeNode {
    std::vector<Entry> val;
    std::vector<size_t> children;
    size_t sz;
};

template<typename Entry>
class ExteranlBTree {
 public:
    ExteranlBTree(size_t order):
            reader_(NULL),
            root_offset(-1),
            order_(order),
            depth_(0) {
        assert(order > 1);
    }
    void setReader(IExternalReader* reader);
    bool insert(Entry val);
    std::pair<size_t, bool> find(const Entry& val);
    bool erase(const Entry& val);

 private:
    bool writeNode(const size_t &offset, const BTreeNode<Entry> &node);
    bool readNode(const size_t offset, BTreeNode<Entry>* node);
    bool eraseNode(const size_t& offset);
    std::pair<size_t, BTreeNode<Entry> > splitNode(const size_t& cur_offset,
                                                   BTreeNode<Entry>& node,
                                                   const size_t& parent_offset,
                                                   BTreeNode<Entry>& parent);
    bool eraseStep(const Entry& val, size_t offset,
                   BTreeNode<Entry>& node, size_t cur_depth,
                   bool need_write_node);

    IExternalReader* reader_;
    size_t root_offset;
    size_t order_;
    size_t depth_;
};

template<typename Entry>
void ExteranlBTree<Entry>::setReader(IExternalReader* reader) {
    reader_ = reader;
}

template<typename Entry>
bool ExteranlBTree<Entry>::insert(Entry val) {
    if (reader_ == NULL)
        return false;
    if (root_offset == -1) {
        root_offset = reader_->getFreeBlock();
        BTreeNode<Entry> node;
        node.sz = 1;
        node.val.push_back(val);
        node.children.resize(2, -1);
        assert(writeNode(root_offset, node));
        ++depth_;
        return true;
    }
    size_t cur_offset = root_offset;
    size_t search_depth = 0;
    BTreeNode<Entry> parent;
    size_t parent_offset = -1;
    while (true) {
        BTreeNode<Entry> node;
        assert(readNode(cur_offset, &node));
        ++search_depth;
        size_t lbound = node.sz;
        for (size_t i = 0; i < node.sz; ++i) {
            if (node.val[i] == val)
                return false;
            if (node.val[i] > val) {
                lbound = i;
                break;
            }
        }
        bool need_write = false;
        if (node.sz == order_ - 1) {
            std::pair<size_t, BTreeNode<Entry> > added_node;
            size_t old_depth = depth_;
            added_node = splitNode(cur_offset, node, parent_offset, parent);
            if (old_depth != depth_) {
                search_depth++;
            }
            if (lbound > node.sz) {
                lbound -= node.sz + 1;
                std::swap(node, added_node.second);
                std::swap(cur_offset, added_node.first);
            }
            writeNode(added_node.first, added_node.second);
            need_write = true;
        }
        if (depth_ == search_depth) {
            node.val.insert(node.val.begin() + lbound, val);
            node.children.push_back(-1);
            node.sz++;
            writeNode(cur_offset, node);
            return true;
        }
        if (need_write)
            writeNode(cur_offset, node);
        parent_offset = cur_offset;
        cur_offset = node.children[lbound];
        parent = node;
    }
    return true;
}

template <typename Entry>
std::pair<size_t, bool> ExteranlBTree<Entry>::find(const Entry &val) {
    size_t cur_offset = root_offset;
    if (cur_offset == -1)
        return std::make_pair(-1, false);
    size_t search_depth = 0;
    while (true) {
        search_depth++;
        BTreeNode<Entry> node;
        readNode(cur_offset, &node);
        size_t lbound = node.sz;
        for (size_t i = 0; i < node.sz; ++i) {
            if (node.val[i] == val)
                return std::make_pair(cur_offset, true);
            if (val < node.val[i]) {
                lbound = i;
                break;
            }
        }
        if (search_depth == depth_)
            return std::make_pair(-1, false);
        cur_offset = node.children[lbound];
    }
}

template <typename Entry>
bool ExteranlBTree<Entry>::erase(const Entry &val) {
    if (root_offset == -1)
        return false;
    BTreeNode<Entry> node;
    readNode(root_offset, &node);
    return eraseStep(val, root_offset, node, 1, false);
}

template<typename Entry>
bool ExteranlBTree<Entry>::writeNode(const size_t& offset,
                                     const BTreeNode<Entry> &node) {
    char* buf = (char*)malloc(sizeof(Entry) * (order_ - 1) +
                              (order_ + 1) * sizeof(size_t));
    memcpy(buf, &node.sz, sizeof(size_t));
    memcpy(buf + sizeof(size_t),
           &(node.val[0]), node.sz * sizeof(Entry));
    memcpy(buf + (order_ - 1) * sizeof(Entry) + sizeof(size_t),
            &(node.children[0]), sizeof(size_t) * (node.sz + 1));
    bool res = reader_->writeBlock(offset, buf);
    delete buf;
    return res;
}

template<typename Entry>
bool ExteranlBTree<Entry>::readNode(const size_t offset,
                                    BTreeNode<Entry> *node) {
    if (node == NULL)
        return false;
    char *in;
    if (!reader_->readBlock(offset, &in))
        return false;
    memcpy(&(node->sz), in, sizeof(size_t));
    node->val.resize(node->sz);
    node->children.resize(node->sz + 1);
    memcpy(&(node->val[0]), in + sizeof(size_t),
            node->sz * sizeof(Entry));
    memcpy(&(node->children[0]),
            in + sizeof(size_t) + (order_ - 1) * sizeof(Entry),
            sizeof(size_t) * (node->sz + 1));
    delete in;
    return true;
}

template <typename Entry>
std::pair<size_t, BTreeNode<Entry> > ExteranlBTree<Entry>::splitNode(
                                        const size_t& cur_offset,
                                        BTreeNode<Entry>& node,
                                        const size_t& parent_offset,
                                        BTreeNode<Entry>& parent) {
    size_t rch_pos;
    BTreeNode<Entry> rch;
    size_t mid = (order_ - 1) / 2;
    for (size_t i = mid + 1; i < node.sz; ++i)
        rch.val.push_back(node.val[i]);
    for (size_t i = mid + 1; i <= node.sz; ++i)
        rch.children.push_back(node.children[i]);
    rch.sz = rch.val.size();
    node.sz = node.val.size() - rch.val.size() - 1;
    Entry mid_val = node.val[mid];
    node.val.resize(node.sz);
    node.children.resize(node.sz + 1);
    if (parent.val.size() == 0) {
        size_t nroot_pos = reader_->getFreeBlock();
        rch_pos = reader_->getFreeBlock();
        BTreeNode<Entry> root;
        root.val.push_back(mid_val);
        root.children.push_back(cur_offset);
        root.children.push_back(rch_pos);
        root.sz = 1;
        writeNode(nroot_pos, root);
        ++depth_;
        root_offset = nroot_pos;
        return std::make_pair(rch_pos, rch);
    }
    size_t ins_pos = parent.sz;
    for (size_t i = 0; i < parent.sz; ++i) {
        if (parent.val[i] > mid_val) {
            ins_pos = i;
            break;
        }
    }
    rch_pos = reader_->getFreeBlock();
    parent.val.insert(parent.val.begin() + ins_pos, mid_val);
    parent.children.insert(parent.children.begin() + ins_pos + 1, rch_pos);
    assert(parent.children[ins_pos] == cur_offset);
    ++parent.sz;
    writeNode(parent_offset, parent);
    return std::make_pair(rch_pos, rch);
}

template <typename Entry>
bool ExteranlBTree<Entry>::eraseStep(const Entry& val, size_t offset,
                    BTreeNode<Entry> &node, size_t cur_depth,
                    bool need_write_node) {
    size_t lbound = node.sz;
    for (size_t i = 0; i < node.sz; ++i)
        if (node.val[i] >= val) {
            lbound = i;
            break;
        }
    if (lbound != node.sz && node.val[lbound] == val) {
        if (depth_ == cur_depth) {
            node.val.erase(node.val.begin() + lbound);
            node.children.pop_back();
            node.sz--;
            writeNode(offset, node);
            return true;
        }
        BTreeNode<Entry> lch;
        size_t lch_pos = node.children[lbound];
        readNode(node.children[lbound], &lch);
        if (lch.sz > order_ / 2 - 1) {
            size_t temp_depth = cur_depth + 1;
            BTreeNode<Entry> search_node = lch;
            size_t search_offset = node.children[lbound];
            while (temp_depth != depth_) {
                search_offset = search_node.children[search_node.sz];
                readNode(search_offset, &search_node);
                ++temp_depth;
            }
            node.val[lbound] =
                      search_node.val[search_node.sz - 1];
            writeNode(offset, node);
            return eraseStep(node.val[lbound], node.children[lbound],
                             lch, cur_depth + 1, false);
        }
        BTreeNode<Entry> rch;
        readNode(node.children[lbound + 1], &rch);
        if (rch.sz > order_ / 2 - 1) {
            size_t temp_depth = cur_depth + 1;
            BTreeNode<Entry> search_node = rch;
            size_t search_offset = node.children[lbound + 1];
            while (temp_depth != depth_) {
                search_offset = search_node.children[0];
                readNode(search_offset, &search_node);
                ++temp_depth;
            }
            node.val[lbound] = search_node.val[0];
            writeNode(offset, node);
            return eraseStep(node.val[lbound], node.children[lbound + 1],
                             rch, cur_depth + 1, false);
        }
        lch.val.push_back(val);
        lch.val.insert(lch.val.end(), rch.val.begin(), rch.val.end());
        lch.children.insert(lch.children.end(), rch.children.begin(),
                            rch.children.end());
        lch.sz += rch.sz + 1;
        size_t rch_pos = node.children[lbound + 1];
        node.val.erase(node.val.begin() + lbound);
        node.children.erase(node.children.begin() + lbound + 1);
        --node.sz;
        if (node.sz == 0) {
            reader_->deleteBlock(offset);
            root_offset = lch_pos;
            reader_->deleteBlock(rch_pos);
            --depth_;
            return eraseStep(val, lch_pos, lch, cur_depth, true);
        }
        reader_->deleteBlock(rch_pos);
        writeNode(offset, node);
        return eraseStep(val, lch_pos, lch, cur_depth + 1, true);
    }
    if (cur_depth == depth_) {
        if (need_write_node)
            writeNode(offset, node);
        return false;
    }
    BTreeNode<Entry> next;
    readNode(node.children[lbound], &next);
    if (next.sz > (order_ - 1) / 2) {
        if (need_write_node)
            writeNode(offset, node);
        return eraseStep(val, node.children[lbound], next, cur_depth + 1, false);
    }
    size_t left_offset = -1, right_offset = -1;
    BTreeNode<Entry> left, right;
    if (lbound > 0) {
        left_offset = node.children[lbound - 1];
        readNode(left_offset, &left);
        if (left.sz > (order_ - 1) / 2) {
            next.val.insert(next.val.begin(), node.val[lbound - 1]);
            next.children.insert(next.children.begin(),
                                 left.children.back());
            node.val[lbound - 1] = left.val.back();
            left.val.pop_back();
            left.children.pop_back();
            left.sz--;
            next.sz++;
            writeNode(left_offset, left);
            writeNode(offset, node);
            return eraseStep(val, node.children[lbound], next, cur_depth + 1, true);
        }
    }
    if (lbound < node.sz) {
        right_offset = node.children[lbound + 1];
        readNode(right_offset, &right);
        if (right.sz > (order_ - 1) / 2) {
            next.val.push_back(node.val[lbound]);
            next.children.push_back(right.children.front());
            node.val[lbound] = right.val[0];
            right.val.erase(right.val.begin());
            right.children.erase(right.children.begin());
            right.sz--;
            next.sz++;
            writeNode(right_offset, right);
            writeNode(offset, node);
            return eraseStep(val, node.children[lbound], next, cur_depth + 1, true);
        }
    }
    if (lbound > 0) {
        size_t ch_offset = node.children[lbound];
        left.val.push_back(node.val[lbound - 1]);
        left.val.insert(left.val.end(), next.val.begin(), next.val.end());
        left.children.insert(left.children.end(), next.children.begin(),
                             next.children.end());
        left.sz += (1 + next.sz);
        node.val.erase(node.val.begin() + lbound - 1);
        node.children.erase(node.children.begin() + lbound);
        node.sz--;
        reader_->deleteBlock(ch_offset);
        if (node.sz == 0) {
            root_offset = left_offset;
            reader_->deleteBlock(offset);
            --depth_;
            return eraseStep(val, left_offset, left, cur_depth, true);
        }
        writeNode(offset, node);
        return eraseStep(val, left_offset, left, cur_depth + 1, true);
    }
    next.val.push_back(node.val[lbound]);
    next.val.insert(next.val.end(), right.val.begin(), right.val.end());
    next.children.insert(next.children.end(), right.children.begin(),
                         right.children.end());
    next.sz += (1 + right.sz);
    node.val.erase(node.val.begin() + lbound);
    node.children.erase(node.children.begin() + lbound + 1);
    node.sz--;
    reader_->deleteBlock(right_offset);
    if (node.sz == 0) {
        root_offset = node.children[lbound + 1];
        reader_->deleteBlock(offset);
        --depth_;
        return eraseStep(val, root_offset, next, cur_depth, true);
    }
    writeNode(offset, node);
    return eraseStep(val, node.children[lbound], next, cur_depth + 1, true);
}

#endif  // INCLUDE_BTREE_H_
