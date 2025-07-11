#pragma once

// This is a stub implementation of the Lua C API for compilation purposes only
// No actual Lua functionality is provided

#ifndef lua_h
#define lua_h

#ifdef __cplusplus
extern "C" {
#endif

#define LUA_VERSION     "Lua 5.4.4 (stub implementation)"
#define LUA_VERSION_NUM 504
#define LUA_VERSION_RELEASE "4"

// Lua types
typedef struct lua_State lua_State;
typedef int (*lua_CFunction) (lua_State *L);
typedef long long lua_Integer;
typedef double lua_Number;

// Lua type constants
#define LUA_TNONE               (-1)
#define LUA_TNIL                0
#define LUA_TBOOLEAN            1
#define LUA_TLIGHTUSERDATA      2
#define LUA_TNUMBER             3
#define LUA_TSTRING             4
#define LUA_TTABLE              5
#define LUA_TFUNCTION           6
#define LUA_TUSERDATA           7
#define LUA_TTHREAD             8
#define LUA_NUMTAGS             9

// Minimum Lua API for compilation
lua_State *luaL_newstate(void);
void lua_close(lua_State *L);
int lua_gettop(lua_State *L);
void lua_settop(lua_State *L, int index);
void lua_pushnil(lua_State *L);
void lua_pushboolean(lua_State *L, int b);
void lua_pushinteger(lua_State *L, lua_Integer n);
void lua_pushnumber(lua_State *L, lua_Number n);
void lua_pushstring(lua_State *L, const char *s);
int lua_toboolean(lua_State *L, int idx);
lua_Integer lua_tointeger(lua_State *L, int idx);
lua_Number lua_tonumber(lua_State *L, int idx);
const char *lua_tostring(lua_State *L, int idx);
void lua_getglobal(lua_State *L, const char *name);
void lua_setglobal(lua_State *L, const char *name);
int lua_pcall(lua_State *L, int nargs, int nresults, int errfunc);
int lua_isnil(lua_State *L, int idx);
int lua_isboolean(lua_State *L, int idx);
int lua_isnumber(lua_State *L, int idx);
int lua_isstring(lua_State *L, int idx);
int lua_isinteger(lua_State *L, int idx);
int lua_type(lua_State *L, int idx);
void *lua_touserdata(lua_State *L, int idx);
void lua_newtable(lua_State *L);
void lua_getfield(lua_State *L, int idx, const char *k);
void lua_setfield(lua_State *L, int idx, const char *k);
void lua_pop(lua_State *L, int n);
int lua_error(lua_State *L);
void lua_pushvalue(lua_State *L, int idx);
void lua_sethook(lua_State *L, void (*func)(lua_State *, void *), int mask, int count);
int lua_upvalueindex(int i);
void lua_pushlightuserdata(lua_State *L, void *p);
void lua_pushcclosure(lua_State *L, lua_CFunction fn, int n);
int lua_isfunction(lua_State *L, int idx);
int lua_yield(lua_State *L, int nresults);

// Debug masks
#define LUA_MASKCALL 1
#define LUA_MASKRET 2
#define LUA_MASKLINE 4
#define LUA_MASKCOUNT 8

// Debug structure
typedef struct lua_Debug {
    int event;
    const char *name;
    const char *namewhat;
    const char *what;
    const char *source;
    int currentline;
    int linedefined;
    int lastlinedefined;
    unsigned char nups;
    unsigned char nparams;
    char isvararg;
    char istailcall;
    char short_src[60];
} lua_Debug;

// Additional functions
void lua_getinfo(lua_State *L, const char *what, lua_Debug *ar);
int lua_getstack(lua_State *L, int level, lua_Debug *ar);
const char *lua_getlocal(lua_State *L, const lua_Debug *ar, int n);

// Set panic handler
lua_CFunction lua_atpanic(lua_State *L, lua_CFunction panicf);

#ifdef __cplusplus
}
#endif

#endif /* lua_h */

// Auxiliary library
#ifndef lauxlib_h
#define lauxlib_h

#ifdef __cplusplus
extern "C" {
#endif

// Registry
#define LUA_REGISTRYINDEX (-1000000)
#define LUA_NOREF (-2)
#define LUA_REFNIL (-1)

// Standard functions
int luaL_loadstring(lua_State *L, const char *s);
int luaL_loadbuffer(lua_State *L, const char *buff, size_t sz, const char *name);
int luaL_dostring(lua_State *L, const char *s);
int luaL_dofile(lua_State *L, const char *filename);
void luaL_openlibs(lua_State *L);
int luaL_ref(lua_State *L, int t);
void luaL_unref(lua_State *L, int t, int ref);
void *luaL_checkudata(lua_State *L, int ud, const char *tname);

// Library registration
typedef struct luaL_Reg {
  const char *name;
  lua_CFunction func;
} luaL_Reg;

void luaL_newmetatable(lua_State *L, const char *tname);
void luaL_setfuncs(lua_State *L, const luaL_Reg *l, int nup);
void luaL_requiref(lua_State *L, const char *modname, lua_CFunction openf, int glb);
void luaL_newlib(lua_State *L, const luaL_Reg *l);

// Common functions
int luaL_error(lua_State *L, const char *fmt, ...);
const char *luaL_checkstring(lua_State *L, int arg);
lua_Integer luaL_checkinteger(lua_State *L, int arg);
lua_Number luaL_checknumber(lua_State *L, int arg);
lua_Integer luaL_optinteger(lua_State *L, int arg, lua_Integer d);
void *luaL_testudata(lua_State *L, int ud, const char *tname);
int luaL_loadfile(lua_State *L, const char *filename);

// Error handling
#define LUA_OK 0
#define LUA_ERRRUN 1
#define LUA_ERRSYNTAX 2
#define LUA_ERRMEM 3
#define LUA_ERRERR 4

#ifdef __cplusplus
}
#endif

#endif /* lauxlib_h */

// Standard libraries
#ifndef lualib_h
#define lualib_h

#ifdef __cplusplus
extern "C" {
#endif

#define LUA_COLIBNAME "coroutine"
#define LUA_TABLIBNAME "table"
#define LUA_IOLIBNAME "io"
#define LUA_OSLIBNAME "os"
#define LUA_STRLIBNAME "string"
#define LUA_MATHLIBNAME "math"
#define LUA_UTF8LIBNAME "utf8"
#define LUA_DBLIBNAME "debug"
#define LUA_LOADLIBNAME "package"

int luaopen_base(lua_State *L);
int luaopen_coroutine(lua_State *L);
int luaopen_table(lua_State *L);
int luaopen_io(lua_State *L);
int luaopen_os(lua_State *L);
int luaopen_string(lua_State *L);
int luaopen_math(lua_State *L);
int luaopen_utf8(lua_State *L);
int luaopen_debug(lua_State *L);
int luaopen_package(lua_State *L);

#define LUA_MULTRET (-1)

#ifdef __cplusplus
}
#endif

#endif /* lualib_h */

// For compatibility with C++ code that includes <lua.hpp>
#ifndef lua_hpp
#define lua_hpp

#endif /* lua_hpp */
