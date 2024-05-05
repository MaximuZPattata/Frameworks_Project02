#pragma once
#include <cControlGameEngine.h>
#include "iAnimationCommand.h"

class cFollowObjectCommand : public iAnimationCommand
{
public:
	cFollowObjectCommand(const char* modelName, const char* objectNameToFollow, float followDistance, glm::vec3 followOffset, 
						float maxSpeed, float followTime);

	bool UpdateAnimation(cControlGameEngine& gameEngine) override;

private:
	cFollowObjectCommand() {};

	bool bValuesCalculated = false;

	unsigned int mUniqueId = 0;

	const char* mMeshName = "";
	const char* mFollowObjectName = "";

	float mFollowTime = 0.f;
	float mSpeed = 0.f;
	float mFollowDistance = 0.f;
	float mMaxSpeed = 0.f;

	double mElapsedTime = 0.0;

	glm::vec3 mFollowOffset = glm::vec3(0.f);
};

