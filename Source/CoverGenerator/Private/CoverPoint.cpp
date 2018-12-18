// Copyright (c) 2016 Mickaël Fourgeaud

#include "CoverPoint.h"


UCoverPoint::UCoverPoint()
	: Location(FVector::ZeroVector), _DirectionToWall(FVector::ZeroVector)
{

}

UCoverPoint::UCoverPoint(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer), Location(FVector::ZeroVector), _DirectionToWall(FVector::ZeroVector)
{

}

UCoverPoint::UCoverPoint(const FVector & InLocation, const FVector & InDirectionToWall)
	: Location(InLocation), _DirectionToWall(InDirectionToWall)
{

}

UCoverPoint::UCoverPoint(const UCoverPoint& InCopy)
	: Location(InCopy.Location), _DirectionToWall(InCopy.DirectionToWall())
{

}
