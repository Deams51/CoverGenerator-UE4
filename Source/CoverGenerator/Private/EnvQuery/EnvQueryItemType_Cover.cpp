// Copyright (c) 2016 Mickaël Fourgeaud

#include "EnvQueryItemType_Cover.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"

UEnvQueryItemType_Cover::UEnvQueryItemType_Cover(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	ValueSize = sizeof(FWeakObjectPtr);
}

UCoverPoint* UEnvQueryItemType_Cover::GetValue(const uint8* RawData)
{
	FWeakObjectPtr WeakObjPtr = GetValueFromMemory<FWeakObjectPtr>(RawData);
	return (UCoverPoint*)(WeakObjPtr.Get());
}

void UEnvQueryItemType_Cover::SetValue(uint8* RawData, const UCoverPoint* Value)
{
	FWeakObjectPtr WeakObjPtr(Value);
	SetValueInMemory<FWeakObjectPtr>(RawData, WeakObjPtr);
}

FVector UEnvQueryItemType_Cover::GetItemLocation(const uint8 * RawData) const
{
	UCoverPoint* MyCover = UEnvQueryItemType_Cover::GetValue(RawData);
	return MyCover ? MyCover->Location : FVector::ZeroVector;
}

void UEnvQueryItemType_Cover::AddBlackboardFilters(FBlackboardKeySelector& KeySelector, UObject* FilterOwner) const
{
	Super::AddBlackboardFilters(KeySelector, FilterOwner);
	KeySelector.AddObjectFilter(FilterOwner, GetClass()->GetFName(), AActor::StaticClass());
}

bool UEnvQueryItemType_Cover::StoreInBlackboard(FBlackboardKeySelector& KeySelector, UBlackboardComponent* Blackboard, const uint8* RawData) const
{
	bool bStored = Super::StoreInBlackboard(KeySelector, Blackboard, RawData);
	if (!bStored && KeySelector.SelectedKeyType == UBlackboardKeyType_Object::StaticClass())
	{
		UObject* MyObject = GetValue(RawData);
		Blackboard->SetValue<UBlackboardKeyType_Object>(KeySelector.GetSelectedKeyID(), MyObject);

		bStored = true;
	}

	return bStored;
}

FString UEnvQueryItemType_Cover::GetDescription(const uint8* RawData) const
{
	const UCoverPoint* Cover = GetValue(RawData);
	return GetNameSafe(Cover);
}
