#ifndef lauxlib_h
#define lauxlib_h

#include "lua.h"

/* Types */
typedef struct luaL_Reg {
  const char *name;
  lua_CFunction func;
} luaL_Reg;

/* Functions */
void luaL_openlibs(lua_State *L);
int luaL_loadstring(lua_State *L, const char *s);
int luaL_loadfile(lua_State *L, const char *filename);
int luaL_dofile(lua_State *L, const char *filename);
int luaL_dostring(lua_State *L, const char *str);
void luaL_setfuncs(lua_State *L, const luaL_Reg *l, int nup);
void luaL_newlib(lua_State *L, const luaL_Reg *l);
int luaL_error(lua_State *L, const char *fmt, ...);
lua_State *luaL_newstate(void);
const char *luaL_checkstring(lua_State *L, int arg);
lua_Number luaL_checknumber(lua_State *L, int arg);
lua_Integer luaL_checkinteger(lua_State *L, int arg);

#endif
