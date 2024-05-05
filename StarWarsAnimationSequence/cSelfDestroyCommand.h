#pragma once
#include <cControlGameEngine.h>
#include "iAnimationCommand.h"

class cSelfDestroyCommand : public iAnimationCommand
{
public:
	cSelfDestroyCommand(const char* modelName)
	{
		this->mModelName = modelName;
	}

	bool UpdateAnimation(cControlGameEngine& gameEngine) override
	{
		std::string explosionModelName = "ExplosionSphere";
		
		if (!bExplosionCreated)
		{
			std::string modelName(this->mModelName);

			glm::vec3 modelPos = gameEngine.GetModelPosition(modelName);

			gameEngine.LoadModelsInto3DSpace("Sphere_1_unit_Radius_uv.ply", explosionModelName, modelPos.x, modelPos.y, modelPos.z);

			this->mScaleValue = 1.0f;

			gameEngine.TurnWireframeModeOn(explosionModelName);

			gameEngine.UseManualColors(explosionModelName, true);

			gameEngine.ChangeColor(explosionModelName, 0.57, 0.38, 0.57);

			gameEngine.DeleteMesh(modelName);

			bExplosionCreated = true;
		}

		gameEngine.ScaleModel(explosionModelName, this->mScaleValue);

		this->mScaleValue += 3.f;

		if (this->mScaleValue >= 180.0f)
		{
			gameEngine.DeleteMesh(explosionModelName);
			return true;
		}

		return false;
	}

private:
	cSelfDestroyCommand() {};

	const char* mModelName;
	bool bExplosionCreated = false;

	float mScaleValue = 0.f;
};