#pragma once
#include <cControlGameEngine.h>
#include "iAnimationCommand.h"

class cMoveToCommand : public iAnimationCommand
{
public:
	cMoveToCommand(const char* modelName, glm::vec3 endXYZ, float timeToMove, float easeIn, float easeOut);

	bool UpdateAnimation(cControlGameEngine& gameEngine) override;

private:
	cMoveToCommand() {};

	bool bValuesCalculated = false;
	bool bEaseValuesCalculated = false;

	unsigned int mUniqueId = 0;
	
	const char* mMeshName = "";

	float mTimeToMove = 0.f;
	float mSpeed = 0.f;
	float mEaseIn = 0.f;
	float mEaseOut = 0.f;

	double mElapsedTime = 0.0;

	glm::vec3 mEndingPos = glm::vec3(0.f);
	glm::vec3 mTotalDistance = glm::vec3(0.f);
	glm::vec3 mMoveDirection = glm::vec3(0.f);
	glm::vec3 mVelocity = glm::vec3(0.f);
};
