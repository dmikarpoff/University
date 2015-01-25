#include <LogitSolver.h>
#include <limits>
#include <iostream>

namespace logit {

LogitSolver::LogitSolver() {
    solution = cv::Mat(3, 1, CV_64F);
    solution.at<double>(0, 0) = std::numeric_limits<double>::quiet_NaN();
}

bool LogitSolver::train(const std::vector<std::pair<pair_dd, bool> > &s) {
    cv::Mat X(s.size(), 3, CV_64F);
    cv::Mat Y(s.size(), 1, CV_64F);
    for (size_t i = 0; i < s.size(); ++i) {
        X.at<double>(i, 0) = s[i].first.first;
        X.at<double>(i, 1) = s[i].first.second;
        X.at<double>(i, 2) = 1.0;
        Y.at<double>(i, 0) = static_cast<double>(s[i].second);
    }
    cv::Mat W = cv::Mat::zeros(3, 1, CV_64F);
    cv::Mat updW = cv::Mat::ones(3, 1, CV_64F);
    int counter = 0;
    double a = 0.01;
    while (counter < 10000 && cv::norm(W , updW) > 1E-8) {
        updW = W + a * X.t() * (Y - flogit(X * W));
        std::swap(W, updW);
        ++counter;
    }
    if (counter == 10000) {
        solution.at<double>(0, 0) = std::numeric_limits<double>::quiet_NaN();
        return false;
    }
    W.copyTo(solution);
    return true;
}

bool LogitSolver::load(std::ifstream &in) {
    in >> solution.at<double>(0, 0) >> solution.at<double>(1, 0);
    if (in.fail() || in.bad())
        return false;
    return true;
}

bool LogitSolver::save(std::ofstream &out) {
    if (solution.at<double>(0, 0) !=
            solution.at<double>(0, 0))
        return false;
    out << solution.at<double>(0, 0) << " " << solution.at<double>(1, 0);
    if (out.bad() || out.fail())
        return false;
    return true;
}

pair_bd LogitSolver::classify(const pair_dd &item) {
    cv::Mat x(3, 1, CV_64F);
    x.at<double>(0, 0) = item.first;
    x.at<double>(1, 0) = item.second;
    x.at<double>(2, 0) = 1.0;
    cv::Mat mres = flogit(solution.t() * x);
    bool res = mres.at<double>(0, 0) > 0.5 ? 1.0 : 0.0;
    return std::make_pair(res, mres.at<double>(0, 0));
}

LogitSolver::~LogitSolver() { }

cv::Mat LogitSolver::flogit(const cv::Mat &x) {
    cv::Mat res = x.clone();
    res = -res;
    cv::exp(res, res);
    cv::add(res, 1, res);
    cv::divide(1.0, res, res);
    return res;
}

}  // namespace logit
