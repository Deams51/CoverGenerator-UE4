// Copyright (c) 2016 Mickaël Fourgeaud

#pragma once

#include "GameFramework/Actor.h"
#include "Runtime/Engine/Public/GenericOctree.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "Runtime/NavigationSystem/Public/NavigationSystem.h"
#include "AI/Navigation/NavAgentInterface.h"
#include "Navmesh/RecastNavMesh.h"
#include "CoverPoint.h" 
#include "GenerateCoversAsync.h"

#include "CoverGenerator.generated.h"


struct FCoverPointOctreeElement
{
	FBoxSphereBounds Bounds;

	/** Cover Point these data represents */
	const TWeakObjectPtr<UCoverPoint> CoverPoint;

	FCoverPointOctreeElement(UCoverPoint& CoverPointIn)
		:CoverPoint(&CoverPointIn)
	{
		Bounds = FBoxSphereBounds(&CoverPoint->Location, 1);
	}

	FORCEINLINE UCoverPoint* GetOwner() const { return CoverPoint.Get(); }
};

struct FCoverPointOctreeSemantics
{
	enum { MaxElementsPerLeaf = 16 };
	enum { MinInclusiveElementsPerNode = 7 };
	enum { MaxNodeDepth = 12 };

	/** Using the heap allocator instead of an inline allocator to trade off add/remove performance for memory. */
	/** Since we won't generate covers after init, should be ok. */
	typedef FDefaultAllocator ElementAllocator;

	FORCEINLINE static bool AreElementsEqual(const FCoverPointOctreeElement& A, const FCoverPointOctreeElement& B)
	{
		return A.CoverPoint == B.CoverPoint;
	}

	static void SetElementId(const FCoverPointOctreeElement& Element, FOctreeElementId Id)
	{
	}

	FORCEINLINE static const FBoxSphereBounds& GetBoundingBox(const FCoverPointOctreeElement& CoverData)
	{
		return CoverData.Bounds;
	}
};

typedef TOctree<FCoverPointOctreeElement, FCoverPointOctreeSemantics> FCoverPointOctree;

UCLASS()
class COVERGENERATOR_API ACoverGenerator : public AActor
{
	GENERATED_UCLASS_BODY()

protected:

	UPROPERTY(Category = Default, EditAnywhere)
	bool ForceRefresh = false;

	UPROPERTY(Category = Default, EditAnywhere)
	bool bRegenerateAtBeginPlay = true;
	
	/** If the cover generation should be regenerated when navigation is rebuilt at runtime */
	UPROPERTY(Category = Default, EditAnywhere)
	bool bRegenerateAtNavigationRebuilt = false;

	UPROPERTY(Category = Default, EditAnywhere)
	float SegmentLength = 10; 

	UPROPERTY(Category = Default, EditAnywhere)
	float MaxSegDist = 350;

	UPROPERTY(Category = Default, EditAnywhere)
	float CharHeight = 180;

	UPROPERTY(Category = Default, EditAnywhere)
	float StepDistTrace = 20;

	UPROPERTY(Category = Default, EditAnywhere)
	float TraceLength = 100;

	UPROPERTY(Category = Default, EditAnywhere)
	float MinSpaceBetweenValidPoints = 50.f;
	
	/** Size of the spheres used to sphere-cast */
	UPROPERTY(Category = Default, EditAnywhere)
	float SphereSize = 5;

	/** The maximum height of the agents when crouching */
	UPROPERTY(Category = Default, EditAnywhere)
	float WidthMaxAgent = 80;

	/** The maximum height of the agents when crouching */
	UPROPERTY(Category = Default, EditAnywhere)
	float HeightMaxCrouching = 80;

	/** The maximum height of the agents when standing */
	UPROPERTY(Category = Default, EditAnywhere)
	float HeightMaxStanding = 180;

	/** The distance between in cover position and leaning out of the cover on the sides */
	UPROPERTY(Category = Default, EditAnywhere)
	float OffsetWhenLeaningSides = 65;

	/** The distance in front of a shooting position that must be free */
	UPROPERTY(Category = Default, EditAnywhere)
	float OffsetFrontAim = 100;


	/** Debug */

	/** Draw a green sphere for the start points, a red for the end and blue for pieces. */
	UPROPERTY(Category = Debug, EditAnywhere)
	bool bDraw1AllSegmentPointsTested = false;

	/** Draw a green sphere when the point is not within bounds of an existing one, otherwise red. */
	UPROPERTY(Category = Debug, EditAnywhere)
	bool bDraw2SegmentPointsWithinBounds = false;

	/** Draw a green sphere when the point passed the test, or red if failed the first line trace, orange if failed one of the sub line trace. */
	UPROPERTY(Category = Debug, EditAnywhere)
	bool bDraw3SimpleCoverGeometryTest = false;

	/** Draw a red sphere if the position failed the side trace as well as the trace in blue and the collision in red. Green sphere is succeeded. */
	UPROPERTY(Category = Debug, EditAnywhere)
	bool bDraw4SecondPassTracesSides = false;
	
	/** Draw a green if successful, or orange sphere + red trace if failed side bottom or side front trace. */
	UPROPERTY(Category = Debug, EditAnywhere)
	bool bDraw4SecondPassTracesSidesFrontAndBottom = false;

	UPROPERTY(Category = Debug, EditAnywhere)
	bool bDraw5SecondPassArrows = false;

	// Display all the points found in the world
	UPROPERTY(Category = Debug, EditAnywhere)
	bool DebugDrawAllPoints = false;

	// Display all the points currently used in the world
	UPROPERTY(Category = Debug, EditAnywhere)
	bool DebugDrawAllBusyPoints = false;

	/** Toggle the text log outputs */
	UPROPERTY(Category = Debug, EditAnywhere)
	bool DebugDisplayLog = false;

	/** The maximum distance from this actor at which to draw the debug information */
	UPROPERTY(Category = Debug, EditAnywhere)
	float DebugDistance = 25000.f;

	/** Toggle the display of the Octree boundaries */
	UPROPERTY(Category = Debug, EditAnywhere)
	bool bDrawOctreeBounds = false;
	
	
private:
	// Octree of all the generated cover points
	FCoverPointOctree* CoverPointOctree; 

	/* Array of all cover points, used to keep GC from collecting the cp in Octree */
	UPROPERTY(Transient)
	TArray<UCoverPoint*> AllCoverPoints;

	/* Array of all currently used as coverPoints */
	UPROPERTY(Transient)
	TArray<UCoverPoint*> CoverPointsCurrentlyUsed;

	/** Do we have covers? */
	bool HasGeneratedCovers; 

public:

	// Sets default values for this actor's properties
	ACoverGenerator();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	/** Called when begin to destroy this actor */
	virtual void BeginDestroy() override;

	/** Allow game tick in viewport */
	virtual bool ShouldTickIfViewportsOnly() const override { return true; }

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif // WITH_EDITOR

	void DrawDebugCover(UWorld* World, const FVector& StartLocation, const FVector& Direction1, const FVector Direction2, const float& DistanceToDraw);

	// Return world navigation mesh data 
	ARecastNavMesh* GetNavMeshData(UWorld* World);

	UFUNCTION(BlueprintCallable, Category = "CoverGenerator")
	bool OccupyCover(UCoverPoint* CoverPoint, class AController* Controller);

	UFUNCTION(BlueprintCallable, Category = "CoverGenerator")
	void ReleaseCover(UCoverPoint* CoverPoint);

	UFUNCTION(BlueprintCallable, Category = "CoverGenerator")
	bool IsFreeCoverPoint(const UCoverPoint* CoverPoint);
	
	UFUNCTION(BlueprintCallable, Category = "CoverGenerator")
	TArray<UCoverPoint*> GetCoverWithinBox(const FBox& BoxIn);

	UFUNCTION(BlueprintCallable, Category = "CoverGenerator")
	bool CoverExistWithinBox(const FBox& BoxIn);

	UFUNCTION(BlueprintPure, Category = "CoverGenerator", meta = (WorldContext = "WorldContextObject"))
	static ACoverGenerator* GetCoverGenerator(UObject* WorldContextObject);

	bool IsProvidingCover(UWorld* World, const FVector SegmentPoint, const FVector SegmentDirection, const FVector TraceVec, const float StartHeight, const float EndHeight);
	
	bool TwoPassesCheck(UWorld* World, const FVector& SegmentPoint, const FVector& FirstTestDirection, const float& VerticalOffset, const FVector& Perp);
	
	UCoverPoint* IsValidCoverPoint(UWorld* World, const FVector& SegmentPoint, const FVector& Segment, const FVector& TraceVec, const FVector& Perp);

	void TestAndAddPoint(const FVector SegmentPoint, FVector Segment, UWorld* World, FVector Perp);

	FVector Get2DPerpVector(const FVector& v1) const;

	void GenerateCovers(bool ForceRegeneration = false, bool DoAsync = false);

	// can be called async
	void AnalizeMeshData(FRecastDebugGeometry& NavMeshGeometry);

	bool MeOcuppying(UCoverPoint* CoverPoint, AController* Controller);

	void DrawOctreeBounds();

	TArray<FCoverPointOctreeElement> GetOctreeElementsWithinBounds(const FBoxCenterAndExtent& BoundsIn);
	TArray<UCoverPoint*> GetCoverWithinBounds(const FBoxCenterAndExtent& BoundsIn);
	bool CoverExistWithinBounds(const FBoxCenterAndExtent& BoundsIn);

private:

	UFUNCTION()
	void OnNavigationGenerationFinished(class ANavigationData* NavData);

	// required by async task:
public:

	bool bIsRefreshing;
	bool bIsRefreshed;
};