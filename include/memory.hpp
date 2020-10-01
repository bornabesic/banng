#pragma once

#include <cassert>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

template <typename T>
class Memory {
  protected:
    T *data;
    unsigned int size;
    bool owned;

    Memory() : data(nullptr), size(0), owned(false) {}

  public:
    // Memory(Memory<T> &other) {
    //     this->data = other.data;
    //     this->size = other.size;
    //     this->owned = other.owned;

    //     other.owned = false;
    // }

    // Memory(Memory<T> &&other) {
    //     this->data = other.data;
    //     this->size = other.size;
    //     this->owned = other.owned;

    //     other.owned = false;
    // }

    Memory(T *data, unsigned int size)
        : data(data), size(size), owned(false) {}

    T &operator*() const { return *data; }

    T &operator[](unsigned int i) const { return *(this->data + i); }

    T *operator+(unsigned int i) const { return this->data + i; }

    operator T *() const { return this->data; }

    virtual ~Memory(){};
};

template <typename T>
class RAM : public Memory<T> {
  public:
    using Memory<T>::Memory;

    RAM(unsigned int size) {
        this->data = new T[size];
        this->size = size;
        this->owned = true;
    }

    ~RAM() {
        if (!this->owned)
            return;

        delete[] this->data;
    }
};

template <typename T>
class Disk : public Memory<T> {
  private:
    int fd;

  public:
    using Memory<T>::Memory;

    Disk(unsigned int size, const std::string &filename) {
        this->fd = open(filename.c_str(), O_RDWR | O_CREAT | O_TRUNC,
                        S_IRUSR | S_IWUSR);
        assert(this->fd != -1);

        fallocate(fd, 0, 0, size * sizeof(T));
        this->data = (T *)mmap(nullptr, size * sizeof(T),
                               PROT_READ | PROT_WRITE, MAP_SHARED, this->fd, 0);
        assert(this->data != nullptr);

        this->size = size;
        this->owned = true;
    }

    ~Disk() {
        if (!this->owned)
            return;

        munmap((void *)this->data, this->size * sizeof(T));
        close(this->fd);
    }
};
