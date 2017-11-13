// Copyright (2017) Ivan Stummer - All rights reserved.

#pragma once

#include "AsyncWork.h"
#include "Runtime/Engine/Classes/AI/Navigation/NavigationSystem.h"
//#include "GenerateCoversAsyncTask.generated.h" // NO !


class FGenerateCoversAsyncTask : public FNonAbandonableTask
{
public:

	friend class FAutoDeleteAsyncTask<FGenerateCoversAsyncTask>;

	// data
	class ACoverGenerator* CoverGenerator;
	FRecastDebugGeometry NavMeshGeometry;

	//---

	// constructor
	FGenerateCoversAsyncTask(ACoverGenerator* TheCoverGenerator, FRecastDebugGeometry& TheNavMeshGeometry)
	{
		CoverGenerator = TheCoverGenerator;
		NavMeshGeometry = TheNavMeshGeometry;
	}

	// destructor
	~FGenerateCoversAsyncTask()
	{
		CoverGenerator = nullptr;
	}

	/*This function is needed from the API of the engine.
	My guess is that it provides necessary information
	about the thread that we occupy and the progress of our task*/
	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(FGenerateCoversAsyncTask, STATGROUP_ThreadPoolAsyncTasks);
	}

	//---

	/*This function is executed when we tell our task to execute*/
	void DoWork();
};