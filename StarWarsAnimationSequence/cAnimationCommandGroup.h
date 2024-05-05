#pragma once
#include "pch.h"
#include "iAnimationCommand.h"

class cAnimationCommandGroup
{
public:
	std::vector<std::unique_ptr<iAnimationCommand>> groupSerialAnimationsList;
	std::vector<std::unique_ptr<iAnimationCommand>> groupParallelAnimationsList;

	bool IsSerialGroup();
	bool IsParallelGroup();
	
	void MakeGroupSerial();
	void MakeGroupParallel();

	void SetGroupName(const char* groupName);
	std::string GetGroupName();

	void SetUniqueId(unsigned int uniqueId);
	unsigned int GetUniqueId();

private:
	unsigned int mUniqueId = 0;

	const char* mGroupName;

	bool bIsSerialAnimation = false;
	bool bIsParallelAnimation = false;
};

