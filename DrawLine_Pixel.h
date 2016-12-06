#pragma once

#ifdef AE_OS_WIN
typedef unsigned short PixelType;
#include <Windows.h>
#endif

#define LPOINT_RENAME_COMPONENTS 1

#include "entry.h"
#include "AE_Effect.h"
#include "AE_GeneralPlug.h"

struct Pixel {
public:
    PF_FpLong alpha;
    PF_FpLong red;
    PF_FpLong green;
    PF_FpLong blue;

    Pixel() : alpha(0.0), red(0.0), green(0.0), blue(0.0) { }

    Pixel(const PF_FpLong alpha, const PF_FpLong red, const PF_FpLong green, const PF_FpLong blue) :
        alpha(alpha), red(red), green(green), blue(blue) { }

    Pixel(const AEGP_ColorVal &color) :
        alpha(color.alphaF), red(color.redF), green(color.greenF), blue(color.blueF) { }
};