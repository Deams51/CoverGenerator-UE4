// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EnvironmentQuery/EnvQueryTest.h"
#include "EnvQueryTest_FilterCoverPoints.generated.h"

/**
 * 
 */
UCLASS()
class COVERGENERATOR_API UEnvQueryTest_FilterCoverPoints : public UEnvQueryTest
{
	GENERATED_UCLASS_BODY()

	//UPROPERTY(EditDefaultsOnly, Category = Filter)
	//bool bAllFlagsTogether = true;

	UPROPERTY(EditDefaultsOnly, Category = Filter)
	bool bFrontCoverCrouched;

	UPROPERTY(EditDefaultsOnly, Category = Filter)
	bool bLeftCoverCrouched;

	UPROPERTY(EditDefaultsOnly, Category = Filter)
	bool bRightCoverCrouched;

	UPROPERTY(EditDefaultsOnly, Category = Filter)
	bool bLeftCoverStanding;

	UPROPERTY(EditDefaultsOnly, Category = Filter)
	bool bRightCoverStanding;
	
	virtual void RunTest(FEnvQueryInstance& QueryInstance) const override;
	
};
