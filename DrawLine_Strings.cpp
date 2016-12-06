#include "DrawLine.h"

typedef struct {
    A_u_long index;
    A_char str[256];
} TableString;



TableString g_strs[StrID_NUMTYPES] = {
    StrID_NONE, "",
    StrID_Name, "DrawLine",
    StrID_Description, "Render Line.\rCopyright 2016 mes.",
    StrID_Position_Param_Name, "Position",
    StrID_Width_Param_Name, "Width",
    StrID_Color_Param_Name, "Color",
    StrID_Subdivision_Param_Name, "Subdivision",
    StrID_RefTime_Param_Name, "ReferenceTime"
};


char *GetStringPtr(int strNum)
{
    return g_strs[strNum].str;
}
