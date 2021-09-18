#pragma once

#include <maestro.h>

namespace std
{
class mstring
{
// rounds an integer x up to the nearest multiple of to
#define round(x, to) ((x + to - 1) & (-to))
public:
    mstring();
    mstring(const char *);
    mstring(const mstring &);
    ~mstring();

    void append(const char *);
    const char *c_str();

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