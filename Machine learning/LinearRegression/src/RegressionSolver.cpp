#include <RegressionSolver.h>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/lu.hpp>

namespace regression {

bool RegressionSolver::train(
        const std::vector<std::pair<pair_ii, double> > &s) {
    using namespace boost::numeric::ublas;
    matrix<double> fi(s.size(), 2);
    matrix<double> y(s.size(), 1);
    for (size_t i = 0; i < s.size(); ++i) {
        fi(i, 0) = s[i].first.first;
        fi(i, 1) = s[i].first.second;
        y(i, 0) = s[i].second;
    }
    matrix<double> to_inv = prod(trans(fi), fi);
    matrix<double> inv(to_inv.size1(), to_inv.size2());
    permutation_matrix<size_t> pmat(to_inv.size1());
    int res = lu_factorize(to_inv, pmat);
    if (res != 0)
        return false;
    inv.assign(identity_matrix<double>(to_inv.size1()));
    lu_substitute(to_inv, pmat, inv);
    solution = prod(inv, trans(fi));
    solution = prod(solution, y);
    std::cout << solution << std::endl;
    return true;
}

bool RegressionSolver::save(std::ofstream &out) {
    out << solution;
}

bool RegressionSolver::load(std::ifstream &in) {
    in >> solution;
}

double RegressionSolver::classify(const pair_ii &item) {
    boost::numeric::ublas::matrix<double> mitem(1, 2);
    mitem(0, 0) = item.first;
    mitem(0, 1) = item.second;
    return prod(mitem, solution)(0, 0);
}

}  // namespace regression
