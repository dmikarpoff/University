#ifndef INCLUDE_REGRESSIONSOLVER_H_
#define INCLUDE_REGRESSIONSOLVER_H_

#include <IClassifier.h>
#include <boost/numeric/ublas/matrix.hpp>

namespace regression {

typedef std::pair<int, int> pair_ii;

class RegressionSolver : public IClassifier<pair_ii, double> {
 public:
    RegressionSolver() {}
    bool train(const std::vector<std::pair<pair_ii, double> > &s);
    bool save(std::ofstream &out);
    bool load(std::ifstream &in);
    double classify(const pair_ii &item);
    ~RegressionSolver() {}
 private:
    boost::numeric::ublas::matrix<double> solution;
};

}  // namespace regression

#endif  // INCLUDE_REGRESSIONSOLVER_H_
