#pragma once
#include <cControlGameEngine.h>
#include "iAnimationCommand.h"

class cChangeColorCommand : public iAnimationCommand
{
public:
	cChangeColorCommand(const char* modelName, glm::vec3 meshColor)
	{
		this->mModelName = modelName;
		this->mMeshColor = meshColor;
	}

	bool UpdateAnimation(cControlGameEngine& gameEngine) override
	{
		std::string modelName(this->mModelName);

		gameEngine.UseManualColors(modelName, true);

		gameEngine.ChangeColor(modelName, this->mMeshColor.r, this->mMeshColor.g, this->mMeshColor.b);

		return true;
	}

private:
	cChangeColorCommand() {};

	const char* mModelName;
	glm::vec3 mMeshColor = glm::vec3(0.f);
};