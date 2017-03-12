// Copyright (c) 2016 Mickaël Fourgeaud

#pragma once

#include "EnvironmentQuery/EnvQueryGenerator.h"
#include "DataProviders/AIDataProvider.h"
#include "EnvQueryGenerator_CoverFMemory.generated.h"

/**
 * 
 */
UCLASS()
class UEnvQueryGenerator_CoverFMemory : public UEnvQueryGenerator
{
	GENERATED_UCLASS_BODY()

	/** Square's extent */
	UPROPERTY(EditDefaultsOnly, Category = Generator)
	FAIDataProviderFloatValue SquareExtent;

	/** Box's height */
	UPROPERTY(EditDefaultsOnly, Category = Generator)
	FAIDataProviderFloatValue BoxHeight;

	/** context */
	UPROPERTY(EditDefaultsOnly, Category = Generator)
	TSubclassOf<UEnvQueryContext> GenerateAround;

	virtual void GenerateItems(FEnvQueryInstance& QueryInstance) const override;

	virtual FText GetDescriptionTitle() const override;
	virtual FText GetDescriptionDetails() const override;
	
};
