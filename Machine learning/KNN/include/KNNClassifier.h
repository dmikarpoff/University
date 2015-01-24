#ifndef INCLUDE_KNNCLASSIFIER_H_
#define INCLUDE_KNNCLASSIFIER_H_

#include <IClassfier.h>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/index/rtree.hpp>
#include <map>

namespace KNN {

typedef boost::geometry::model::d2::point_xy<double> point2d;

class KNNClassifier : public IClassifier<point2d, int> {
 public:
    KNNClassifier(size_t k = 5) {
        knn = k;
    }
    virtual bool train(const std::vector<std::pair<point2d, int> >&
                       samples);
    virtual bool load(std::ifstream &in) {
        return true;
    }
    virtual bool save(std::ofstream &out) {
        return true;
    }
    virtual int classify(const point2d &item);
    virtual ~KNNClassifier() {}
 private:
    size_t knn;
    boost::geometry::index::rtree<std::pair<point2d, int>,
                    boost::geometry::index::quadratic<16> > tree;
};

}  // namespace KNN

#endif  // INCLUDE_KNNCLASSIFIER_H_
