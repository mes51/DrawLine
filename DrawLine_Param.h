#pragma once

#ifdef AE_OS_WIN
typedef unsigned short PixelType;
#include <Windows.h>
#endif

#define LPOINT_RENAME_COMPONENTS 1

#include "entry.h"
#include "AE_Effect.h"
#include "AE_Macros.h"
#include "AE_EffectCBSuites.h"
#include "AE_GeneralPlug.h"
#include "AEGP_SuiteHandler.h"

#include "DrawLine_Global.h"

enum DrawLineParamType {
    DL_INPUT = 0,
    DL_POSITION,
    DL_WIDTH,
    DL_COLOR,
    DL_SUBDIVISION,
    DL_REF_TIME,
    DL_NUM_PARAMS
};

enum DrawLineUIID {
    POSITION_DISK_ID = 1,
    WIDTH_DISK_ID,
    COLOR_DISK_ID,
    SUBDIVISION_DISK_ID,
    REF_TIME_DISK_ID
};

PF_Err GetIntValue(PF_InData *in_data, PF_OutData *out_data, A_Time time, DrawLineParamType type, A_long *result);

PF_Err GetDoubleValue(PF_InData *in_data, PF_OutData *out_data, A_Time time, DrawLineParamType type, PF_FpLong *result);

PF_Err GetPointValue(PF_InData *in_data, PF_OutData *out_data, A_Time time, DrawLineParamType type, AEGP_TwoDVal *result);

PF_Err GetColorValue(PF_InData *in_data, PF_OutData *out_data, A_Time time, DrawLineParamType type, AEGP_ColorVal *result);