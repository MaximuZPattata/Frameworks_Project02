#include "pch.h"
#include "cMoveToCommand.h"

cMoveToCommand::cMoveToCommand(const char* modelName, glm::vec3 endXYZ, float timeToMove, float easeIn, float easeOut)
{
	this->mMeshName = modelName;
	this->mEndingPos = endXYZ;
	this->mTimeToMove = timeToMove;
	this->mEaseIn = easeIn;
	this->mEaseOut = easeOut;
}

bool cMoveToCommand::UpdateAnimation(cControlGameEngine& gameEngine)
{
	this->mElapsedTime += gameEngine.deltaTime;

	std::string modelName(this->mMeshName);

	glm::vec3 currentXYZ = gameEngine.GetModelPosition(modelName);

	//--------Check if physics values are calculated already--------------

	if (!bValuesCalculated)
	{
		if (this->mEaseIn <= 0.f && this->mEaseOut <= 0.f)
			this->bEaseValuesCalculated = true;

		this->mTotalDistance = this->mEndingPos - currentXYZ;

		this->mMoveDirection = glm::normalize(this->mTotalDistance);

		float totalDistance = glm::distance(this->mEndingPos, currentXYZ);
		this->mSpeed = totalDistance / this->mTimeToMove;

		if (this->mEaseIn > 0.f)
		{
			float easeInFactor = glm::clamp((float)this->mElapsedTime / this->mEaseIn, 0.0f, 1.0f);

			float easedSpeed = this->mSpeed * (1.0f - std::cos(easeInFactor * glm::pi<float>()) * 0.5f);

			this->mVelocity = this->mMoveDirection * easedSpeed;
		}

		else
			this->mVelocity = this->mMoveDirection * this->mSpeed;

		this->bValuesCalculated = true;
	}

	

	if (!this->bEaseValuesCalculated)
	{
		if (this->mEaseIn > 0.f && this->mElapsedTime >= this->mEaseIn)
		{
			this->mVelocity = this->mMoveDirection * this->mSpeed;

			if (this->mEaseOut <= 0.f)
				this->bEaseValuesCalculated = true;
		}

		if (this->mEaseOut > 0.f && (this->mTimeToMove - this->mElapsedTime) <= this->mEaseOut)
		{
			float easeOutFactor = glm::clamp((this->mTimeToMove - (float)this->mElapsedTime) / this->mEaseOut, 0.0f, 1.0f);

			float easedSpeed = this->mSpeed * (1.0f - std::cos(easeOutFactor * glm::pi<float>()) * 0.5f);

			this->mVelocity = this->mMoveDirection * easedSpeed;

			this->bEaseValuesCalculated = true;
		}
	}

	//--------Calculate current position and move model-----------------

	currentXYZ += (this->mVelocity * (float)gameEngine.deltaTime);

	gameEngine.MoveModel(modelName, currentXYZ.x, currentXYZ.y, currentXYZ.z);

	if (this->mElapsedTime >= this->mTimeToMove)
		return true;

	return false;
}


