#include "cLuaBrain.h"

#include <iostream>

int Lua_MoveCommand(lua_State* L);
int Lua_OrientCommand(lua_State* L);
int Lua_CamMoveCommand(lua_State* L);
int Lua_CamPositionCommand(lua_State* L);
int Lua_CamLookAtCommand(lua_State* L);
int Lua_CurveCommand(lua_State* L);
int Lua_FollowObjectCommand(lua_State* L);
int Lua_CamLookAtAnimationCommand(lua_State* L);
int Lua_ChangeColorCommand(lua_State* L);
int Lua_SelfDestructCommand(lua_State* L);

cLuaBrain::cLuaBrain()
{
	this->m_pLuaState = luaL_newstate();

	luaL_openlibs(this->m_pLuaState);

	lua_pushcfunction(this->m_pLuaState, Lua_MoveCommand);
	lua_setglobal(this->m_pLuaState, "MoveToCommand");

	lua_pushcfunction(this->m_pLuaState, Lua_OrientCommand);
	lua_setglobal(this->m_pLuaState, "OrientToCommand");

	lua_pushcfunction(this->m_pLuaState, Lua_CamMoveCommand);
	lua_setglobal(this->m_pLuaState, "CamMoveToCommand");

	lua_pushcfunction(this->m_pLuaState, Lua_CamPositionCommand);
	lua_setglobal(this->m_pLuaState, "CamInstantMoveCommand");

	lua_pushcfunction(this->m_pLuaState, Lua_CamLookAtCommand);
	lua_setglobal(this->m_pLuaState, "CamInstantLookAtCommand");

	lua_pushcfunction(this->m_pLuaState, Lua_CamLookAtAnimationCommand);
	lua_setglobal(this->m_pLuaState, "CamLookAtAnimationCommand");

	lua_pushcfunction(this->m_pLuaState, Lua_CurveCommand);
	lua_setglobal(this->m_pLuaState, "CurvePathCommand");

	lua_pushcfunction(this->m_pLuaState, Lua_FollowObjectCommand);
	lua_setglobal(this->m_pLuaState, "FollowObjectCommand");

	lua_pushcfunction(this->m_pLuaState, Lua_ChangeColorCommand);
	lua_setglobal(this->m_pLuaState, "ChangeColorCommand");

	lua_pushcfunction(this->m_pLuaState, Lua_SelfDestructCommand);
	lua_setglobal(this->m_pLuaState, "SelfDestructCommand");

	return;
}

cLuaBrain::~cLuaBrain()
{
	lua_close(this->m_pLuaState);
	return;
}

void cLuaBrain::LoadScript( std::string scriptName, std::string scriptSource )
{
	this->m_mapScripts[scriptName] = scriptSource;
	return;
}

void cLuaBrain::DeleteScript( std::string scriptName )
{
	this->m_mapScripts.erase(scriptName);
	return;
}

void cLuaBrain::Update(float deltaTime)
{
	for( std::map< std::string /*name*/, std::string /*source*/>::iterator itScript = 
		 this->m_mapScripts.begin(); itScript != this->m_mapScripts.end(); itScript++ )
	{
		std::string curLuaScript = itScript->second;

		int error = luaL_loadstring( this->m_pLuaState, 
									 curLuaScript.c_str() );

		if ( error != 0 /*no error*/)	
		{
			std::cout << "-------------------------------------------------------" << std::endl;
			std::cout << "Error running Lua script: ";
			std::cout << itScript->first << std::endl;
			std::cout << this->m_decodeLuaErrorToString(error) << std::endl;
			std::cout << "-------------------------------------------------------" << std::endl;
			continue;
		}

		// execute funtion in "protected mode", where problems are 
		//  caught and placed on the stack for investigation
		error = lua_pcall(this->m_pLuaState, 0, 0, 0);

		if ( error != 0 /*no error*/)	
		{
			std::cout << "Lua: There was an error..." << std::endl;
			std::cout << this->m_decodeLuaErrorToString(error) << std::endl;

			std::string luaError;
			// Get error information from top of stack (-1 is top)
			luaError.append( lua_tostring(this->m_pLuaState, -1) );

			// Make error message a little more clear
			std::cout << "-------------------------------------------------------" << std::endl;
			std::cout << "Error running Lua script: ";
			std::cout << itScript->first << std::endl;
			std::cout << luaError << std::endl;
			std::cout << "-------------------------------------------------------" << std::endl;
			// We passed zero (0) as errfunc, so error is on stack)
			lua_pop(this->m_pLuaState, 1);  /* pop error message from the stack */

			continue;
		}

	}

	return;
}

void cLuaBrain::RunScriptImmediately(std::string script)
{

	int error = luaL_loadstring( this->m_pLuaState, 
								 script.c_str() );

	if ( error != 0 /*no error*/)	
	{
		std::cout << "-------------------------------------------------------" << std::endl;
		std::cout << "Error running Lua script: ";
		std::cout << this->m_decodeLuaErrorToString(error) << std::endl;
		std::cout << "-------------------------------------------------------" << std::endl;
		return;
	}

	error = lua_pcall(this->m_pLuaState, 0,	0, 0);	
							
	if ( error != 0 /*no error*/)	
	{
		std::cout << "Lua: There was an error..." << std::endl;
		std::cout << this->m_decodeLuaErrorToString(error) << std::endl;

		std::string luaError;
		
		// Get error information from top of stack (-1 is top)
		luaError.append( lua_tostring(this->m_pLuaState, -1) );

		// Make error message a little more clear
		std::cout << "-------------------------------------------------------" << std::endl;
		std::cout << "Error running Lua script: ";
		std::cout << luaError << std::endl;
		std::cout << "-------------------------------------------------------" << std::endl;
	
		lua_pop(this->m_pLuaState, 1);  /* pop error message from the stack */
	}

	return;
}

int l_ChangeTaylorSwiftTexture(lua_State* L)
{
	const char* bitmapname = lua_tostring(L, 1);
	std::string sBitMapName(bitmapname);

	return 0;
}

std::string cLuaBrain::m_decodeLuaErrorToString( int error )
{
	switch ( error )
	{
	case 0:
		return "Lua: no error";
		break;
	case LUA_ERRSYNTAX:
		return "Lua: syntax error"; 
		break;
	case LUA_ERRMEM:
		return "Lua: memory allocation error";
		break;
	case LUA_ERRRUN:
		return "Lua: Runtime error";
		break;
	case LUA_ERRERR:
		return "Lua: Error while running the error handler function";
		break;
	}//switch ( error )

	// Who knows what this error is?
	return "Lua: UNKNOWN error";
}
