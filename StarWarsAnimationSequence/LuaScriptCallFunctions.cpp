#include "pch.h"
#include "cMoveToCommand.h"
#include "cOrientToCommand.h"
#include "cFollowCurveCommand.h"
#include "cFollowObjectCommand.h"
#include "cCameraAnimation.h"
#include "iAnimationCommand.h"
#include "cAnimationCommandGroup.h"
#include "LuaBrain/cLuaBrain.h"
#include "cUniqueIdGenerator.h"
#include "cChangeColorCommand.h"
#include "cSelfDestroyCommand.h"

#include <cControlGameEngine.h>

extern std::vector<std::unique_ptr<cAnimationCommandGroup>> groupAnimationsList;
extern std::vector<cCameraAnimation> camAnimationList;
extern cControlGameEngine gameEngine;

cUniqueIDGenerator uniqueIdGenerator;

std::string toUpperCase(const std::string& stringInput) 
{
	std::string result;
	result.reserve(stringInput.size());

	for (char c : stringInput)
		result += std::toupper(static_cast<unsigned char>(c));

	return result;
}

int Lua_MoveCommand(lua_State* L)
{
	//---------------------------Initialize and retrieve all parameters-----------------------------------------

	glm::vec3 meshDestinationPos, meshCurrentPos;

	const char* meshName(lua_tostring(L, 1));
	meshDestinationPos.x = (float)lua_tonumber(L, 2);  
	meshDestinationPos.y = (float)lua_tonumber(L, 3);  
	meshDestinationPos.z = (float)lua_tonumber(L, 4);  
	float timeTaken = (float)lua_tonumber(L, 5);
	float easeIn = (float)lua_tonumber(L, 6);
	float easeOut = (float)lua_tonumber(L, 7);
	std::string groupType(lua_tostring(L, 8));
	const char* groupName(lua_tostring(L, 9));

	groupType = toUpperCase(groupType);

	//------------------------------------Create Move Command---------------------------------------------------

	auto moveMeshObject = std::make_unique<cMoveToCommand>(meshName, meshDestinationPos, timeTaken, easeIn, easeOut);

	//--------------------------------Check if group already exists---------------------------------------------

	bool groupAlreadyExists = false;

	for (const auto& commandGroup : groupAnimationsList)
	{	
		// If 'yes' push into serial/parallel group
		if (commandGroup->GetGroupName() == groupName)
		{
			groupAlreadyExists = true;

			if (groupType == "PARALLEL")
			{
				commandGroup->MakeGroupParallel();
				commandGroup->groupParallelAnimationsList.push_back(std::move(moveMeshObject));
			}

			else if (groupType == "SERIAL")
			{
				commandGroup->MakeGroupSerial();
				commandGroup->groupSerialAnimationsList.push_back(std::move(moveMeshObject));
			}

			else
			{
				std::cout << "THE GROUP TYPE MENTIONED IN THE SCRIPT IS WRONG !" << std::endl;
				return -1;
			}
			
			break;
		}
	}

	// If 'no' create command group with all relevant details
	if(!groupAlreadyExists)
	{
		auto animationCommandsGroup = std::make_unique<cAnimationCommandGroup>();

		animationCommandsGroup->SetGroupName(groupName);
		animationCommandsGroup->SetUniqueId(uniqueIdGenerator.GenerateUnqiueID());

		if (groupType == "SERIAL")
		{
			animationCommandsGroup->MakeGroupSerial();
			animationCommandsGroup->groupSerialAnimationsList.push_back(std::move(moveMeshObject));
		}

		else if (groupType == "PARALLEL")
		{
			animationCommandsGroup->MakeGroupParallel();
			animationCommandsGroup->groupParallelAnimationsList.push_back(std::move(moveMeshObject));
		}

		else
		{
			std::cout << "THE GROUP TYPE MENTIONED IN THE SCRIPT IS WRONG !" << std::endl;
			return -1;
		}

		//-------------------Push the commands group to the main animations group---------------------------------------------

		groupAnimationsList.push_back(std::move(animationCommandsGroup));
	}

	return 0;
}

int Lua_OrientCommand(lua_State* L)
{
	//---------------------------Initialize and retrieve all parameters-----------------------------------------

	glm::vec3 rotationDirection;

	const char* meshName(lua_tostring(L, 1));
	float rotationDegrees = (float)lua_tonumber(L, 2);
	rotationDirection.x = (float)lua_tonumber(L, 3);
	rotationDirection.y = (float)lua_tonumber(L, 4);
	rotationDirection.z = (float)lua_tonumber(L, 5);
	float timeTaken = (float)lua_tonumber(L, 6);
	std::string groupType(lua_tostring(L, 7));
	const char* groupName(lua_tostring(L, 8));

	groupType = toUpperCase(groupType);

	//------------------------------------Create rotate command---------------------------------------------------

	auto rotateMeshObject = std::make_unique<cOrientToCommand>(meshName, rotationDegrees, rotationDirection, timeTaken);

	//--------------------------------Check if group already exists---------------------------------------------

	bool groupAlreadyExists = false;

	for (const auto& commandGroup : groupAnimationsList)
	{
		// If 'yes' push into serial/parallel group
		if (commandGroup->GetGroupName() == groupName)
		{
			groupAlreadyExists = true;

			if (groupType == "PARALLEL")
			{
				commandGroup->MakeGroupParallel();
				commandGroup->groupParallelAnimationsList.push_back(std::move(rotateMeshObject));
			}

			else if (groupType == "SERIAL")
			{
				commandGroup->MakeGroupSerial();
				commandGroup->groupSerialAnimationsList.push_back(std::move(rotateMeshObject));
			}

			else
			{
				std::cout << "THE GROUP TYPE MENTIONED IN THE SCRIPT IS WRONG !" << std::endl;
				return -1;
			}

			break;
		}
	}

	// If 'no' create command group with all relevant details
	if (!groupAlreadyExists)
	{
		auto animationCommandsGroup = std::make_unique<cAnimationCommandGroup>();

		animationCommandsGroup->SetGroupName(groupName);
		animationCommandsGroup->SetUniqueId(uniqueIdGenerator.GenerateUnqiueID());

		if (groupType == "SERIAL")
		{
			animationCommandsGroup->MakeGroupSerial();
			animationCommandsGroup->groupSerialAnimationsList.push_back(std::move(rotateMeshObject));
		}

		else if (groupType == "PARALLEL")
		{
			animationCommandsGroup->MakeGroupParallel();
			animationCommandsGroup->groupParallelAnimationsList.push_back(std::move(rotateMeshObject));
		}

		else
		{
			std::cout << "THE GROUP TYPE MENTIONED IN THE SCRIPT IS WRONG !" << std::endl;
			return -1;
		}

		//-------------------Push the commands group to the main animations group---------------------------------------------

		groupAnimationsList.push_back(std::move(animationCommandsGroup));
	}

	return 0;
}

int Lua_CamMoveCommand(lua_State* L)
{
	//---------------------------Initialize and retrieve all parameters---------------------------------------------

	glm::vec3 camDestinationPos;

	camDestinationPos.x = (float)lua_tonumber(L, 1);
	camDestinationPos.y = (float)lua_tonumber(L, 2);
	camDestinationPos.z = (float)lua_tonumber(L, 3);
	float timeTaken = (float)lua_tonumber(L, 4);

	//------------------------------------Create Cam Move command---------------------------------------------------

	cCameraAnimation camAnimation;
	
	camAnimation.IsSingleMovement(false);

	camAnimation.MoveAnimationCamDetails(camDestinationPos, timeTaken);

	//--------------------------Push the command to the main command group------------------------------------------

	camAnimationList.push_back(camAnimation); 

	return 0;
}

int Lua_CamPositionCommand(lua_State* L)
{
	//---------------------------Initialize and retrieve all parameters---------------------------------------------

	glm::vec3 camPosition;

	camPosition.x = (float)lua_tonumber(L, 1);
	camPosition.y = (float)lua_tonumber(L, 2);
	camPosition.z = (float)lua_tonumber(L, 3);

	//------------------------------------Create Cam Position command---------------------------------------------------

	cCameraAnimation camInstantPosition;

	camInstantPosition.IsSingleMovement(true);

	camInstantPosition.PositionCam(camPosition);

	//--------------------------Push the command to the main command group------------------------------------------

	camAnimationList.push_back(camInstantPosition);

	return 0;
}

int Lua_CamLookAtCommand(lua_State* L)
{
	//---------------------------Initialize and retrieve all parameters---------------------------------------------

	glm::vec3 camLookAt;

	camLookAt.x = (float)lua_tonumber(L, 1);
	camLookAt.y = (float)lua_tonumber(L, 2);
	camLookAt.z = (float)lua_tonumber(L, 3);

	//------------------------------------Create Cam LookAt command---------------------------------------------------

	cCameraAnimation camInstantLookAt;

	camInstantLookAt.IsSingleMovement(true);

	camInstantLookAt.CamLookAtTarget(camLookAt);

	//--------------------------Push the command to the main command group------------------------------------------

	camAnimationList.push_back(camInstantLookAt);

	return 0;
}

int Lua_CamLookAtAnimationCommand(lua_State* L)
{
	//---------------------------Initialize and retrieve all parameters---------------------------------------------

	glm::vec3 camLookAt;

	const char* meshToLookAt(lua_tostring(L, 1));
	float animationTime = (float)lua_tonumber(L, 2);

	//------------------------------------Create Cam LookAt command---------------------------------------------------

	cCameraAnimation camLookAtAnimation;

	camLookAtAnimation.IsSingleMovement(false);

	camLookAtAnimation.LookAtAnimationCamDetails(meshToLookAt, animationTime);

	//--------------------------Push the command to the main command group------------------------------------------

	camAnimationList.push_back(camLookAtAnimation);

	return 0;
}

int Lua_CurveCommand(lua_State* L)
{	
	//---------------------------Initialize and retrieve all parameters-----------------------------------------

	glm::vec3 controlPoint01, controlPoint02, meshDestinationPos;

	const char* meshName(lua_tostring(L, 1));
	controlPoint01.x = (float)lua_tonumber(L, 2);
	controlPoint01.y = (float)lua_tonumber(L, 3);
	controlPoint01.z = (float)lua_tonumber(L, 4);
	controlPoint02.x = (float)lua_tonumber(L, 5);
	controlPoint02.y = (float)lua_tonumber(L, 6);
	controlPoint02.z = (float)lua_tonumber(L, 7);
	meshDestinationPos.x = (float)lua_tonumber(L, 8);
	meshDestinationPos.y = (float)lua_tonumber(L, 9);
	meshDestinationPos.z = (float)lua_tonumber(L, 10);
	float timeTaken = (float)lua_tonumber(L, 11);
	std::string groupType(lua_tostring(L, 12));
	const char* groupName(lua_tostring(L, 13));

	groupType = toUpperCase(groupType);

	//------------------------------------Create Curve Command---------------------------------------------------

	auto followCurvePattern = std::make_unique<cFollowCurveCommand>(meshName, controlPoint01, controlPoint02, meshDestinationPos, timeTaken);

	//--------------------------------Check if group already exists---------------------------------------------

	bool groupAlreadyExists = false;

	for (const auto& commandGroup : groupAnimationsList)
	{
		// If 'yes' push into serial/parallel group
		if (commandGroup->GetGroupName() == groupName)
		{
			groupAlreadyExists = true;

			if (groupType == "PARALLEL")
			{
				commandGroup->MakeGroupParallel();
				commandGroup->groupParallelAnimationsList.push_back(std::move(followCurvePattern));
			}

			else if (groupType == "SERIAL")
			{
				commandGroup->MakeGroupSerial();
				commandGroup->groupSerialAnimationsList.push_back(std::move(followCurvePattern));
			}

			else
			{
				std::cout << "THE GROUP TYPE MENTIONED IN THE SCRIPT IS WRONG !" << std::endl;
				return -1;
			}

			break;
		}
	}

	// If 'no' create command group with all relevant details
	if (!groupAlreadyExists)
	{
		auto animationCommandsGroup = std::make_unique<cAnimationCommandGroup>();

		animationCommandsGroup->SetGroupName(groupName);
		animationCommandsGroup->SetUniqueId(uniqueIdGenerator.GenerateUnqiueID());

		if (groupType == "SERIAL")
		{
			animationCommandsGroup->MakeGroupSerial();
			animationCommandsGroup->groupSerialAnimationsList.push_back(std::move(followCurvePattern));
		}

		else if (groupType == "PARALLEL")
		{
			animationCommandsGroup->MakeGroupParallel();
			animationCommandsGroup->groupParallelAnimationsList.push_back(std::move(followCurvePattern));
		}

		else
		{
			std::cout << "THE GROUP TYPE MENTIONED IN THE SCRIPT IS WRONG !" << std::endl;
			return -1;
		}

		//-------------------Push the commands group to the main animations group---------------------------------------------

		groupAnimationsList.push_back(std::move(animationCommandsGroup));
	}

	return 0;


}

int Lua_FollowObjectCommand(lua_State* L)
{
	//---------------------------Initialize and retrieve all parameters-----------------------------------------

	glm::vec3 followOffset;

	const char* meshName(lua_tostring(L, 1));
	const char* followMeshName(lua_tostring(L, 2));
	float followDistance = (float)lua_tonumber(L, 3);
	followOffset.x = (float)lua_tonumber(L, 4);
	followOffset.y = (float)lua_tonumber(L, 5);
	followOffset.z = (float)lua_tonumber(L, 6);
	float maxSpeed = (float)lua_tonumber(L, 7);
	float followTime = (float)lua_tonumber(L, 8);
	std::string groupType(lua_tostring(L, 9));
	const char* groupName(lua_tostring(L, 10));

	groupType = toUpperCase(groupType);

	//------------------------------------Create Move Command---------------------------------------------------

	auto followObjectCommand = std::make_unique<cFollowObjectCommand>(meshName, followMeshName, followDistance, followOffset, maxSpeed, followTime);

	//--------------------------------Check if group already exists---------------------------------------------

	bool groupAlreadyExists = false;

	for (const auto& commandGroup : groupAnimationsList)
	{
		// If 'yes' push into serial/parallel group
		if (commandGroup->GetGroupName() == groupName)
		{
			groupAlreadyExists = true;

			if (groupType == "PARALLEL")
			{
				commandGroup->MakeGroupParallel();
				commandGroup->groupParallelAnimationsList.push_back(std::move(followObjectCommand));
			}

			else if (groupType == "SERIAL")
			{
				commandGroup->MakeGroupSerial();
				commandGroup->groupSerialAnimationsList.push_back(std::move(followObjectCommand));
			}

			else
			{
				std::cout << "THE GROUP TYPE MENTIONED IN THE SCRIPT IS WRONG !" << std::endl;
				return -1;
			}

			break;
		}
	}

	// If 'no' create command group with all relevant details
	if (!groupAlreadyExists)
	{
		auto animationCommandsGroup = std::make_unique<cAnimationCommandGroup>();

		animationCommandsGroup->SetGroupName(groupName);
		animationCommandsGroup->SetUniqueId(uniqueIdGenerator.GenerateUnqiueID());

		if (groupType == "SERIAL")
		{
			animationCommandsGroup->MakeGroupSerial();
			animationCommandsGroup->groupSerialAnimationsList.push_back(std::move(followObjectCommand));
		}

		else if (groupType == "PARALLEL")
		{
			animationCommandsGroup->MakeGroupParallel();
			animationCommandsGroup->groupParallelAnimationsList.push_back(std::move(followObjectCommand));
		}

		else
		{
			std::cout << "THE GROUP TYPE MENTIONED IN THE SCRIPT IS WRONG !" << std::endl;
			return -1;
		}

		//-------------------Push the commands group to the main animations group---------------------------------------------

		groupAnimationsList.push_back(std::move(animationCommandsGroup));
	}

	return 0;

}

int Lua_ChangeColorCommand(lua_State* L)
{
	//---------------------------Initialize and retrieve all parameters-----------------------------------------

	glm::vec3 meshColor;

	const char* meshName(lua_tostring(L, 1));
	meshColor.x = (float)lua_tonumber(L, 2);
	meshColor.y = (float)lua_tonumber(L, 3);
	meshColor.z = (float)lua_tonumber(L, 4);
	std::string groupType(lua_tostring(L, 5));
	const char* groupName(lua_tostring(L, 6));

	groupType = toUpperCase(groupType);

	//------------------------------------Create Move Command---------------------------------------------------

	auto changeObjectColor = std::make_unique<cChangeColorCommand>(meshName, meshColor);

	//--------------------------------Check if group already exists---------------------------------------------

	bool groupAlreadyExists = false;

	for (const auto& commandGroup : groupAnimationsList)
	{
		// If 'yes' push into serial/parallel group
		if (commandGroup->GetGroupName() == groupName)
		{
			groupAlreadyExists = true;

			if (groupType == "PARALLEL")
			{
				commandGroup->MakeGroupParallel();
				commandGroup->groupParallelAnimationsList.push_back(std::move(changeObjectColor));
			}

			else if (groupType == "SERIAL")
			{
				commandGroup->MakeGroupSerial();
				commandGroup->groupSerialAnimationsList.push_back(std::move(changeObjectColor));
			}

			else
			{
				std::cout << "THE GROUP TYPE MENTIONED IN THE SCRIPT IS WRONG !" << std::endl;
				return -1;
			}

			break;
		}
	}

	// If 'no' create command group with all relevant details
	if (!groupAlreadyExists)
	{
		auto animationCommandsGroup = std::make_unique<cAnimationCommandGroup>();

		animationCommandsGroup->SetGroupName(groupName);
		animationCommandsGroup->SetUniqueId(uniqueIdGenerator.GenerateUnqiueID());

		if (groupType == "SERIAL")
		{
			animationCommandsGroup->MakeGroupSerial();
			animationCommandsGroup->groupSerialAnimationsList.push_back(std::move(changeObjectColor));
		}

		else if (groupType == "PARALLEL")
		{
			animationCommandsGroup->MakeGroupParallel();
			animationCommandsGroup->groupParallelAnimationsList.push_back(std::move(changeObjectColor));
		}

		else
		{
			std::cout << "THE GROUP TYPE MENTIONED IN THE SCRIPT IS WRONG !" << std::endl;
			return -1;
		}

		//-------------------Push the commands group to the main animations group---------------------------------------------

		groupAnimationsList.push_back(std::move(animationCommandsGroup));
	}
}

int Lua_SelfDestructCommand(lua_State* L)
{
	const char* meshName(lua_tostring(L, 1));
	std::string groupType(lua_tostring(L, 2));
	const char* groupName(lua_tostring(L, 3));

	groupType = toUpperCase(groupType);

	//------------------------------------Create Move Command---------------------------------------------------

	auto selfDestructObject = std::make_unique<cSelfDestroyCommand>(meshName);

	//--------------------------------Check if group already exists---------------------------------------------

	bool groupAlreadyExists = false;

	for (const auto& commandGroup : groupAnimationsList)
	{
		// If 'yes' push into serial/parallel group
		if (commandGroup->GetGroupName() == groupName)
		{
			groupAlreadyExists = true;

			if (groupType == "PARALLEL")
			{
				commandGroup->MakeGroupParallel();
				commandGroup->groupParallelAnimationsList.push_back(std::move(selfDestructObject));
			}

			else if (groupType == "SERIAL")
			{
				commandGroup->MakeGroupSerial();
				commandGroup->groupSerialAnimationsList.push_back(std::move(selfDestructObject));
			}

			else
			{
				std::cout << "THE GROUP TYPE MENTIONED IN THE SCRIPT IS WRONG !" << std::endl;
				return -1;
			}

			break;
		}
	}

	// If 'no' create command group with all relevant details
	if (!groupAlreadyExists)
	{
		auto animationCommandsGroup = std::make_unique<cAnimationCommandGroup>();

		animationCommandsGroup->SetGroupName(groupName);
		animationCommandsGroup->SetUniqueId(uniqueIdGenerator.GenerateUnqiueID());

		if (groupType == "SERIAL")
		{
			animationCommandsGroup->MakeGroupSerial();
			animationCommandsGroup->groupSerialAnimationsList.push_back(std::move(selfDestructObject));
		}

		else if (groupType == "PARALLEL")
		{
			animationCommandsGroup->MakeGroupParallel();
			animationCommandsGroup->groupParallelAnimationsList.push_back(std::move(selfDestructObject));
		}

		else
		{
			std::cout << "THE GROUP TYPE MENTIONED IN THE SCRIPT IS WRONG !" << std::endl;
			return -1;
		}

		//-------------------Push the commands group to the main animations group---------------------------------------------

		groupAnimationsList.push_back(std::move(animationCommandsGroup));
	}

	return 0;
}
