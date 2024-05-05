#pragma once
#include "pch.h"
#include <cControlGameEngine.h>

class cCameraAnimation
{
public:
	void MoveAnimationCamDetails(glm::vec3 endPos, float animationTime)
	{
		this->bIsMoveAnimation = true;
		this->mAnimationTime = animationTime;
		this->mCamEndPos = endPos;
	}

	void LookAtAnimationCamDetails(const char* meshToLookAt, float animationTime)
	{
		this->bIsLookAtAnimation = true;
		this->mAnimationTime = animationTime;
		this->mModelToLookAt = meshToLookAt;
	}

	void IsSingleMovement(bool isOneTake)
	{
		this->bIsSingleMovement = isOneTake;
	}

	void PositionCam(glm::vec3 camPos)
	{
		this->mCamEndPos.x = camPos.x;
		this->mCamEndPos.y = camPos.y;
		this->mCamEndPos.z = camPos.z;
	}

	void CamLookAtTarget(glm::vec3 camLookAt)
	{
		this->mCamLookAt.x = camLookAt.x;
		this->mCamLookAt.y = camLookAt.y;
		this->mCamLookAt.z = camLookAt.z;
	}

	bool UpdateCameraAnimation(cControlGameEngine& gameEngine)
	{
		if (!bIsSingleMovement)
		{
			this->mElapsedTime += gameEngine.deltaTime;

			//---------------------Cam Move Animation--------------------------------

			if (bIsMoveAnimation)
			{
				glm::vec3 cameraPos = gameEngine.GetCurrentCameraPosition();

				if (!bIsValuesCalculated)
				{
					this->mTotalDistance = this->mCamEndPos - cameraPos;

					this->mMoveDirection = glm::normalize(this->mTotalDistance);

					float totalDistance = glm::distance(this->mCamEndPos, cameraPos);
					this->mSpeed = totalDistance / this->mAnimationTime;

					this->mVelocity = this->mMoveDirection * this->mSpeed;

					bIsValuesCalculated = true;
				}

				cameraPos += (this->mVelocity * (float)gameEngine.deltaTime);

				gameEngine.MoveCameraPosition(cameraPos.x, cameraPos.y, cameraPos.z);
			}

			//---------------------Cam Look At Animation-----------------------------

			else if (bIsLookAtAnimation)
			{
				std::string modelName(this->mModelToLookAt);

				glm::vec3 cameraPos = gameEngine.GetCurrentCameraPosition();
				glm::vec3 targetPos = gameEngine.GetModelPosition(modelName);

				glm::vec3 frontDirection = glm::normalize(targetPos - cameraPos);

				gameEngine.MoveCameraTarget(frontDirection.x, frontDirection.y, frontDirection.z);
			}

			//--------------------------Check Timer----------------------------------

			if (this->mElapsedTime >= this->mAnimationTime)
				return true;
		}
		else
		{
			if (mCamEndPos != glm::vec3(0.f))
				gameEngine.MoveCameraPosition(mCamEndPos.x, mCamEndPos.y, mCamEndPos.z);

			else if (mCamLookAt != glm::vec3(0.f))
				gameEngine.MoveCameraTarget(mCamLookAt.x, mCamLookAt.y, mCamLookAt.z);

			return true;
		}

		return false;
	}

private:
	glm::vec3 mCamEndPos = glm::vec3(0.f);
	glm::vec3 mCamLookAt = glm::vec3(0.f);
	glm::vec3 mTotalDistance = glm::vec3(0.f);
	glm::vec3 mMoveDirection = glm::vec3(0.f);
	glm::vec3 mVelocity = glm::vec3(0.f);

	const char* mModelToLookAt;

	bool bIsSingleMovement = false;
	bool bIsValuesCalculated = false;
	bool bIsMoveAnimation = false;
	bool bIsLookAtAnimation = false;

	unsigned int mUniqueId = 0;

	double mElapsedTime = 0.0;
	float mAnimationTime = 0.f;
	float mSpeed = 0.f;
};
