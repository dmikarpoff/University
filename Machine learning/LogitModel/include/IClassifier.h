#ifndef INCLUDE_ICLASSIFIER_H_
#define INCLUDE_ICLASSIFIER_H_

#include <fstream>
#include <vector>

template <class Item, class Label>
class IProbClassifier {
 public:
    virtual bool train(const std::vector<std::pair<Item, Label> >& s) = 0;
    virtual bool load(std::ifstream& in) = 0;
    virtual bool save(std::ofstream& out) = 0;
    virtual std::pair<Label, double> classify(const Item& item) = 0;
    virtual ~IProbClassifier() {}
};

#endif  // INCLUDE_ICLASSIFIER_H_
