#include "DataHolder.h"

#include <fstream>
#include <sstream>
#include <boost/filesystem.hpp>

using namespace boost;

bool DataHolder::loadData(const std::string &path) {
    filesystem::path boost_path(path);
    if (!filesystem::exists(boost_path))
        return false;
    filesystem::directory_iterator end_it;
    for (filesystem::directory_iterator i(boost_path); i != end_it; ++i) {
        if (filesystem::is_directory(i->status())) {
            loadData(i->path().string());
        } else {
            readFile(i->path());
        }
    }
    return true;
}

void DataHolder::readFile(const filesystem::path &path) {
    std::string file_name = path.string();
    std::ifstream in(file_name.c_str());
    bool spam;
    if (file_name.find("spmsg") != std::string::npos) {
        spam = true;
    } else {
        if (file_name.find("legit") != std::string::npos)
            spam = false;
        else
            return;
    }
    std::string subj;
    std::getline(in, subj);
    std::stringstream ss;
    ss.str(subj);
    ss >> subj;
    while (!ss.eof()) {
        long long n;
        ss >> n;
        if (spam) {
            subject[n].spam++;
            total_subj.spam++;
        } else {
            subject[n].legit++;
            total_subj.legit++;
        }
    }
    while (!in.eof()) {
        long long n;
        in >> n;
        if (spam) {
            text[n].spam++;
            total_text.spam++;
        } else {
            text[n].legit++;
            total_text.legit++;
        }
    }
}

void DataHolder::removeFile(const filesystem::path &path) {
    std::string file_name = path.string();
    std::ifstream in(file_name.c_str());
    bool spam;
    if (file_name.find("spmsg") != std::string::npos) {
        spam = true;
    } else {
        if (file_name.find("legit") != std::string::npos)
            spam = false;
        else
            return;
    }
    std::string subj;
    std::getline(in, subj);
    std::stringstream ss;
    ss.str(subj);
    ss >> subj;
    while (!ss.eof()) {
        long long n;
        ss >> n;
        if (spam) {
            subject[n].spam--;
            total_subj.spam--;
        } else {
            subject[n].legit--;
            total_subj.legit--;
        }
    }
    while (!in.eof()) {
        long long n;
        in >> n;
        if (spam) {
            text[n].spam--;
            total_text.spam--;
        } else {
            text[n].legit--;
            total_text.legit--;
        }
    }
}
