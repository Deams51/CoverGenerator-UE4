// Copyright (c) 2016 Mickaël Fourgeaud

#pragma once

#include "CoreMinimal.h"
#include "ModuleManager.h"

class COVERGENERATOR_API FCoverGeneratorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};