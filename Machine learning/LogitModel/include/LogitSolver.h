#ifndef INCLUDE_LOGITSOLVER_H_
#define INCLUDE_LOGITSOLVER_H_

#include <IClassifier.h>
#include <opencv2/core/core.hpp>
#include <utility>

namespace logit {

typedef std::pair<double, double> pair_dd;
typedef std::pair<bool, double> pair_bd;

class LogitSolver : public IProbClassifier<pair_dd, bool> {
 public:
    LogitSolver();
    virtual bool train(const std::vector<std::pair<pair_dd, bool> > &s);
    bool load(std::ifstream &in);
    bool save(std::ofstream &out);
    pair_bd classify(const pair_dd &item);
    virtual ~LogitSolver();
 private:
    cv::Mat solution;

    static cv::Mat flogit(const cv::Mat& x);
};

}  // namespace logit

#endif  // INCLUDE_LOGITSOLVER_H_
