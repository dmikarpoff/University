#ifndef INCLUDE_DATAHOLDER_H_
#define INCLUDE_DATAHOLDER_H_

#include <boost/filesystem.hpp>
#include <map>

struct WordStat {
    WordStat():
        spam(0), legit(0) {}

    long long spam;
    long long legit;
};

struct DataHolder {
    DataHolder() {}
    bool loadData(const std::string& path);
    void readFile(const boost::filesystem::path& path);
    void removeFile(const boost::filesystem::path& path);

    std::map<long long, WordStat> text;
    std::map<long long, WordStat> subject;
    WordStat total_subj;
    WordStat total_text;
};

#endif  // INCLUDE_DATAHOLDER_H_
