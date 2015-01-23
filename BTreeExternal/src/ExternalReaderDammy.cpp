#include "ExternalReaderDammy.h"

Reader::Reader(size_t block_size):
        block_size_(block_size),
        eof_pos_(-1),
        access_counter(0) {}

size_t Reader::getFreeBlock() {
    if (free_blocks_.size() == 0) {
        size_t pos = eof_pos_ + 1;
        eof_pos_ += block_size_;
        return pos;
    }
    size_t pos = *free_blocks_.begin();
    *free_blocks_.erase(free_blocks_.begin());
    return pos;
}

void Reader::setBlockSize(const size_t& nsize) {
    block_size_ = nsize;
}

bool Reader::writeBlock(size_t offset, char *data) {
    if  (offset % block_size_ != 0)
        return false;
    std::fstream file("tree.dat", std::ios_base::binary |
                      std::ios_base::in | std::ios_base::out);
    file.seekp(offset, std::ios_base::beg);
    file.write(data, block_size_);
    file.close();
    access_counter++;
    return true;
}

bool Reader::readBlock(size_t offset, char **data) {
    std::fstream file("tree.dat", std::ios_base::in |
                        std::ios_base::binary);
    file.seekg(offset, std::ios_base::beg);
    *data = static_cast<char*>(malloc(block_size_));
    file.read(*data, block_size_);
    access_counter++;
    bool res = file.gcount() == block_size_;
    file.close();
    return res;
}

void Reader::deleteBlock(size_t offset) {
    free_blocks_.insert(offset);
    access_counter++;
}
