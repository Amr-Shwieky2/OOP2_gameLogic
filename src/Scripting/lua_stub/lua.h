#ifndef lua_h
#define lua_h

/* Lua version */
#define LUA_VERSION_MAJOR  "5"
#define LUA_VERSION_MINOR  "4"
#define LUA_VERSION_RELEASE  "4"
#define LUA_VERSION_NUM  504
#define LUA_VERSION  "Lua " LUA_VERSION_MAJOR "." LUA_VERSION_MINOR
#define LUA_RELEASE  LUA_VERSION "." LUA_VERSION_RELEASE

/* Types */
typedef struct lua_State lua_State;
typedef int (*lua_CFunction) (lua_State *L);
typedef long long lua_Integer;
typedef double lua_Number;

/* Functions */
lua_State *lua_newstate(void *f, void *ud);
void lua_close(lua_State *L);
int lua_gettop(lua_State *L);
void lua_settop(lua_State *L, int index);
void lua_pushnil(lua_State *L);
void lua_pushinteger(lua_State *L, lua_Integer n);
void lua_pushnumber(lua_State *L, lua_Number n);
void lua_pushstring(lua_State *L, const char *s);
void lua_pushboolean(lua_State *L, int b);
void lua_getglobal(lua_State *L, const char *name);
void lua_setglobal(lua_State *L, const char *name);
int lua_pcall(lua_State *L, int nargs, int nresults, int msgh);
int lua_error(lua_State *L);
const char *lua_tostring(lua_State *L, int index);
lua_Integer lua_tointeger(lua_State *L, int index);
lua_Number lua_tonumber(lua_State *L, int index);
int lua_toboolean(lua_State *L, int index);
int lua_type(lua_State *L, int index);
void lua_pushlightuserdata(lua_State *L, void *p);

#endif
