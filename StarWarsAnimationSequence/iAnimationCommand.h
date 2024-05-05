#pragma once

#include <cControlGameEngine.h>
#include <string>

class iAnimationCommand
{
public:
	virtual ~iAnimationCommand() = default;

	virtual bool UpdateAnimation(cControlGameEngine& gameEngine) = 0;
};