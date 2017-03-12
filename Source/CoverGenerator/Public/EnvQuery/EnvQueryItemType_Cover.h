// Copyright (c) 2016 Mickaël Fourgeaud

#pragma once

#include "EnvironmentQuery/Items/EnvQueryItemType_VectorBase.h"
#include "CoverPoint.h"
#include "EnvQueryItemType_Cover.generated.h"

struct FBlackboardKeySelector;
class UBlackboardComponent; 


UCLASS()
class UEnvQueryItemType_Cover : public UEnvQueryItemType_VectorBase
{
	GENERATED_UCLASS_BODY()

public:
	typedef const UCoverPoint* FValueType;

	static UCoverPoint* GetValue(const uint8* RawData);
	static void SetValue(uint8* RawData, const UCoverPoint* Value);

	virtual FVector GetItemLocation(const uint8* RawData) const override;

	virtual void AddBlackboardFilters(FBlackboardKeySelector& KeySelector, UObject* FilterOwner) const override;
	virtual bool StoreInBlackboard(FBlackboardKeySelector& KeySelector, UBlackboardComponent* Blackboard, const uint8* RawData) const override;
	virtual FString GetDescription(const uint8* RawData) const override;
};
