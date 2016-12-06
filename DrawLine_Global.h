#pragma once

#ifdef AE_OS_WIN
typedef unsigned short PixelType;
#include <Windows.h>
#endif

#define LPOINT_RENAME_COMPONENTS 1

#include "entry.h"
#include "AE_Effect.h"
#include "AE_GeneralPlug.h"

typedef struct {
    PF_Boolean initialized;
    AEGP_PluginID pluginId;
} DrawLineGlobalData, *DrawLineGlobalDataPtr, **DrawLineGlobalDataHandle;

inline void SetTime(A_Time &val, A_long time, A_u_long scale) {
    val.value = time;
    val.scale = scale;
}

inline A_Time CreateTime(A_long time, A_u_long scale) {
    A_Time result;
    result.value = time;
    result.scale = scale;
    return result;
}