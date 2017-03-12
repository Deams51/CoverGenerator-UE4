// Copyright (c) 2016 Mickaël Fourgeaud

#pragma once

#include "EnvironmentQuery/EnvQueryTest.h"
#include "EnvQueryTest_TraceDistance.generated.h"

/**
 * 
 */
UCLASS()
class UEnvQueryTest_TraceDistance : public UEnvQueryTest
{
	GENERATED_UCLASS_BODY()
	
	/** trace data */
	UPROPERTY(EditDefaultsOnly, Category = Trace)
	FEnvTraceData TraceData;

	/** context: other end of trace test */
	UPROPERTY(EditDefaultsOnly, Category = Trace)
	TSubclassOf<UEnvQueryContext> Context;

	/** max trace distance */
	UPROPERTY(EditDefaultsOnly, Category = Trace)
	FAIDataProviderFloatValue MaxTraceDistance;

	UPROPERTY(EditDefaultsOnly, Category = Trace)
	FAIDataProviderFloatValue VerticalOffset;

	virtual void RunTest(FEnvQueryInstance& QueryInstance) const override;

protected:
	bool RunLineTraceTo(const FVector& ItemPos, const FVector& ContextPos, AActor* ItemActor, UWorld* World, enum ECollisionChannel Channel, const FCollisionQueryParams& Params) const;

};
