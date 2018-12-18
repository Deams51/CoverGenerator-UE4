// Copyright (c) 2016 Mickaël Fourgeaud

#include "EnvQueryGenerator_CoverFMemory.h"
#include "EnvironmentQuery/Contexts/EnvQueryContext_Querier.h"
#include "CoverGenerator.h"
#include "EnvQueryItemType_Cover.h"


#define LOCTEXT_NAMESPACE "EnvQueryGenerator_CoverFMemory"

UEnvQueryGenerator_CoverFMemory::UEnvQueryGenerator_CoverFMemory(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	ItemType = UEnvQueryItemType_Cover::StaticClass();
	GenerateAround = UEnvQueryContext_Querier::StaticClass();
	SquareExtent.DefaultValue = 750.f;
	BoxHeight.DefaultValue = 200.f;
}


void UEnvQueryGenerator_CoverFMemory::GenerateItems(FEnvQueryInstance& QueryInstance) const
{
	UObject* BindOwner = QueryInstance.Owner.Get();

	SquareExtent.BindData(BindOwner, QueryInstance.QueryID);
	const float SquareE = SquareExtent.GetValue();

	BoxHeight.BindData(BindOwner, QueryInstance.QueryID);
	const float BoxH = BoxHeight.GetValue();


	TArray<FVector> ContextLocations;
	QueryInstance.PrepareContext(GenerateAround, ContextLocations);

	ACoverGenerator* CoverGenerator = ACoverGenerator::GetCoverGenerator(QueryInstance.World);
	if (!CoverGenerator)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s Failed to retrieve CoverGenerator."), *GetNameSafe(this));
		return;
	}

	for (int32 ContextIndex = 0; ContextIndex < ContextLocations.Num(); ContextIndex++)
	{
		TArray<UCoverPoint*> Covers = CoverGenerator->GetCoverWithinBounds(FBoxCenterAndExtent(ContextLocations[ContextIndex], FVector(SquareE, SquareE, BoxH)));
		QueryInstance.AddItemData<UEnvQueryItemType_Cover>(Covers);
	}
}

FText UEnvQueryGenerator_CoverFMemory::GetDescriptionTitle() const
{
	return FText::Format(LOCTEXT("DescriptionGenerateAroundContext", "{0}: generate around {1}"),
		Super::GetDescriptionTitle(), UEnvQueryTypes::DescribeContext(GenerateAround));
};

FText UEnvQueryGenerator_CoverFMemory::GetDescriptionDetails() const
{
	FText Desc = FText::Format(LOCTEXT("SimpleGridDescription", "Extent: {0}, {0}, {1}"),
		FText::FromString(SquareExtent.ToString()),
		FText::FromString(BoxHeight.ToString()));
	return Desc;
}


#undef LOCTEXT_NAMESPACE