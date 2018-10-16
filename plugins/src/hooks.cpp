#include "hooks.h"
#include "main.h"

#include "sdk/amx/amx.h"
#include "sdk/plugincommon.h"
#include "subhook/subhook.h"

#include <cstring>
#include <limits>
#include <malloc.h>

extern void *pAMXFunctions;

template <class FType>
class amx_hook_func;

template <class Ret, class... Args>
class amx_hook_func<Ret(*)(Args...)>
{
public:
	typedef Ret hook_ftype(Ret(*)(Args...), Args...);

	typedef Ret AMXAPI handler_ftype(Args...);

	template <subhook_t &Hook, hook_ftype *Handler>
	static Ret AMXAPI handler(Args... args)
	{
		return Handler(reinterpret_cast<Ret(*)(Args...)>(subhook_get_trampoline(Hook)), args...);
	}
};

template <int Index>
class amx_hook
{
	static subhook_t hook;

public:
	template <class FType, typename amx_hook_func<FType>::hook_ftype *Func>
	struct ctl
	{
		static void load()
		{
			typename amx_hook_func<FType>::handler_ftype *hookfn = &amx_hook_func<FType>::template handler<hook, Func>;

			hook = subhook_new(reinterpret_cast<void*>(((FType*)pAMXFunctions)[Index]), reinterpret_cast<void*>(hookfn), {});
			subhook_install(hook);
		}

		static void unload()
		{
			subhook_remove(hook);
			subhook_free(hook);
		}

		static FType orig()
		{
			if(subhook_is_installed(hook))
			{
				return reinterpret_cast<FType>(subhook_get_trampoline(hook));
			}else{
				return ((FType*)pAMXFunctions)[Index];
			}
		}
	};
};

template <int index>
subhook_t amx_hook<index>::hook;

#define AMX_HOOK_FUNC(Func, ...) Func(decltype(&::Func) orig, __VA_ARGS__)
namespace hooks
{
	int AMX_HOOK_FUNC(amx_Init, AMX *amx, void *program)
	{
		if(amx && program)
		{
			if((amx->flags & AMX_FLAG_RELOC) == 0)
			{
				auto hdr = static_cast<AMX_HEADER*>(program);

				if(hdr->magic == AMX_MAGIC && hdr->defsize == sizeof(AMX_FUNCSTUBNT))
				{
					auto namelength = reinterpret_cast<uint16_t*>(static_cast<unsigned char*>(program) + static_cast<uint32_t>(hdr->nametable));
					uint16_t length = *namelength;
					if(length > sNAMEMAX)
					{
						*namelength = sNAMEMAX;
						int result = orig(amx, program);
						if(result == AMX_ERR_NONE)
						{
							*namelength = length;
						}
						return result;
					}
				}
			}
		}
		return orig(amx, program);
	}

	int AMX_HOOK_FUNC(amx_FindNative, AMX *amx, const char *name, int *index)
	{
		int namelength;
		amx_NameLength(amx, &namelength);
		if(namelength <= sNAMEMAX)
		{
			return orig(amx, name, index);
		}
		char *pname = static_cast<char*>(alloca(namelength + 1));

		int last;
		amx_NumNatives(amx, &last);
		
		for(int idx = 0; idx < last; idx++)
		{
			amx_GetNative(amx, idx, pname);
			if(strcmp(pname, name) == 0)
			{
				*index = idx;
				return AMX_ERR_NONE;
			}
		}
		*index = std::numeric_limits<int>::max();
		return AMX_ERR_NOTFOUND;
	}

	int AMX_HOOK_FUNC(amx_FindPublic, AMX *amx, const char *name, int *index)
	{
		int namelength;
		amx_NameLength(amx, &namelength);
		if(namelength <= sNAMEMAX)
		{
			return orig(amx, name, index);
		}
		char *pname = static_cast<char*>(alloca(namelength + 1));

		int last;
		amx_NumPublics(amx, &last);
		last--;
		int first = 0;
		while(first <= last)
		{
			int mid = (first + last) / 2;
			amx_GetPublic(amx, mid, pname);
			int result = std::strcmp(pname, name);
			if(result > 0)
			{
				last = mid - 1;
			}else if(result < 0)
			{
				first = mid + 1;
			}else{
				*index = mid;
				return AMX_ERR_NONE;
			}
		}
		*index = std::numeric_limits<int>::max();
		return AMX_ERR_NOTFOUND;
	}

	int AMX_HOOK_FUNC(amx_FindPubVar, AMX *amx, const char *varname, cell *amx_addr)
	{
		int namelength;
		amx_NameLength(amx, &namelength);
		if(namelength <= sNAMEMAX)
		{
			return orig(amx, varname, amx_addr);
		}
		char *pname = static_cast<char*>(alloca(namelength + 1));

		int last;
		amx_NumPubVars(amx, &last);
		last--;
		int first = 0;
		while(first <= last)
		{
			int mid = (first + last) / 2;
			cell paddr;
			amx_GetPubVar(amx, mid, pname, &paddr);
			int result = std::strcmp(pname, varname);
			if(result > 0)
			{
				last = mid - 1;
			}else if(result < 0)
			{
				first = mid + 1;
			}else{
				*amx_addr = paddr;
				return AMX_ERR_NONE;
			}
		}
		*amx_addr = 0;
		return AMX_ERR_NOTFOUND;
	}
}

#define amx_Hook(Func) amx_hook<PLUGIN_AMX_EXPORT_##Func>::ctl<decltype(&::amx_##Func), &hooks::amx_##Func>

void hooks::load()
{
	amx_Hook(Init)::load();
	amx_Hook(FindNative)::load();
	amx_Hook(FindPublic)::load();
	amx_Hook(FindPubVar)::load();
}

void hooks::unload()
{
	amx_Hook(Init)::unload();
	amx_Hook(FindNative)::unload();
	amx_Hook(FindPublic)::unload();
	amx_Hook(FindPubVar)::unload();
}
