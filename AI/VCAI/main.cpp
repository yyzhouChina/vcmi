#include "StdInc.h"
#include "VCAI.h"

#ifdef __GNUC__
#define strcpy_s(a, b, c) strncpy(a, c, b)
#endif

const char *VCAI_g_cszAiName = "VCAI";

extern "C" DLL_EXPORT int VCAI_GetGlobalAiVersion()
{
	return AI_INTERFACE_VER;
}

extern "C" DLL_EXPORT void VCAI_GetAiName(char* name)
{
	strcpy_s(name, strlen(VCAI_g_cszAiName) + 1, VCAI_g_cszAiName);
}

extern "C" DLL_EXPORT void VCAI_GetNewAI(shared_ptr<CGlobalAI> &out)
{
	out = make_shared<VCAI>();
}
