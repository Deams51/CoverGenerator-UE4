// Copyright (c) 2016 Mickaël Fourgeaud

#pragma once

#include "EnvironmentQuery/EnvQueryTest.h"
#include "EnvQueryTest_IsCoverPosition.generated.h"

/**
 * 
 */
UCLASS()
class UEnvQueryTest_IsCoverPosition : public UEnvQueryTest
{
	GENERATED_UCLASS_BODY()

	/** trace data */
	UPROPERTY(EditDefaultsOnly, Category = Trace)
	FEnvTraceData TraceData;

	/** context: other end of trace test */
	UPROPERTY(EditDefaultsOnly, Category = Trace)
	TSubclassOf<UEnvQueryContext> Context;

	/** distance offset from querier */
	UPROPERTY(EditDefaultsOnly, Category = Trace, AdvancedDisplay)
	FAIDataProviderFloatValue ContextHorizontalDistanceOffset;

	/** distance offset from querier */
	UPROPERTY(EditDefaultsOnly, Category = Trace, AdvancedDisplay)
	FAIDataProviderFloatValue ContextVerticalDistanceOffset;

	/** Display debug info */
	UPROPERTY(EditDefaultsOnly, Category = Trace, AdvancedDisplay)
	FAIDataProviderBoolValue DebugData;

	virtual void RunTest(FEnvQueryInstance& QueryInstance) const override;
	
protected:
	bool RunLineTraceTo(const FVector& ItemPos, const FVector& ContextPos, AActor* ItemActor, UWorld* World, enum ECollisionChannel Channel, const FCollisionQueryParams& Params, bool Debug = false) const;

	static FVector PerpendicularClockwise(FVector V);
};
