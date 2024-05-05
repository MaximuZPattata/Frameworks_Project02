#pragma once

#include <cControlGameEngine.h>
#include "iAnimationCommand.h"

class cOrientToCommand : public iAnimationCommand
{
public:
	cOrientToCommand(const char* modelName, float degrees, glm::vec3 rotationDirection, float timeToRotate);

	bool UpdateAnimation(cControlGameEngine& gameEngine) override;

private:
	cOrientToCommand() {};

	unsigned int mUniqueId = 0;

	std::string mMeshName;

	float mTotalDegreesToBeRotated = 0.f;
	float mTimeToRotate = 0.f;

	double mElapsedTime = 0.0;

	glm::vec3 mRotationDirection = glm::vec3(0.f);
};
