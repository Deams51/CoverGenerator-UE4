// Copyright (c) 2016 Mickaël Fourgeaud

#include "EnvQueryTest_TraceDistance.h"
#include "EnvironmentQuery/Contexts/EnvQueryContext_Querier.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_VectorBase.h"
#include "EnvQueryTest_IsCoverPosition.h"
#include "WorldCollision.h"
#include "Engine/World.h"


UEnvQueryTest_TraceDistance::UEnvQueryTest_TraceDistance(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	Cost = EEnvTestCost::High;
	ValidItemType = UEnvQueryItemType_VectorBase::StaticClass();
	SetWorkOnFloatValues(false);

	Context = UEnvQueryContext_Querier::StaticClass();
	TraceData.SetGeometryOnly();
}

void UEnvQueryTest_TraceDistance::RunTest(FEnvQueryInstance& QueryInstance) const
{
	UObject* DataOwner = QueryInstance.Owner.Get();
	BoolValue.BindData(DataOwner, QueryInstance.QueryID);
	MaxTraceDistance.BindData(DataOwner, QueryInstance.QueryID);
	VerticalOffset.BindData(DataOwner, QueryInstance.QueryID);

	const bool bWantsHit = BoolValue.GetValue();
	const float TraceDistance = MaxTraceDistance.GetValue();
	const float VerticalOff = VerticalOffset.GetValue(); 

	TArray<FVector> ContextLocations;
	if (!QueryInstance.PrepareContext(Context, ContextLocations))
	{
		return;
	}

	FCollisionQueryParams TraceParams(TEXT("EnvQueryTrace"), TraceData.bTraceComplex);
	TraceParams.bTraceAsyncScene = true;

	TArray<AActor*> IgnoredActors;
	if (QueryInstance.PrepareContext(Context, IgnoredActors))
	{
		TraceParams.AddIgnoredActors(IgnoredActors);
	}

	ECollisionChannel TraceCollisionChannel = UEngineTypes::ConvertToCollisionChannel(TraceData.TraceChannel);

	for (FEnvQueryInstance::ItemIterator It(this, QueryInstance); It; ++It)
	{
		const FVector ItemLocation = GetItemLocation(QueryInstance, It.GetIndex());
		AActor* ItemActor = GetItemActor(QueryInstance, It.GetIndex());

		for (int32 ContextIndex = 0; ContextIndex < ContextLocations.Num(); ContextIndex++)
		{
			// Calculate end trace distance 
			FVector Direction = ContextLocations[ContextIndex] - ItemLocation;
			Direction.Normalize(); 
			const FVector StarTrace = ItemLocation + FVector(0, 0, VerticalOff); 
			const FVector EndTrace = StarTrace + TraceDistance * Direction;

			// Perform trace
			const bool bHit = RunLineTraceTo(StarTrace, EndTrace, ItemActor, QueryInstance.World, TraceCollisionChannel, TraceParams);

			It.SetScore(TestPurpose, FilterType, bHit, bWantsHit);
		}
	}
}


bool UEnvQueryTest_TraceDistance::RunLineTraceTo(const FVector& ItemPos, const FVector& ContextPos, AActor* ItemActor, UWorld* World, enum ECollisionChannel Channel, const FCollisionQueryParams& Params) const
{
	FCollisionQueryParams TraceParams(Params);
	TraceParams.AddIgnoredActor(ItemActor);

	const bool bHit = World->LineTraceTestByChannel(ContextPos, ItemPos, Channel, TraceParams);
	return bHit;
}


