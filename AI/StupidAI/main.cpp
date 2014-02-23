#include "StdInc.h"

#include "../../lib/AI_Base.h"
#include "StupidAI.h"

#ifdef __GNUC__
#define strcpy_s(a, b, c) strncpy(a, c, b)
#endif

const char *StupidAI_g_cszAiName = "Stupid AI 0.1";

extern "C" DLL_EXPORT int StupidAI_GetGlobalAiVersion()
{
	return AI_INTERFACE_VER;
}

extern "C" DLL_EXPORT void StupidAI_GetAiName(char* name)
{
	strcpy_s(name, strlen(StupidAI_g_cszAiName) + 1, StupidAI_g_cszAiName);
}

extern "C" DLL_EXPORT void StupidAI_GetNewBattleAI(shared_ptr<CBattleGameInterface> &out)
{
	out = make_shared<CStupidAI>();
}
