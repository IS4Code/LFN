#include "main.h"
#include "hooks.h"

#include "sdk/amx/amx.h"
#include "sdk/plugincommon.h"

logprintf_t logprintf;
extern void *pAMXFunctions;

PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports() 
{
	return SUPPORTS_VERSION | SUPPORTS_AMX_NATIVES;
}

PLUGIN_EXPORT bool PLUGIN_CALL Load(void **ppData)
{
	pAMXFunctions = ppData[PLUGIN_DATA_AMX_EXPORTS];
	logprintf = (logprintf_t)ppData[PLUGIN_DATA_LOGPRINTF];

	hooks::load();

	logprintf(" Long Function Names v1.0 loaded");
	logprintf(" Created by IllidanS4");
	return true;
}

PLUGIN_EXPORT void PLUGIN_CALL Unload()
{
	hooks::unload();

	logprintf(" Long Function Names v1.0 unloaded");
}

PLUGIN_EXPORT int PLUGIN_CALL AmxLoad(AMX *amx) 
{
	return AMX_ERR_NONE;
}

PLUGIN_EXPORT int PLUGIN_CALL AmxUnload(AMX *amx) 
{
	return AMX_ERR_NONE;
}
