
#include "NavFilter_Cover.h"
#include "NavAreas/NavArea_Default.h"


UNavArea_Cover::UNavArea_Cover(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	DrawColor = FColor(125, 125, 0, 164);

}


UNavFilter_Cover::UNavFilter_Cover(const FObjectInitializer& ObjectInitializer) //: Super(ObjectInitializer)
{
	FNavigationFilterArea Nav_Default;
	Nav_Default.AreaClass = UNavArea_Default::StaticClass();
	Nav_Default.bOverrideTravelCost = true;
	Nav_Default.TravelCostOverride = FLT_MAX;

	FNavigationFilterArea Nav_Cover;
	Nav_Cover.AreaClass = UNavArea_Cover::StaticClass();
	Nav_Cover.bOverrideTravelCost = true;
	Nav_Cover.TravelCostOverride = 1.0f;

	Areas.Add(Nav_Default);
	Areas.Add(Nav_Cover);
}