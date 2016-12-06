#pragma once

#ifndef DRAW_LINE_H
#define DRAW_LINE_H

typedef unsigned char u_char;
typedef unsigned short u_short;
typedef unsigned short u_int16;
typedef unsigned long u_long;
typedef short int int16;
#define PF_TABLE_BITS    12
#define PF_TABLE_SZ_16    4096

#define LPOINT_RENAME_COMPONENTS 1
#define PF_DEEP_COLOR_AWARE 1    // make sure we get 16bpc pixels; 
// AE_Effect.h checks for this.

#include "AEConfig.h"

#ifdef AE_OS_WIN
typedef unsigned short PixelType;
#include <Windows.h>
#endif

#include "entry.h"
#include "AE_Effect.h"
#include "AE_EffectCB.h"
#include "AE_Macros.h"
#include "Param_Utils.h"
#include "AE_EffectCBSuites.h"
#include "String_Utils.h"
#include "AE_GeneralPlug.h"
#include "AEFX_ChannelDepthTpl.h"
#include "AEGP_SuiteHandler.h"

#include "DrawLine_Pixel.h"
#include "DrawLine_Util.h"
#include "DrawLine_Geometry.h"
#include "DrawLine_Global.h"
#include "DrawLine_Param.h"
#include "DrawLine_Strings.h"

/* Versioning information */

#define MAJOR_VERSION 1
#define MINOR_VERSION 0
#define BUG_VERSION 0
#define STAGE_VERSION PF_Stage_DEVELOP
#define BUILD_VERSION 1

constexpr A_long GroupSize = 40;
constexpr A_long GroupOverlap = 1;
constexpr A_long GroupSeparator = (GroupSize - GroupOverlap);

/* Parameter defaults */

#define DL_POSITION_CENTER 50
#define RESTRICT_BOUNDS 0

#define DL_WIDTH_MIN 0
#define DL_WIDTH_MAX 1000
#define DL_WIDTH_DFLT 2

#define DL_SUBDIVISION_MIN 1
#define DL_SUBDIVISION_MAX 100
#define DL_SUBDIVISION_DFLT 5

#define DL_REF_TIME_MIN 0
#define DL_REF_TIME_MAX 10000
#define DL_REF_TIME_DFLT 5

class RenderInfo {
public:
    const std::vector<std::unique_ptr<Path>> path;
    const PF_FpLong minY;
    const PF_FpLong maxY;

    RenderInfo(std::vector<std::unique_ptr<Path>> path, PF_FpLong minY, PF_FpLong maxY) :
        path(std::move(path)), minY(minY), maxY(maxY) { }
};

#ifdef __cplusplus
extern "C" {
#endif

    DllExport    PF_Err
        EntryPointFunc(
            PF_Cmd            cmd,
            PF_InData        *in_data,
            PF_OutData        *out_data,
            PF_ParamDef        *params[],
            PF_LayerDef        *output,
            void            *extra);

#ifdef __cplusplus
}
#endif

#endif // DRAW_LINE_H