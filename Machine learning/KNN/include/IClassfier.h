#ifndef INCLUDE_ICLASSIFIER_H_
#define INCLUDE_ICLASSIFIER_H_

#include <fstream>
#include <vector>

template <class Item, class Label>
class IClassifier {
 public:
    virtual bool train(const std::vector<std::pair<Item, Label> >& s) = 0;
    virtual bool load(std::ifstream& in) = 0;
    virtual bool save(std::ofstream& out) = 0;
    virtual Label classify(const Item& item) = 0;
    virtual ~IClassifier() {}
};

#endif  // INCLUDE_ICLASSIFIER_H_
