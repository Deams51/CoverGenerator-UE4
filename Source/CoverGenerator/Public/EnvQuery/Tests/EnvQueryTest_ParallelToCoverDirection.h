// Copyright (c) 2016 Mickaël Fourgeaud

#pragma once

#include "EnvironmentQuery/EnvQueryTest.h"
#include "EnvQueryTest_ParallelToCoverDirection.generated.h"

/**
 * 
 */
UCLASS()
class UEnvQueryTest_ParallelToCoverDirection : public UEnvQueryTest
{
	GENERATED_UCLASS_BODY()

	/** context: other end of trace test */
	UPROPERTY(EditDefaultsOnly, Category = Trace)
	TSubclassOf<UEnvQueryContext> Context;

	/** max trace distance */
	UPROPERTY(EditDefaultsOnly, Category = Trace)
	FAIDataProviderFloatValue MaxEpsilon;

	virtual void RunTest(FEnvQueryInstance& QueryInstance) const override;
};
