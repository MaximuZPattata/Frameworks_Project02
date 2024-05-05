#ifndef _cLuaBrain_HG_
#define _cLuaBrain_HG_

extern "C" {
#include <Lua5.4.6/lua.h>
#include <Lua5.4.6/lauxlib.h>
#include <Lua5.4.6/lualib.h>
}

#include <string>
#include <vector>
#include <map>

class cGameObject;

class cLuaBrain
{
public:
	cLuaBrain();

	~cLuaBrain();

	void LoadScript( std::string scriptName, 
					 std::string scriptSource );
	void DeleteScript( std::string scriptName );
	
	void Update(float deltaTime);

	void RunScriptImmediately(std::string script);

private:
	std::map< std::string, std::string > m_mapScripts;

	lua_State* m_pLuaState;

	std::string m_decodeLuaErrorToString( int error );
};

#endif
