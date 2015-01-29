#include <fstream>
#include <iostream>
#include <stdint.h>

#include <opencv2/core/core.hpp>

template <class T>
void reverse(T* x) {
    unsigned char* ptr = reinterpret_cast<uchar*>(x);
    std::reverse(ptr, ptr + sizeof(T));
}

int main(int argc, char** argv) {
    std::ifstream data_train("../data/train-images.idx3-ubyte", std::ios_base::binary);
    std::ifstream label_train("../data/train-labels.idx1-ubyte", std::ios_base::binary);
    int32_t n;
    int32_t width, height;
    label_train.read((char*)&n, 4);
    label_train.read((char*)&n, 4);
    reverse(&n);
    data_train.read((char*)&width, 4);
    data_train.read((char*)&n, 4);
    reverse(&n);
    data_train.read((char*)&width, 4);
    reverse(&width);
    data_train.read((char*)&height, 4);
    reverse(&height);
    cv::Mat Y = cv::Mat::ones(10, n, CV_64F);
    Y = -Y;
    cv::Mat X(width * height + 1, n, CV_64F);
    uchar *labels = (uchar*)malloc(n);
    label_train.read((char*)labels, n);
    uchar* pixels = (uchar*)malloc(width * height * n);
    data_train.read((char*)pixels, width * height * n);
    for (int i = 0; i < n; ++i) {
        reverse(labels + i);
        Y.at<double>(labels[i], i) = 1.0;
        for (int j = 0; j < width * height; ++j) {
            reverse(pixels + width * height * i + j);
            X.at<double>(j, i) =
                    ((double)pixels[width * height * i + j] - 128.0) / 128.0;
            assert(std::abs(X.at<double>(j, i)) <= 1.0);
        }
        X.at<double>(width * height, i) = 1.0;
    }
    free(pixels);
    free(labels);
    cv::Mat teta = cv::Mat::zeros(width * height + 1, 10, CV_64F);
    cv::Mat best_teta = teta.clone();
    size_t best_error = 3 *n;
    size_t counter = 0;
    while (counter < 4) {
        for (size_t i = 0; i < n; ++ i) {
            cv::Mat Y_star = teta.t() * X.col(i);
            Y_star = (Y_star > 0) & 1;
            Y_star.convertTo(Y_star, CV_64F, 2.0, -1.0);
            teta += 0.5 * X.col(i) * (Y.col(i) - Y_star).t();
        }
        cv::Mat res = teta.t() * X;
        size_t error = 0;
        for (int j = 0; j < n; ++j) {
            double val_max = -1E+9;
            int dig = -1;
            for (int i = 0; i < 10; ++i)
                if (res.at<double>(i, j) > val_max) {
                    val_max = res.at<double>(i, j);
                    dig = i;
                }
            if (dig == -1 || Y.at<double>(dig, j) < 0.0)
                error++;
        }
        std::cout << "error on " << error << std::endl;
        if (error < best_error) {
            best_error = error;
            std::cout << "update..." << std::endl;
            best_teta = teta.clone();
            counter = 0;
        } else {
            counter++;
        }
    }
    std::cout << "Internal error rate = " << (double)best_error / n << std::endl;


    //  ---------------------------------------------------------
    std::ifstream data_check("../data/t10k-images.idx3-ubyte", std::ios_base::binary);
    std::ifstream label_check("../data/t10k-labels.idx1-ubyte", std::ios_base::binary);
    std::vector<uchar> test_labels;
    label_check.read((char*)&n, 4);
    label_check.read((char*)&n, 4);
    reverse(&n);
    data_check.read((char*)&width, 4);
    data_check.read((char*)&n, 4);
    reverse(&n);
    data_check.read((char*)&width, 4);
    data_check.read((char*)&height, 4);
    reverse(&width);
    reverse(&height);
    test_labels.resize(n);
    label_check.read((char*)&(test_labels[0]), n);
    for (int i = 0; i < n; ++i)
        reverse(&(test_labels[i]));
    size_t error = 0;
    for (int i = 0; i < n; ++i) {
        std::vector<uchar> pixel;
        pixel.resize(width*height);
        data_check.read((char*)&(pixel[0]), width * height);
        cv::Mat test_x(width * height + 1, 1, CV_64F);
        for (int j = 0; j < width * height; ++j) {
            reverse(&(pixel[j]));
            test_x.at<double>(j, 0) = (double)pixel[j] / 128.0 - 1.0;
        }
        test_x.at<double>(width * height, 0) = 1.0;
        cv::Mat res = best_teta.t() * test_x;
        double max_val = -1E+9;
        int dig = -1;
        for (int j = 0; j <= 9; ++j)
            if (res.at<double>(j, 0) > max_val) {
                max_val = res.at<double>(j, 0);
                dig = j;
            }
        if (dig != test_labels[i])
            error += 1;
    }
    std::cout << "Test error rate = " << (double)error / n << std::endl;

    return 0;
}
