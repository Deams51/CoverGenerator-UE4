// Copyright (c) 2016 Mickaël Fourgeaud

#include "EnvQueryTest_CoverPointFree.h"
#include "CoverPoint.h"
#include "EnvQueryItemType_Cover.h"
#include "CoverGenerator.h"



UEnvQueryTest_CoverPointFree::UEnvQueryTest_CoverPointFree(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	Cost = EEnvTestCost::Low;
	ValidItemType = UEnvQueryItemType_Cover::StaticClass();
	SetWorkOnFloatValues(false);
}

void UEnvQueryTest_CoverPointFree::RunTest(FEnvQueryInstance& QueryInstance) const
{
	ACoverGenerator* CoverGenerator = ACoverGenerator::GetCoverGenerator(QueryInstance.World);
	if (CoverGenerator)
	{
		for (FEnvQueryInstance::ItemIterator It(this, QueryInstance); It; ++It)
		{
			// Get current CoverPoint
			UCoverPoint* CurrentCoverPoint = UEnvQueryItemType_Cover::GetValue(QueryInstance.RawData.GetData() + QueryInstance.Items[It.GetIndex()].DataOffset);
			bool IsFree = CoverGenerator->IsFreeCoverPoint(CurrentCoverPoint); 

			It.SetScore(TestPurpose, FilterType, IsFree, true);
		}
	}
}
