// Fill out your copyright notice in the Description page of Project Settings.

#include "CoverGeneratorPrivatePCH.h"
#include "EnvQueryTest_FilterCoverPoints.h"
#include "EnvQueryItemType_Cover.h"


UEnvQueryTest_FilterCoverPoints::UEnvQueryTest_FilterCoverPoints(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	Cost = EEnvTestCost::Low;
	ValidItemType = UEnvQueryItemType_Cover::StaticClass();
	SetWorkOnFloatValues(false);
}

void UEnvQueryTest_FilterCoverPoints::RunTest(FEnvQueryInstance& QueryInstance) const
{
	UObject* QueryOwner = QueryInstance.Owner.Get();
	if (QueryOwner == nullptr)
	{
		return;
	}

	BoolValue.BindData(QueryOwner, QueryInstance.QueryID);
	bool bWantsValid = BoolValue.GetValue();

	for (FEnvQueryInstance::ItemIterator It(this, QueryInstance); It; ++It)
	{
		UCoverPoint* CurrentCoverPoint = UEnvQueryItemType_Cover::GetValue(QueryInstance.RawData.GetData() + QueryInstance.Items[It.GetIndex()].DataOffset);

		bool Uno = false;
		if (bFrontCoverCrouched)
		{
			Uno = CurrentCoverPoint->bFrontCoverCrouched && !CurrentCoverPoint->bLeftCoverStanding && !CurrentCoverPoint->bRightCoverStanding;
		}

		bool Dos = false;
		if (bLeftCoverCrouched)
		{
			Dos = CurrentCoverPoint->bLeftCoverCrouched && !CurrentCoverPoint->bLeftCoverStanding && !CurrentCoverPoint->bRightCoverStanding;
		}

		bool Tres = false;
		if (bRightCoverCrouched)
		{
			Tres = CurrentCoverPoint->bRightCoverCrouched && !CurrentCoverPoint->bLeftCoverStanding && !CurrentCoverPoint->bRightCoverStanding;
		}

		bool Cuatro = false;
		if (bLeftCoverStanding)
		{
			Cuatro = CurrentCoverPoint->bLeftCoverStanding;
		}

		bool Cinco = false;
		if (bRightCoverStanding)
		{
			Cinco = CurrentCoverPoint->bRightCoverStanding;
		}

		bool bCoverWithFilter = Uno || Dos || Tres || Cuatro || Cinco;
		
		It.SetScore(TestPurpose, FilterType, bCoverWithFilter, bWantsValid);
	}
}
