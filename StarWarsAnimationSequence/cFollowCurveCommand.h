#pragma once
#include <cControlGameEngine.h>
#include "iAnimationCommand.h"

class cFollowCurveCommand : public iAnimationCommand
{
public:
	cFollowCurveCommand(const char* modelName, glm::vec3 controlPoint1, glm::vec3 controlPoint2, glm::vec3 endingPos, float timeToMove);

	bool UpdateAnimation(cControlGameEngine& gameEngine) override;

private:
	cFollowCurveCommand() {};

	bool bValuesCalculated = false;

	unsigned int mUniqueId = 0;

	const char* mMeshName = "";

	float mTimeToMove = 0.f;
	float mSpeed = 0.f;

	double mElapsedTime = 0.0;

	std::vector <glm::vec3> mControlPointsList;

	glm::vec3 mStartPosition = glm::vec3(0.f);
	glm::vec3 mEndPosition = glm::vec3(0.f);
};

