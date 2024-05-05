#include "pch.h"
#include "cFollowObjectCommand.h"

cFollowObjectCommand::cFollowObjectCommand(const char* modelName, const char* objectNameToFollow, float followDistance, glm::vec3 followOffset,
	float maxSpeed, float followTime)
{
	this->mMeshName = modelName;
	this->mFollowObjectName = objectNameToFollow;
	this->mFollowDistance = followDistance;
	this->mFollowOffset = followOffset;
	this->mMaxSpeed = maxSpeed;
	this->mFollowTime = followTime;
}

bool cFollowObjectCommand::UpdateAnimation(cControlGameEngine& gameEngine)
{
    this->mElapsedTime += gameEngine.deltaTime;

    std::string modelName(this->mMeshName);
    std::string followObjectName(this->mFollowObjectName);

    glm::vec3 currentMeshPosition = gameEngine.GetModelPosition(modelName);
    glm::vec3 targetMeshPosition = gameEngine.GetModelPosition(followObjectName);

    glm::vec3 targetWithOffset = targetMeshPosition + this->mFollowOffset;

    glm::vec3 direction = glm::normalize(targetWithOffset - currentMeshPosition);

    float totalDistance = glm::distance(targetWithOffset, currentMeshPosition);

    if (totalDistance < this->mFollowDistance)
        totalDistance = this->mFollowDistance;

    this->mSpeed = totalDistance / this->mFollowTime;

    /*if (this->mSpeed > this->mMaxSpeed)
        this->mSpeed = this->mMaxSpeed;*/

    glm::vec3 velocity = direction * this->mSpeed * 3.0f;

    // Mesh Position
    currentMeshPosition += (velocity * (float)gameEngine.deltaTime);

     // Mesh Orientation
   // glm::quat quatRotation = glm::quatLookAt(direction, glm::vec3(0.0f, 1.0f, 0.0f));

    gameEngine.MoveModel(modelName, currentMeshPosition.x, currentMeshPosition.y, currentMeshPosition.z);
    //gameEngine.RotateMeshModelWithQuaternion(modelName, quatRotation);

    if (this->mElapsedTime >= this->mFollowTime)
        return true;

    return false;
}