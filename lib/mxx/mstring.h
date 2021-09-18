#pragma once

#include <maestro.h>

namespace std
{
class mstring
{
public:
    mstring();
    ~mstring();

    void append(const char *);
    char *c_str();

    inline mstring & operator += (const char *str)
    {
        append(str);
        return *this;
    }

private:
    size_t len;
    size_t cap;
    char *data;
};
}