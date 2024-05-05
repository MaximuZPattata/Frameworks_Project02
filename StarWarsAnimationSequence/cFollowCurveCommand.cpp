#include "pch.h"
#include "cFollowCurveCommand.h"

cFollowCurveCommand::cFollowCurveCommand(const char* modelName, glm::vec3 controlPoint1, glm::vec3 controlPoint2, glm::vec3 endingPos, float timeToMove)
{
	this->mMeshName = modelName;
	this->mTimeToMove = timeToMove;
	this->mEndPosition = endingPos;

	this->mControlPointsList.push_back(controlPoint1);
	this->mControlPointsList.push_back(controlPoint2);
}

bool cFollowCurveCommand::UpdateAnimation(cControlGameEngine& gameEngine)
{
	this->mElapsedTime += gameEngine.deltaTime;

	std::string modelName(this->mMeshName);

	if (!bValuesCalculated)
	{
		this->mStartPosition = gameEngine.GetModelPosition(modelName);
		bValuesCalculated = true;
	}

	//---------Calculating position along Bezier Curve based on given control points-------------------------

	float timeFactor = this->mElapsedTime / this->mTimeToMove;
	float timeDifference = 1.0f - timeFactor;

	glm::vec3 point1 = timeDifference * timeDifference * timeDifference * this->mStartPosition;
	glm::vec3 point2 = 3.0f * timeDifference * timeDifference * timeFactor * this->mControlPointsList[0];
	glm::vec3 point3 = 3.0f * timeDifference * timeFactor * timeFactor * this->mControlPointsList[1];
	glm::vec3 point4 = timeFactor * timeFactor * timeFactor * this->mEndPosition;

	glm::vec3 finalPos = point1 + point2 + point3 + point4;

	gameEngine.MoveModel(modelName, finalPos.x, finalPos.y, finalPos.z);

	////-----------------------Calculating rotation along the curve--------------------------------------------
	//
	//point1 = 3.0f * timeDifference * timeDifference * (this->mControlPointsList[0] - this->mStartPosition);
	//point2 = 6.0f * timeDifference * timeFactor * (this->mControlPointsList[1] - this->mControlPointsList[0]);
	//point3 = 3.0f * timeFactor * timeFactor * (this->mEndPosition - this->mControlPointsList[1]);

	//glm::vec3 tangent = glm::normalize(point1 + point2 + point3);

	//glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f); 
	//glm::vec3 rotationAxis = glm::cross(upVector, tangent);

	//float rotationDegrees = glm::degrees(glm::acos(glm::dot(upVector, tangent)));

	//gameEngine.RotateMeshModel(modelName, rotationDegrees, rotationAxis.x, rotationAxis.y, rotationAxis.z);

	//-------------------Check if command has completed implementation---------------------------------------

	if (this->mElapsedTime >= this->mTimeToMove)
		return true;

	return false;
}
