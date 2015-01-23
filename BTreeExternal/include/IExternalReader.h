#ifndef INCLUDE_IEXTERNALREADER_H_
#define INCLUDE_IEXTERNALREADER_H_

#include <cstdlib>

class IExternalReader {
 public:
    virtual size_t getFreeBlock() = 0;
    virtual void setBlockSize(const size_t& sz) = 0;
    virtual bool readBlock(size_t offset, char** data) = 0;
    virtual bool writeBlock(size_t offset, char* data) = 0;
    virtual void deleteBlock(size_t offset) = 0;
    virtual ~IExternalReader() {}
};

#endif  // INCLUDE_IEXTERNALREADER_H_
