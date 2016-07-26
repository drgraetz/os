#include "kernel.hpp"

void* memset(void* ptr, int value, size_t byteCount) {
    char* c = (char*)ptr;
    for (; byteCount > 0; byteCount--) {
        *c++ = value;
    }
    return ptr;
}

void* memcpy(void* dest, const void* source, size_t byteCount) {
    char* c = (char*)dest;
    for (; byteCount > 0; byteCount--) {
        *c++ = *(const char*)source;
        source = (char*)source + 1;
    }
    return dest;
}

MemoryStream::MemoryStream(void* buffer, size_t size, size_t pos) {
    start = (char*)buffer;
    end = (char*)buffer + size;
    if (pos > size) {
        pos = size;
    }
    current = (char*)buffer + pos;
}

off_t MemoryStream::seek(off_t pos) {
    if (pos < 0 || pos > end - start) {
        errno = EINVAL;
        return -1;
    }
    current = start + pos;
    errno = ESUCCESS;
    return pos;
}

ssize_t MemoryStream::write(const void* buf, size_t size) {
    if (buf == nullptr || size > SSIZE_MAX) {
        errno = EINVAL;
        return -1;
    }
    if (current + size > end) {
        size = end - current;
    }
    memcpy(current, buf, size);
    current += size;
    return size;
}

ssize_t MemoryStream::read(void* buf, size_t size) {
    if (buf == nullptr || size > SSIZE_MAX) {
        errno = EINVAL;
        return -1;
    }
    if (current + size > end) {
        size = end - current;
    }
    memcpy(buf, current, size);
    current += size;
    return size;
}
