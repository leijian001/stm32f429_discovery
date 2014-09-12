#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

#include "lua_fatfs.h"

static const luaL_Reg lua_exlibs[] =
{
	{"fatfs_packet", 	open_fatfslib},
	{NULL, NULL}
};

LUALIB_API void lua_openexlibs(lua_State *L)
{
	const luaL_Reg *lib;

	for(lib = lua_exlibs; lib->func; lib++)
	{
		luaL_requiref(L, lib->name, lib->func, 1);
		lua_pop(L, 1);  /* remove lib */
	}
}
