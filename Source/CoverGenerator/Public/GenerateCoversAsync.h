// Copyright (2017) Ivan Stummer - All rights reserved.

#pragma once

#include "AsyncWork.h"
#include "Runtime/NavigationSystem/Public/NavigationSystem.h"

class ACoverGenerator;

class FGenerateCoversAsyncTask : public FNonAbandonableTask
{
public:

	friend class FAutoDeleteAsyncTask<FGenerateCoversAsyncTask>;

	ACoverGenerator* CoverGenerator;
	FRecastDebugGeometry NavMeshGeometry;

	FGenerateCoversAsyncTask(ACoverGenerator* CoverGenerator, FRecastDebugGeometry& NavMeshGeometry)
	{
		this->CoverGenerator = CoverGenerator;
		this->NavMeshGeometry = NavMeshGeometry;
	}

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