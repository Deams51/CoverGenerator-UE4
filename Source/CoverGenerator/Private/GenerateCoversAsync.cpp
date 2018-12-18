// Copyright (2017) Ivan Stummer - All rights reserved.

#include "GenerateCoversAsync.h"
#include "CoverGenerator.h"


void FGenerateCoversAsyncTask::DoWork()
{
	if (!CoverGenerator) return;

	// signal of start processing
	CoverGenerator->bIsRefreshing = true;

	CoverGenerator->AnalizeMeshData(NavMeshGeometry);

	// signal of end processing
	CoverGenerator->bIsRefreshed = true;
	CoverGenerator->bIsRefreshing = false;

	GLog->Log("--------------------------------------------------------------------");
	GLog->Log("End of cover generation async");
	GLog->Log("--------------------------------------------------------------------");
}

