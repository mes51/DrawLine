#include "DrawLine.h"

static PF_Err
About(
    PF_InData *in_data,
    PF_OutData *out_data,
    PF_ParamDef *params[],
    PF_LayerDef *output)
{
    AEGP_SuiteHandler suites(in_data->pica_basicP);

    suites.ANSICallbacksSuite1()->sprintf(out_data->return_msg,
        "%s v%d.%d\r%s",
        STR(StrID_Name),
        MAJOR_VERSION,
        MINOR_VERSION,
        STR(StrID_Description));
    return PF_Err_NONE;
}

static PF_Err
GlobalSetup(
    PF_InData *in_data,
    PF_OutData *out_data,
    PF_ParamDef *params[],
    PF_LayerDef *output)
{
    out_data->my_version = PF_VERSION(MAJOR_VERSION,
        MINOR_VERSION,
        BUG_VERSION,
        STAGE_VERSION,
        BUILD_VERSION);

    PF_Err err = PF_Err_NONE;

    out_data->out_flags = PF_OutFlag_DEEP_COLOR_AWARE | // just 16bpc, not 32bpc
        PF_OutFlag_PIX_INDEPENDENT |
        PF_OutFlag_NON_PARAM_VARY |
        PF_OutFlag_USE_OUTPUT_EXTENT |
        PF_OutFlag_CUSTOM_UI;

    AEGP_SuiteHandler suites(in_data->pica_basicP);
    PF_Handle globalHandler = suites.HandleSuite1()->host_new_handle(sizeof(DrawLineGlobalData));

    if (globalHandler) {
        DrawLineGlobalDataPtr globalPtr = reinterpret_cast<DrawLineGlobalDataPtr>(suites.HandleSuite1()->host_lock_handle(globalHandler));
        if (globalPtr) {
            globalPtr->initialized = TRUE;
            if (in_data->appl_id != 'PrMr') {
                ERR(suites.UtilitySuite3()->AEGP_RegisterWithAEGP(NULL, STR(StrID_Name), &globalPtr->pluginId));
            }
            if (!err) {
                out_data->global_data = globalHandler;
            }
        }
        suites.HandleSuite1()->host_unlock_handle(globalHandler);
    } else {
        err = PF_Err_INTERNAL_STRUCT_DAMAGED;
    }

    return PF_Err_NONE;
}

static PF_Err
GlobalSetdown(PF_InData *in_data)
{
    AEGP_SuiteHandler	suites(in_data->pica_basicP);

    if (in_data->global_data) {
        suites.HandleSuite1()->host_dispose_handle(in_data->global_data);
    }

    return PF_Err_NONE;
}

static PF_Err
ParamsSetup(
    PF_InData *in_data,
    PF_OutData *out_data,
    PF_ParamDef *params[],
    PF_LayerDef *output)
{
    PF_Err err = PF_Err_NONE;
    PF_ParamDef def;

    PF_ADD_POINT(
        STR(StrID_Position_Param_Name),
        DL_POSITION_CENTER,
        DL_POSITION_CENTER,
        RESTRICT_BOUNDS,
        POSITION_DISK_ID
    );

    PF_ADD_FLOAT_SLIDERX(
        STR(StrID_Width_Param_Name),
        DL_WIDTH_MIN,
        DL_WIDTH_MAX,
        DL_WIDTH_MIN,
        DL_WIDTH_MAX,
        DL_WIDTH_DFLT,
        PF_Precision_HUNDREDTHS,
        0,
        0,
        WIDTH_DISK_ID
    );

    PF_ADD_COLOR(
        STR(StrID_Color_Param_Name),
        PF_HALF_CHAN8,
        PF_MAX_CHAN8,
        PF_MAX_CHAN8,
        COLOR_DISK_ID
    );


    PF_ADD_SLIDER(
        STR(StrID_Subdivision_Param_Name),
        DL_SUBDIVISION_MIN,
        DL_SUBDIVISION_MAX,
        DL_SUBDIVISION_MIN,
        DL_SUBDIVISION_MAX,
        DL_SUBDIVISION_DFLT,
        WIDTH_DISK_ID
    );

    PF_ADD_FLOAT_SLIDERX(
        STR(StrID_RefTime_Param_Name),
        DL_REF_TIME_MIN,
        DL_REF_TIME_MAX,
        DL_REF_TIME_MIN,
        DL_REF_TIME_MAX,
        DL_REF_TIME_DFLT,
        PF_Precision_HUNDREDTHS,
        0,
        0,
        REF_TIME_DISK_ID
    );

    out_data->num_params = DL_NUM_PARAMS;

    return err;
}

static Pixel RasterizePath(const std::vector<std::unique_ptr<Path>> &path, Vector2 &p) {
    std::vector<HitInfo> hits;
    for (auto itr = path.begin(), end = path.end(); itr != end; ++itr) {
        if ((*itr)->rect.inPoint(p)) {
            (*itr)->GetNearests(p, hits);
        }
    }
    //std::sort(hits.begin(), hits.end(), ([](const HitInfo &a, const HitInfo &b) -> bool { return a.time < b.time; }));

    Pixel result;

    for (auto itr = hits.begin(), end = hits.end(); itr != end; ++itr) {
        PF_FpLong maxAlpha = min(1.0, itr->info.width * 2.0);
        PF_FpLong alpha = (1.0 - max(0.0, min(1.0, itr->distance - itr->info.width + 1.0))) * maxAlpha;
        if (alpha > 0.0) {
            Pixel px = itr->info.color;
            if (result.alpha <= 0.0) {
                result.alpha = px.alpha * alpha;
                result.red = px.red;
                result.green = px.green;
                result.blue = px.blue;
            } else {
                PF_FpLong fa = px.alpha * alpha;
                PF_FpLong ra = result.alpha + fa - result.alpha * fa;
                PF_FpLong ira = 1.0 / ra;
                result.alpha = ra;
                result.red = min((px.red * fa + (1.0 - fa) * result.red * result.alpha) * ira, 1.0);
                result.green = min((px.green * fa + (1.0 -fa) * result.green * result.alpha) * ira, 1.0);
                result.blue = min((px.blue * fa + (1.0 - fa) * result.blue * result.alpha) * ira, 1.0);
            }
        }
    }

    return result;
}

static PF_Err Rasterize8(void *refcon, A_long x, A_long y, PF_Pixel8 *in, PF_Pixel8 *out) {
    RenderInfo *renderInfo = reinterpret_cast<RenderInfo*>(refcon);
    
    if (renderInfo->minY <= y && renderInfo->maxY >= y) {
        Vector2 p(x, y);
        Pixel pixel = RasterizePath(renderInfo->path, p);

        out->alpha = (A_long)floor(pixel.alpha * in->alpha);
        out->red = (A_long)floor(pixel.red * PF_MAX_CHAN8);
        out->green = (A_long)floor(pixel.green * PF_MAX_CHAN8);
        out->blue = (A_long)floor(pixel.blue * PF_MAX_CHAN8);
    }

    return PF_Err_NONE;
}

static PF_Err Rasterize16(void *refcon, A_long x, A_long y, PF_Pixel16 *in, PF_Pixel16 *out) {
    RenderInfo *renderInfo = reinterpret_cast<RenderInfo*>(refcon);

    if (renderInfo->minY <= y && renderInfo->maxY >= y) {
        Vector2 p(x, y);
        Pixel pixel = RasterizePath(renderInfo->path, p);

        out->alpha = (A_long)floor(pixel.alpha * in->alpha);
        out->red = (A_long)floor(pixel.red * PF_MAX_CHAN16);
        out->green = (A_long)floor(pixel.green * PF_MAX_CHAN16);
        out->blue = (A_long)floor(pixel.blue * PF_MAX_CHAN16);
    }

    return PF_Err_NONE;
}

static PF_Err
Render(
    PF_InData *in_data,
    PF_OutData *out_data,
    PF_ParamDef *params[],
    PF_LayerDef *output)
{
    PF_Err err = PF_Err_NONE;
    PF_FpLong minY = HUGE_VAL;
    PF_FpLong maxY = -HUGE_VAL;
    PF_FpLong maxWidth = -HUGE_VAL;

    std::vector<ParameterizedVertex> points;
    A_long time = max(0, in_data->current_time - (A_long)floor(params[DL_REF_TIME]->u.fs_d.value * in_data->time_scale));
    for (; time <= in_data->current_time; time += in_data->time_step) {
        A_long subdivision = 0;
        ERR(GetIntValue(in_data, out_data, CreateTime(time, in_data->time_scale), DL_SUBDIVISION, &subdivision));
        A_long timeScale = in_data->time_scale * subdivision;
        A_long scaledTime = time * subdivision;
        A_long now = in_data->current_time * subdivision;
        A_Time structuredTime = CreateTime(scaledTime, timeScale);
        for (A_long i = 0; i < subdivision && scaledTime <= now; i++, scaledTime += in_data->time_step) {
            AEGP_TwoDVal point;
            AEGP_ColorVal color;
            PF_FpLong width;
            structuredTime.value = scaledTime;
            ERR(GetPointValue(in_data, out_data, structuredTime, DL_POSITION, &point));
            ERR(GetDoubleValue(in_data, out_data, structuredTime, DL_WIDTH, &width));
            ERR(GetColorValue(in_data, out_data, structuredTime, DL_COLOR, &color));
            Vector2 p(point);
            if (points.size() > 1 && (points.end() - 1)->point == p && (points.end() - 2)->point == p) {
                points.pop_back();
            } else {
                minY = min(minY, point.y);
                maxY = max(maxY, point.x);
            }
            maxWidth = max(maxWidth, width);
            points.push_back(ParameterizedVertex(p, VertexInfo(width, Pixel(color)), scaledTime / (PF_FpLong)timeScale));
        }
    }
    if (points.size() < 2) {
        return err;
    }


    std::vector<std::unique_ptr<std::vector<ParameterizedVertex>>> separatedPoints;
    for (A_long i = (A_long)points.size() / (GroupSize - GroupOverlap * 2) + 1; i > -1; --i) {
        separatedPoints.push_back(std::make_unique<std::vector<ParameterizedVertex>>());
    }
    A_long pointIndex = 0;
    for (auto itr = points.begin(), end = points.end(); itr != end; ++pointIndex, ++itr) {
        A_long index = pointIndex / GroupSeparator;
        separatedPoints[index]->push_back(*itr);
        A_long prevIndex = index - 1;
        if (prevIndex > -1 && (pointIndex - prevIndex * GroupSeparator) < GroupSize) {
            separatedPoints[prevIndex]->push_back(*itr);
        }
    }
    while (separatedPoints.back()->size() < 1) {
        separatedPoints.pop_back();
    }

    std::vector<std::unique_ptr<Path>> path;
    for (auto itr = separatedPoints.begin(), end = separatedPoints.end(); itr != end; ++itr) {
        path.push_back(std::make_unique<Path>(**itr));
    }

    AEGP_SuiteHandler suites(in_data->pica_basicP);
    A_long lines = in_data->extent_hint.top - in_data->extent_hint.bottom;
    RenderInfo renderInfo(std::move(path), minY - maxWidth, maxY + maxWidth);

    if (in_data->extent_hint.top != output->extent_hint.top ||
        in_data->extent_hint.left != output->extent_hint.left ||
        in_data->extent_hint.bottom != output->extent_hint.bottom ||
        in_data->extent_hint.right != output->extent_hint.right
        ) {
        ERR(PF_FILL(NULL, &output->extent_hint, output));
    }

    if (PF_WORLD_IS_DEEP(output)) {
        ERR(suites.Iterate16Suite1()->iterate(in_data, 0, lines, &params[DL_INPUT]->u.ld, &in_data->extent_hint, reinterpret_cast<void*>(&renderInfo), Rasterize16, output));
    } else {
        ERR(suites.Iterate8Suite1()->iterate(in_data, 0, lines, &params[DL_INPUT]->u.ld, &in_data->extent_hint, reinterpret_cast<void*>(&renderInfo), Rasterize8, output));
    }

    return err;
}

DllExport
PF_Err
EntryPointFunc(
    PF_Cmd cmd,
    PF_InData *in_data,
    PF_OutData *out_data,
    PF_ParamDef *params[],
    PF_LayerDef *output,
    void *extra)
{
    PF_Err err = PF_Err_NONE;

    try {
        switch (cmd) {
        case PF_Cmd_ABOUT:
            err = About(in_data,
                out_data,
                params,
                output);
            break;

        case PF_Cmd_GLOBAL_SETUP:
            err = GlobalSetup(in_data,
                out_data,
                params,
                output);
            break;

        case PF_Cmd_GLOBAL_SETDOWN:
            err = GlobalSetdown(in_data);
            break;

        case PF_Cmd_PARAMS_SETUP:
            err = ParamsSetup(in_data,
                out_data,
                params,
                output);
            break;

        case PF_Cmd_RENDER:
            err = Render(in_data,
                out_data,
                params,
                output);
            break;
        }
    } catch (PF_Err &thrown_err) {
        err = thrown_err;
    }
    return err;
}

