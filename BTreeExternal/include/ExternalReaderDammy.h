#ifndef INCLUDE_EXTRENALREADERDAMMY_H_
#define INCLUDE_EXTRENALREADERDAMMY_H_

#include <IExternalReader.h>

#include <cstdlib>
#include <fstream>
#include <set>

class Reader : public IExternalReader {
 public:
    Reader(size_t block_size);
    virtual size_t getFreeBlock();
    virtual void setBlockSize(const size_t& nsize);
    virtual bool writeBlock(size_t offset, char *data);
    virtual bool readBlock(size_t offset, char **data);
    virtual void deleteBlock(size_t offset);
    size_t getCounter() const {
        return access_counter;
    }
    void dropCounter() {
        access_counter = 0;
    }
 private:
    std::set<size_t> free_blocks_;
    size_t block_size_;
    size_t eof_pos_;
    size_t access_counter;
};

#endif  // INCLUDE_EXTRENALREADERDAMMY_H_
