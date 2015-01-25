#include <KNNClassifier.h>

namespace KNN {

bool KNNClassifier::train(
        const std::vector<std::pair<point2d, int> > &samples) {
    for (size_t i = 0; i < samples.size(); ++i) {
        tree.insert(samples[i]);
    }
    return true;
}

int KNNClassifier::classify(const point2d &item) {
    std::vector<std::pair<point2d, int> > res;
    tree.query(boost::geometry::index::nearest(item, knn),
               std::back_inserter(res));
    size_t zero = 0;
    for (size_t i = 0; i < res.size(); ++i)
        if (res[i].second == 0)
            ++zero;
    if (2 * zero > knn)
        return 0;
    return 1;
}

}  // namespace KNN
