#include "StdInc.h"

#include "../../lib/AI_Base.h"
#include "BattleAI.h"

#ifdef __GNUC__
#define strcpy_s(a, b, c) strncpy(a, c, b)
#endif

const char *BattleAI_g_cszAiName = "Battle AI";

extern "C" DLL_EXPORT int BattleAI_GetGlobalAiVersion()
{
	return AI_INTERFACE_VER;
}

extern "C" DLL_EXPORT void BattleAI_GetAiName(char* name)
{
	strcpy_s(name, strlen(BattleAI_g_cszAiName) + 1, BattleAI_g_cszAiName);
}

extern "C" DLL_EXPORT void BattleAI_GetNewBattleAI(shared_ptr<CBattleGameInterface> &out)
{
	out = make_shared<CBattleAI>();
}
