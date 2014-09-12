#ifndef __LUA_FATFS_H__
#define __LUA_FATFS_H__

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

#include "ff.h"

int fatfs_loadfilex (lua_State *L, const char *filename, const char *mode);
LUAMOD_API int open_fatfslib(lua_State *L);

#define fatfs_loadfile(L, f) fatfs_loadfilex(L, f, 0)
#define fatfs_dofile(L, fn) \
		(fatfs_loadfile(L, fn) || lua_pcall(L, 0, LUA_MULTRET, 0))

#endif
