#include "pch.h"
#include "cAnimationCommandGroup.h"

bool cAnimationCommandGroup::IsSerialGroup()
{
	return this->bIsSerialAnimation;
}

bool cAnimationCommandGroup::IsParallelGroup()
{
	return this->bIsParallelAnimation;
}

void cAnimationCommandGroup::MakeGroupSerial()
{
	this->bIsSerialAnimation = true;
}

void cAnimationCommandGroup::MakeGroupParallel()
{
	this->bIsParallelAnimation = true;
}

void cAnimationCommandGroup::SetGroupName(const char* groupName)
{
	this->mGroupName = groupName;
}
std::string cAnimationCommandGroup::GetGroupName()
{
	std::string commandGroupName(this->mGroupName);
	
	return commandGroupName;
}

void cAnimationCommandGroup::SetUniqueId(unsigned int uniqueId)
{
	this->mUniqueId = uniqueId;
}

unsigned int cAnimationCommandGroup::GetUniqueId()
{
	return this->mUniqueId;
}
