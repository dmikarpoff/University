#include <LogitSolver.h>
#include <limits>
#include <iostream>

namespace logit {

LogitSolver::LogitSolver() {
    solution = cv::Mat(2, 1, CV_64F);
    solution.at<double>(0, 0) = std::numeric_limits<double>::quiet_NaN();
}

bool LogitSolver::train(const std::vector<std::pair<pair_dd, bool> > &s) {
    cv::Mat X(s.size(), 2, CV_64F);
    cv::Mat Y(s.size(), 1, CV_64F);
    for (size_t i = 0; i < s.size(); ++i) {
        X.at<double>(i, 0) = s[i].first.first;
        X.at<double>(i, 1) = s[i].first.second;
        Y.at<double>(i, 0) = static_cast<double>(s[i].second);
    }
    cv::Mat W = cv::Mat::ones(2, 1, CV_64F);
    cv::Mat updW = cv::Mat::zeros(2, 1, CV_64F);
    int counter = 0;
    double prev_error = cv::norm(Y, flogit(X * updW));
    double a = 0.1;
    int succ = -1;
    while (counter < 10000 && cv::norm(W , updW) > 1E-6) {
        updW = W + a * X.t() * (Y - flogit(X * W));
//        double error = cv::norm(Y - flogit(X * updW));
//        if (error < prev_error) {
//            succ = (succ + 1) % 5;
//            if (succ == 4)
//                a *= 2.0;
//        } else {
//            a /= 2.0;
//        }
//        prev_error = error;
        std::swap(W, updW);
    }
    if (counter == 10000) {
        solution.at<double>(0, 0) = std::numeric_limits<double>::quiet_NaN();
        return false;
    }
    W.copyTo(solution);
//    std::cout << "Y: " << Y.rows << " " << Y.cols << std::endl;
//    std::cout << "X: " << X.rows << " " << X.cols << std::endl;
//    std::cout << "S: " << solution.rows << " " << solution.cols << std::endl;
//    cv::Mat m = flogit(X * solution);
//    std::cout << "m: " << m.rows << " " << m.cols << std::endl;
    std::cout << "Average error = " <<
        cv::norm(Y, flogit(X * solution), cv::NORM_L1) / s.size() << std::endl;
    std::cout << "Max error = " <<
                 cv::norm(Y, flogit(X * solution), cv::NORM_INF) << std::endl;
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
    cv::Mat x(2, 1, CV_64F);
    x.at<double>(0, 0) = item.first;
    x.at<double>(1, 0) = item.second;
    cv::Mat mres = flogit(solution.t() * x);
    return std::make_pair(true, mres.at<double>(0, 0));
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
