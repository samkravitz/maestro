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
    data = new char[8];
}

mstring::~mstring()
{
    if (data)
        delete[] data;
}

char *mstring::c_str()
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