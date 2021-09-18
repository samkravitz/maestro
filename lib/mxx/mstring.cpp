#include "mstring.h"

#include "mxx.h"

extern "C"
{
#include "string.h"
}

namespace std
{
mstring::mstring()
{
    len = 0;
    cap = 8;
    data = new char[cap];
}

mstring::mstring(const char *str)
{
    len = strlen(str);
    cap = round(len, 8);
    data = new char[cap];
    strcpy(data, str);
}

mstring::mstring(const mstring &other)
{
    len = strlen(other.c_str());
    cap = round(len, 8);
    data = new char[cap];
    strcpy(data, other.c_str());
}

mstring::~mstring()
{
    if (data)
        delete[] data;
}

const char *mstring::c_str()
{
    return data;
}

void mstring::append(const char *str)
{
    size_t add_len = strlen(str);
    while (add_len + len > cap)
    {
        cap *= 2;
        krealloc(data, cap);
    }

    memcpy(&data[len], str, add_len);
    len += add_len;
}
}