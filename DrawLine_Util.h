#pragma once

#ifdef AE_OS_WIN
typedef unsigned short PixelType;
#include <Windows.h>
#endif

#define LPOINT_RENAME_COMPONENTS 1

#include "AE_Effect.h"

class Util {
public:
    inline static PF_FpLong Interpolate(const PF_FpLong v1, const PF_FpLong v2, const PF_FpLong t);

private:
    Util() { };
};

inline PF_FpLong Util::Interpolate(const PF_FpLong v1, const PF_FpLong v2, const PF_FpLong t)
{
    PF_FpLong diff = v2 - v1;
    return v1 + diff * t;
}