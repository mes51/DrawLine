#include "DrawLine_Param.h"

PF_Err GetParam(PF_InData * in_data, PF_OutData * out_data, A_Time time, DrawLineParamType type, AEGP_StreamValue2 *paramValue) {
    PF_Err err = PF_Err_NONE;
    AEGP_SuiteHandler suites(in_data->pica_basicP);
    DrawLineGlobalDataPtr globalPtr = reinterpret_cast<DrawLineGlobalDataPtr>(DH(out_data->global_data));
    AEGP_EffectRefH effect = NULL;
    AEGP_StreamRefH paramStream = NULL;

    ERR(suites.PFInterfaceSuite1()->AEGP_GetNewEffectForEffect(globalPtr->pluginId, in_data->effect_ref, &effect));
    ERR(suites.StreamSuite2()->AEGP_GetNewEffectStreamByIndex(globalPtr->pluginId, effect, type, &paramStream));
    ERR(suites.StreamSuite4()->AEGP_GetNewStreamValue(globalPtr->pluginId, paramStream, AEGP_LTimeMode_LayerTime, &time, FALSE, paramValue));

    if (paramStream) {
        ERR(suites.StreamSuite2()->AEGP_DisposeStream(paramStream));
    }
    if (effect) {
        ERR(suites.EffectSuite2()->AEGP_DisposeEffect(effect));
    }

    return err;
}

PF_Err GetIntValue(PF_InData * in_data, PF_OutData * out_data, A_Time time, DrawLineParamType type, A_long *result) {
    AEGP_StreamValue2 paramValue;
    AEFX_CLR_STRUCT(paramValue);
    PF_Err err = GetParam(in_data, out_data, time, type, &paramValue);

    *result = (A_long)paramValue.val.one_d;

    return err;
}

PF_Err GetDoubleValue(PF_InData *in_data, PF_OutData *out_data, A_Time time, DrawLineParamType type, PF_FpLong *result) {
    AEGP_StreamValue2 paramValue;
    AEFX_CLR_STRUCT(paramValue);
    PF_Err err = GetParam(in_data, out_data, time, type, &paramValue);

    *result = paramValue.val.one_d;

    return err;
}

PF_Err GetPointValue(PF_InData *in_data, PF_OutData *out_data, A_Time time, DrawLineParamType type, AEGP_TwoDVal *result) {
    AEGP_StreamValue2 paramValue;
    AEFX_CLR_STRUCT(paramValue);
    PF_Err err = GetParam(in_data, out_data, time, type, &paramValue);

    *result = paramValue.val.two_d;

    return err;
}

PF_Err GetColorValue(PF_InData *in_data, PF_OutData *out_data, A_Time time, DrawLineParamType type, AEGP_ColorVal *result) {
    AEGP_StreamValue2 paramValue;
    AEFX_CLR_STRUCT(paramValue);
    PF_Err err = GetParam(in_data, out_data, time, type, &paramValue);

    *result = paramValue.val.color;

    return err;
}