#include "pch.h"
#include "cOrientToCommand.h"

cOrientToCommand::cOrientToCommand(const char* modelName, float degrees, glm::vec3 rotationDirection, float timeToRotate)
{
	this->mTotalDegreesToBeRotated = degrees;
	this->mTimeToRotate = timeToRotate;
	this->mRotationDirection = rotationDirection;
	this->mMeshName = modelName;
}

bool cOrientToCommand::UpdateAnimation(cControlGameEngine& gameEngine)
{
	this->mElapsedTime += gameEngine.deltaTime;

	std::string modelName(this->mMeshName);

	float rotationDegrees = ((mTotalDegreesToBeRotated / mTimeToRotate) * (float)gameEngine.deltaTime);

	gameEngine.RotateMeshModel(modelName, rotationDegrees, this->mRotationDirection.x, this->mRotationDirection.y, this->mRotationDirection.z);

	if (this->mElapsedTime >= this->mTimeToRotate)
		return true;

	return false;
}

