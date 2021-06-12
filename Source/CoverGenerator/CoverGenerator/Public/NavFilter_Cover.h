#pragma once
#include "NavFilters/NavigationQueryFilter.h"
#include "NavAreas/NavArea.h"
#include "NavFilter_Cover.generated.h"

/** Special area that can be generated in spaces with insufficient free height above. Cannot be traversed by anyone. */
UCLASS(Config = Engine)
class COVERGENERATOR_API UNavArea_Cover : public UNavArea
{
	GENERATED_UCLASS_BODY()

};


/** Class containing definition of a navigation query filter */
UCLASS(Blueprintable)
class COVERGENERATOR_API UNavFilter_Cover : public UNavigationQueryFilter
{
	GENERATED_UCLASS_BODY()

	UNavFilter_Cover();

};