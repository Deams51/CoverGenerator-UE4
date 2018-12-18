// Copyright (c) 2016 Mickaël Fourgeaud

#pragma once

#include "Object.h"
#include "CoverPoint.generated.h"

class AController;

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class COVERGENERATOR_API UCoverPoint : public UObject
{
	GENERATED_BODY()	
	
public: 

	/** location of the cover */
	UPROPERTY(BlueprintReadWrite, Category = "Cover")
	FVector Location;

	/** Direction to wall (Perpendicular to cover) */
	UPROPERTY(BlueprintReadWrite, Category = "Cover")
	FVector _DirectionToWall;

	/** Rotator from X of direction to wall */
	UPROPERTY(BlueprintReadWrite, Category = "Cover")
	FRotator RotatorXToWall;

	/** Is it a Left cover (can lean on left) */
	UPROPERTY(BlueprintReadWrite, Category = "Cover")
	bool bLeftCoverStanding;

	/** Is it a Right cover (can lean on Right) */
	UPROPERTY(BlueprintReadWrite, Category = "Cover")
	bool bRightCoverStanding;

	/** Is it a Left cover (can lean on left) */
	UPROPERTY(BlueprintReadWrite, Category = "Cover")
	bool bLeftCoverCrouched;

	/** Is it a Right cover (can lean on Right) */
	UPROPERTY(BlueprintReadWrite, Category = "Cover")
	bool bRightCoverCrouched;

	/** Is it a Right cover (can lean on Right) */
	UPROPERTY(BlueprintReadWrite, Category = "Cover")
	bool bFrontCoverCrouched;

	/** Is it a cover requiring crouch */
	UPROPERTY(BlueprintReadWrite, Category = "Cover")
	bool bCrouchedCover;

	/** Controller to which this point has been given */
	AController* OccupiedBy; 

	/**
	* Default constructor
	*/
	UCoverPoint();

	/**
	* Constructor
	*/
	UCoverPoint(const FObjectInitializer& ObjectInitializer);

	UCoverPoint(const FVector& InLocation, const FVector& InDirectionToWall);
	
public:
	FVector DirectionToWall() const { return _DirectionToWall; }
	void DirectionToWall(FVector val) { 
		_DirectionToWall = val;
		// Set rotator from direction 
		RotatorXToWall = FRotationMatrix::MakeFromX(_DirectionToWall).Rotator();
	}
};
