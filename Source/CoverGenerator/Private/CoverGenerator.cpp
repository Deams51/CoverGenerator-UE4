// Copyright (c) 2016 Mickaël Fourgeaud

#include "CoverGenerator.h"
#include "EnvQuery/EnvQueryItemType_Cover.h"
#include "EngineUtils.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DrawDebugHelpers.h"


// Sets default values
ACoverGenerator::ACoverGenerator(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	// Simple root scene 
	RootComponent = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("RootSceneComponent"));

	// Octree
	CoverPointOctree = new FCoverPointOctree(FVector(0, 0, 0), 64000);

	// Async
	bIsRefreshed = false;
	bIsRefreshing = false;
}

// Called when the game starts or when spawned
void ACoverGenerator::BeginPlay()
{
	Super::BeginPlay();

	if (bRegenerateAtBeginPlay && ACoverGenerator::GetCoverGenerator(GetWorld()) == this)
	{
		if (!bIsRefreshing)
		{
			GenerateCovers(true, true);
		}
	}
	else if(AllCoverPoints.Num() > 0)
	{
		// Generate from saved data
		for (UCoverPoint* CoverPoint: AllCoverPoints)
		{
			CoverPointOctree->AddElement(FCoverPointOctreeElement(*CoverPoint)); 
		}
	}

	// Bind to navigation generation in case we want to rebuild covers
	if (UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld()))
		NavSystem->OnNavigationGenerationFinishedDelegate.AddDynamic(this, &ACoverGenerator::OnNavigationGenerationFinished);
}

// Called every frame
void ACoverGenerator::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
	
	// debug info
	if (bIsRefreshed)
	{
		bIsRefreshed = false;

		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("ACoverGenerator : GenerateCovers() finished ASYNC")));

		if (DebugDisplayLog)
		{
			UE_LOG(LogTemp, Log, TEXT("%s: Finished generating cover points."), *GetNameSafe(this));
			CoverPointOctree->DumpStats();
		}
	}

#if WITH_EDITOR
	// Display Debug information

	FVector VerticalOffset = FVector(0, 0, 200); 
	if (DebugDrawAllBusyPoints)
	{
		for (UCoverPoint* Cover : CoverPointsCurrentlyUsed)
		{
			if (FVector::Dist(GetActorLocation(), Cover->Location) > DebugDistance)
				continue;

			DrawDebugSphere(GetWorld(), Cover->Location + VerticalOffset, 50, 4, FColor::Red);
		}
	}

	if (DebugDrawAllPoints)
	{
		const FVector CrouchHeight = FVector(0.f, 0.f, HeightMaxCrouching);
		const FVector StandingHeight = FVector(0.f, 0.f, HeightMaxStanding);

		TArray<UCoverPoint*> PointsToDraw = GetCoverWithinBounds(FBoxCenterAndExtent(GetActorLocation(), FVector(DebugDistance)));

		for (const UCoverPoint* Cover : PointsToDraw)
		{
			if (FVector::Dist(GetActorLocation(), Cover->Location) > DebugDistance)
				continue; 


			FVector Perp = Get2DPerpVector(Cover->DirectionToWall());
			Perp.Normalize();

			const FVector Left = Perp;
			const FVector Right = -Perp;
			const FVector Front = Cover->DirectionToWall();
			const FVector Top = FVector::UpVector; 

			if (Cover->bLeftCoverCrouched || Cover->bRightCoverCrouched || Cover->bFrontCoverCrouched)
			{
				const FVector CrouchLocation = Cover->Location + CrouchHeight;

				DrawDebugSphere(GetWorld(), CrouchLocation, 30, 4, FColor::Blue);

				if (Cover->bLeftCoverCrouched)
					DrawDebugCover(GetWorld(), CrouchLocation, Left, Front, 60.f);
				if (Cover->bRightCoverCrouched)
					DrawDebugCover(GetWorld(), CrouchLocation, Right, Front, 60.f);
				if (Cover->bFrontCoverCrouched)
					DrawDebugCover(GetWorld(), CrouchLocation, Top, Front, 60.f);

			}
			if (Cover->bLeftCoverStanding || Cover->bRightCoverStanding)
			{
				const FVector StandingLocation = Cover->Location + StandingHeight; 

				DrawDebugSphere(GetWorld(), StandingLocation, 30, 4, FColor::Blue);

				if (Cover->bLeftCoverStanding)
					DrawDebugCover(GetWorld(), StandingLocation, Left, Front, 60.f);
				if (Cover->bRightCoverStanding)
					DrawDebugCover(GetWorld(), StandingLocation, Right, Front, 60.f);
			}

		}

	}

	if (bDrawOctreeBounds)
	{
		DrawOctreeBounds();
	}
#endif
}

void ACoverGenerator::BeginDestroy()
{
	// Unbind navigation delegate	
	if (UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld()))
		NavSystem->OnNavigationGenerationFinishedDelegate.RemoveDynamic(this, &ACoverGenerator::OnNavigationGenerationFinished);

	delete CoverPointOctree;

	Super::BeginDestroy();
}

void ACoverGenerator::OnNavigationGenerationFinished(class ANavigationData* NavData)
{
	if (bRegenerateAtNavigationRebuilt)
	{
		if (!bIsRefreshing)
		{			
			GenerateCovers(true, true);
		}
	}
}

void ACoverGenerator::DrawDebugCover(UWorld* World, const FVector& StartLocation, const FVector& Direction1, const FVector Direction2, const float& DistanceToDraw)
{
	const FVector End1 = StartLocation + DistanceToDraw*Direction1;

	DrawDebugDirectionalArrow(GetWorld(), StartLocation, End1, 15, FColor::Green);
	DrawDebugDirectionalArrow(GetWorld(), End1, End1 + 60.f*Direction2, 15, FColor::Green);
}

ARecastNavMesh* ACoverGenerator::GetNavMeshData(UWorld* World)
{
	const UNavigationSystemBase* NavSys = World->GetNavigationSystem();

	if (NavSys == nullptr)
	{
		return nullptr;
	}

	return const_cast<ARecastNavMesh*>(static_cast<const ARecastNavMesh*>(NavSys->GetMainNavData()));
}

ACoverGenerator* ACoverGenerator::GetCoverGenerator(UObject* WorldContextObject)
{
	if (WorldContextObject && WorldContextObject->GetWorld())
	{
		TActorIterator<ACoverGenerator> Itr(WorldContextObject->GetWorld());
		if (Itr)
		{
			return *Itr;
		}
	}
	return NULL;
}

bool ACoverGenerator::IsProvidingCover(UWorld* World, const FVector SegmentPoint, const FVector SegmentDirection, const FVector TraceVec, const float StartHeight, const float EndHeight)
{
	bool bHit = false;
	FCollisionQueryParams CollisionParams;
	const float HalfWidth = WidthMaxAgent / 2.f;

#if WITH_EDITOR
	const bool bDebugLocal = bDraw3SimpleCoverGeometryTest && FVector::Dist(GetActorLocation(), SegmentPoint) < DebugDistance;
	//if (bDebugLocal)
	//{
	//	const FName TraceTag("MyTraceTag");
	//	World->DebugDrawTraceTag = TraceTag;
	//	CollisionParams.TraceTag = TraceTag;
	//}
#endif

	// First trace to find normal to geometry
	FHitResult HitResult;
	bHit = World->LineTraceSingleByChannel(HitResult, SegmentPoint, SegmentPoint + TraceVec, ECC_WorldStatic, CollisionParams);
	if (!bHit)
	{
#if WITH_EDITOR
		if (bDebugLocal)
		{
			DrawDebugSphere(World, SegmentPoint + FVector(0.f, 0.f, StartHeight), 25, 4, FColor::Red, true, 10.f);
			DrawDebugDirectionalArrow(World, SegmentPoint, SegmentPoint + TraceVec, 10, FColor::Red, true, 10.f);
		}
#endif
		return false;
	}

	// Multi trace to ensure cover is safe using the normal to impact from previous trace as direction 
	FVector TraceDirection = TraceVec; // -HitResult.ImpactNormal;
	TraceDirection.Normalize();
	TraceDirection = TraceLength*TraceDirection; 
	// First checking for crouch
	for (float xOffset = -HalfWidth; xOffset <= HalfWidth; xOffset += StepDistTrace)
	{
		for (float zOffset = StartHeight; zOffset <= EndHeight; zOffset += StepDistTrace)
		{
			const FVector StartPos = SegmentPoint + FVector(0, 0, zOffset) + xOffset*SegmentDirection;

			bHit = World->LineTraceTestByChannel(StartPos, StartPos + TraceDirection, ECC_WorldStatic, CollisionParams);
			if (!bHit)
			{
#if WITH_EDITOR
				if (bDebugLocal)
				{
					DrawDebugSphere(World, SegmentPoint + FVector(0.f, 0.f, StartHeight + 10.f), 25, 4, FColor::Orange, true, 10.f);
					DrawDebugDirectionalArrow(World, StartPos, StartPos + TraceDirection, 10, FColor::Orange, true, 10.f);
				}
#endif
				return false;
			}
		}
	}


#if WITH_EDITOR
	if (bDebugLocal)
	{
		DrawDebugSphere(World, SegmentPoint + FVector(0.f, 0.f, StartHeight + 20.f), 25, 4, FColor::Green, true, 10.f);
	}
#endif
	
	return true;
}

bool ACoverGenerator::TwoPassesCheck(UWorld* World, const FVector& SegmentPoint, const FVector& FirstTestDirection, const float& VerticalOffset, const FVector& CoverDirection)
{
	const FVector PointOff = SegmentPoint + FVector(0.f, 0.f, VerticalOffset);
	const FVector FirstTestEndPosition = PointOff + FirstTestDirection;
	FQuat Quat = FVector::UpVector.ToOrientationRotator().Quaternion();
	Quat.Normalize();
	FHitResult HitResult;
	
	//Check if it is a left cover, a right cover, or a crouch cover
	FCollisionQueryParams CollisionParams;

#if WITH_EDITOR
	const bool bDebugLocal = FVector::Dist(GetActorLocation(), SegmentPoint) < DebugDistance;
	/*if (bDebugLocal && bDraw4SecondPassTracesSides)
	{
		const FName TraceTag("MyTraceTag");
		World->DebugDrawTraceTag = TraceTag;
		CollisionParams.TraceTag = TraceTag;
	}*/
#endif

	bool HitOnSide = World->SweepSingleByChannel(HitResult, PointOff, FirstTestEndPosition, Quat, ECC_WorldStatic, FCollisionShape::MakeSphere(SphereSize), CollisionParams);

#if WITH_EDITOR
	if (bDebugLocal && bDraw4SecondPassTracesSides)
	{
		DrawDebugSphere(World, SegmentPoint, 20, 2, HitOnSide ? FColor::Red : FColor::Green, true, 10.f);
		if (HitOnSide)
		{
			DrawDebugDirectionalArrow(World, PointOff, FirstTestEndPosition, 10, FColor::Blue, true, 10.f);
			DrawDebugDirectionalArrow(World, PointOff, HitResult.Location, 10, FColor::Red, true, 10.f);
		}
	}
#endif

	if (!HitOnSide)
	{
		FHitResult FrontResult, DownResult;

		// Side Front 
		const FVector SideFrontStart = FirstTestEndPosition;
		const FVector SideFrontEnd = SideFrontStart + OffsetFrontAim*CoverDirection;
		bool HitSideFront = World->SweepSingleByChannel(FrontResult, SideFrontStart, SideFrontEnd, Quat, ECC_WorldStatic, FCollisionShape::MakeSphere(SphereSize), CollisionParams);
		
		// Side Bottom = Floor check
		const FVector SideDownStart = PointOff + FVector(FirstTestDirection.X, FirstTestDirection.Y, 0.f);
		const FVector SideDownEnd = SideDownStart + FVector::UpVector*(-100);
		bool HitSideBottom = World->SweepSingleByChannel(DownResult, SideDownStart, SideDownEnd, Quat, ECC_WorldStatic, FCollisionShape::MakeSphere(SphereSize), CollisionParams);

#if WITH_EDITOR
		if (bDebugLocal && bDraw4SecondPassTracesSidesFrontAndBottom)
		{
			DrawDebugSphere(World, SegmentPoint, 20, 2, !HitSideFront && HitSideBottom ? FColor::Green : FColor::Red, true, 10.f);
			if (HitSideFront)
			{
				DrawDebugDirectionalArrow(World, SideFrontStart, SideFrontEnd, 10, FColor::Purple, true, 10.f);
				DrawDebugDirectionalArrow(World, SideFrontStart, FrontResult.Location, 10, FColor::Red, true, 10.f);
			}
			if (!HitSideBottom)
			{
				DrawDebugDirectionalArrow(World, SideDownStart, SideDownEnd, 10, FColor::Cyan, true, 10.f);
				DrawDebugDirectionalArrow(World, SideDownStart, SideDownEnd, 10, FColor::Red, true, 10.f);
			}
		}
#endif
		// Should not hit on side front to allow aiming, but should hit on side bottom to avoid going through the floor :P 
		if (!HitSideFront && HitSideBottom)
			return true;
	}
	return false; 
}

UCoverPoint* ACoverGenerator::IsValidCoverPoint(UWorld* World, const FVector& SegmentPoint, const FVector& Segment, const FVector& TraceVec, const FVector& Perp)
{
	FVector CoverDirection = -Perp;
	CoverDirection.Normalize();

	// Check if this position hides a crouched character when seen from the perpendicular direction 
	// That's the minimum requirement for being a cover 
	bool ProvidesCoverCrouched = IsProvidingCover(World, SegmentPoint, Segment, TraceVec, 0, HeightMaxCrouching);
	if (ProvidesCoverCrouched)
	{
		const FVector LeaningRightVec = OffsetWhenLeaningSides*(Segment); 
		const FVector LeaningLeftVec = -LeaningRightVec; 


		// Check if it is a right or left or front cover for crouched position 
		const float HeightCrouchingCheck = HeightMaxCrouching / 2.f;
		bool CanShootFromLeftCrouched = TwoPassesCheck(World, SegmentPoint, LeaningLeftVec, HeightCrouchingCheck, CoverDirection);
		bool CanShootFromRightCrouched = TwoPassesCheck(World, SegmentPoint, LeaningRightVec, HeightCrouchingCheck, CoverDirection);
		bool CanShootFromFrontCrouched = TwoPassesCheck(World, SegmentPoint, HeightMaxStanding*FVector::UpVector - 30.f, HeightCrouchingCheck, CoverDirection); // -30.f for the offset between the point and the floor

		// Check for standing (No front in that case) 
		bool CanShootFromLeftStanding = false;
		bool CanShootFromRightStanding = false; 
		bool ProvidesCoverStanding = IsProvidingCover(World, SegmentPoint, Segment, TraceVec, HeightMaxCrouching + StepDistTrace, HeightMaxStanding);
		if (ProvidesCoverStanding)
		{
			const float HeightStandingCheck = HeightMaxCrouching + ((HeightMaxStanding - HeightMaxCrouching) / 2.f);
			CanShootFromLeftStanding = CanShootFromLeftCrouched; // TwoPassesCheck(World, SegmentPoint, LeaningLeftVec, HeightStandingCheck, Perp);
			CanShootFromRightStanding = CanShootFromRightCrouched; //TwoPassesCheck(World, SegmentPoint, LeaningRightVec, HeightStandingCheck, Perp);
		}


		if ( CanShootFromLeftCrouched || CanShootFromRightCrouched || CanShootFromFrontCrouched || CanShootFromLeftStanding || CanShootFromRightStanding )
		{
			UCoverPoint* NewPoint = NewObject<UCoverPoint>();
			NewPoint->Location = SegmentPoint;
			NewPoint->DirectionToWall(CoverDirection);

			NewPoint->bLeftCoverCrouched = CanShootFromLeftCrouched; //TODO Animation unstable 
			NewPoint->bRightCoverCrouched = CanShootFromRightCrouched;
			NewPoint->bFrontCoverCrouched = CanShootFromFrontCrouched;

			NewPoint->bLeftCoverStanding = CanShootFromLeftStanding;
			NewPoint->bRightCoverStanding = CanShootFromRightStanding;

			return NewPoint;
		}
	}
	return NULL; 
}

void ACoverGenerator::TestAndAddPoint(const FVector SegmentPoint, FVector SegmentDirection, UWorld* World, FVector Perp)
{
	const bool AlreadyCoverWithinbounds = CoverExistWithinBounds(FBoxCenterAndExtent(SegmentPoint, FVector(MinSpaceBetweenValidPoints)));

#if WITH_EDITOR
	if (bDraw2SegmentPointsWithinBounds && FVector::Dist(GetActorLocation(), SegmentPoint) < DebugDistance)
	{
		DrawDebugSphere(GetWorld(), SegmentPoint, 20, 2, AlreadyCoverWithinbounds ? FColor::Red : FColor::Green, true, 10.f);
	}
#endif

	// Check that there's no point at a distance of less than MinSpaceBetweenValidPoints already added
	if(AlreadyCoverWithinbounds)
		return;

	FVector TraceDirection = Get2DPerpVector(SegmentDirection);
	TraceDirection.Normalize(); 
	FVector TraceVec = TraceLength*TraceDirection;


	// Check if valid point in first direction 
	UCoverPoint* Point = IsValidCoverPoint(World, SegmentPoint, -SegmentDirection, -TraceVec, Perp);
	// If not then let's try the other side
	if (Point == NULL)
	{
		Point = IsValidCoverPoint(World, SegmentPoint, SegmentDirection, TraceVec, Perp);
	}
	
	// If found a valid point, add to storage
	if (Point != NULL)
	{
		AllCoverPoints.Add(Point);
		FCoverPointOctreeElement CoverPointOctreeElement(*Point); 
		CoverPointOctree->AddElement(CoverPointOctreeElement); 
	}
}

FVector ACoverGenerator::Get2DPerpVector(const FVector& v1) const
{
	return FVector(v1.Y, -v1.X, 0);
}

void ACoverGenerator::GenerateCovers(bool ForceRegeneration, bool DoAsync)
{
	if (HasGeneratedCovers && !ForceRegeneration)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (World == nullptr)
	{
		return;
	}

	// Reset visual debug information
	UKismetSystemLibrary::FlushPersistentDebugLines(World);

	// Reset Octree
	CoverPointOctree->Destroy();
	delete CoverPointOctree; 
	CoverPointOctree = new FCoverPointOctree(FVector(0, 0, 0), 64000);

	// reset
	HasGeneratedCovers = false;

	// Get Navigation Mesh Data 
	ARecastNavMesh* NavMeshData = GetNavMeshData(World);
	if (NavMeshData == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s: No navigation data available!"), *GetNameSafe(this));
		return;
	}

	FRecastDebugGeometry NavMeshGeometry;
	NavMeshGeometry.bGatherNavMeshEdges = true;
	NavMeshData->BeginBatchQuery();
	NavMeshData->GetDebugGeometry(NavMeshGeometry);

	// Async processing
	if (DoAsync)
	{
		(new FAutoDeleteAsyncTask<FGenerateCoversAsyncTask>(this, NavMeshGeometry))->StartBackgroundTask();

		return;
	}
	else // use game thread
	{
		AnalizeMeshData(NavMeshGeometry);
	}

	if (DebugDisplayLog)
	{
		UE_LOG(LogTemp, Log, TEXT("%s: Finished generating cover points."), *GetNameSafe(this));
		CoverPointOctree->DumpStats(); 
	}
}

void ACoverGenerator::AnalizeMeshData(FRecastDebugGeometry& NavMeshGeometry)
{
	// Navigation mesh data analysis
	const TArray<FVector>& NavMeshEdgeVerts = NavMeshGeometry.NavMeshEdges;
	for (int32 Idx = 0; Idx < NavMeshEdgeVerts.Num(); Idx += 2)
	{
		const FNavLocation SegmentStart = FNavLocation(NavMeshEdgeVerts[Idx]);
		const FNavLocation SegmentEnd = FNavLocation(NavMeshEdgeVerts[Idx + 1]);

		const FVector Segment = SegmentEnd.Location - SegmentStart.Location;
		const float SegmentSize = Segment.Size();
		FVector Perp = Get2DPerpVector(Segment);
		FVector SegmentDirection = Segment;
		SegmentDirection.Normalize();

		// Check start and end position 			
		TestAndAddPoint(SegmentStart.Location, SegmentDirection, GetWorld(), Perp);
		TestAndAddPoint(SegmentEnd.Location, SegmentDirection, GetWorld(), Perp);

#if WITH_EDITOR
		bool bDebugDraw = false;
		if (bDraw1AllSegmentPointsTested && FVector::Dist(GetActorLocation(), SegmentStart.Location) < DebugDistance)
		{
			bDebugDraw = true;
			DrawDebugSphere(GetWorld(), SegmentStart.Location, 20, 4, FColor::Green, true, 10.f);
			DrawDebugSphere(GetWorld(), SegmentEnd.Location, 20, 4, FColor::Red, true, 10.f);
		}
#endif

		// Check inner points if segment is long enough
		if (SegmentSize > SegmentLength)
		{
			const int32 NumSegmentPieces = Segment.Size() / SegmentLength;
			for (int32 idx = 1; idx < NumSegmentPieces; idx++)
			{
				const FVector SegmentPoint = SegmentStart.Location + (idx * SegmentLength * SegmentDirection);
				TestAndAddPoint(SegmentPoint, SegmentDirection, GetWorld(), Perp);

#if WITH_EDITOR
				if (bDebugDraw)
				{
					DrawDebugSphere(GetWorld(), SegmentPoint, 20, 4, FColor::Blue, true, 10.f);
				}
#endif
			}
		}
	}

	HasGeneratedCovers = true;

	CoverPointOctree->ShrinkElements();
}

bool ACoverGenerator::OccupyCover(UCoverPoint* CoverPoint, class AController* Controller)
{
	if (CoverPoint && CoverPoint->OccupiedBy == NULL)
	{
		CoverPoint->OccupiedBy = Controller;
		CoverPointsCurrentlyUsed.Add(CoverPoint);
		return true;
	}

	return false;
}

void ACoverGenerator::ReleaseCover(UCoverPoint* CoverPoint)
{
	if (CoverPoint && CoverPoint->OccupiedBy != NULL)
	{
		CoverPointsCurrentlyUsed.Remove(CoverPoint);
		CoverPoint->OccupiedBy = NULL;
	}
}

bool ACoverGenerator::IsFreeCoverPoint(const UCoverPoint* CoverPoint)
{
	return (CoverPoint && CoverPoint->OccupiedBy == NULL);
}

void ACoverGenerator::DrawOctreeBounds()
{
	if (!GetWorld())
		return; 

	FVector extent = CoverPointOctree->GetRootBounds().Extent;

	float max = extent.GetMax();
	FVector maxExtent = FVector(max, max, max);
	FVector center = CoverPointOctree->GetRootBounds().Center;

	DrawDebugBox(GetWorld(), center, maxExtent, FColor().Blue, false, 0.0f);
	DrawDebugSphere(GetWorld(), center + maxExtent, 4.0f, 12, FColor().White, false, 0.0f);
	DrawDebugSphere(GetWorld(), center - maxExtent, 4.0f, 12, FColor().White, false, 0.0f);
}

TArray<FCoverPointOctreeElement> ACoverGenerator::GetOctreeElementsWithinBounds(const FBoxCenterAndExtent& BoundsIn)
{
	// Iterating over a region in the Octree and storing the elements
	int count = 0;
	TArray<FCoverPointOctreeElement> octreeElements;

	for (FCoverPointOctree::TConstElementBoxIterator<> OctreeIt(*CoverPointOctree, BoundsIn);
	OctreeIt.HasPendingElements();
		OctreeIt.Advance())
	{
		octreeElements.Add(OctreeIt.GetCurrentElement());
		count++;
	}
	// UE_LOG(LogTemp, Log, TEXT("%d elements in %s"), count, *boundingBoxQuery.Extent.ToString());

	return octreeElements;
}

TArray<UCoverPoint*> ACoverGenerator::GetCoverWithinBounds(const FBoxCenterAndExtent& BoundsIn)
{
	// Iterating over a region in the octree and storing the elements
	TArray<UCoverPoint*> Covers;

	for (FCoverPointOctree::TConstElementBoxIterator<> OctreeIt(*CoverPointOctree, BoundsIn);
	OctreeIt.HasPendingElements();
		OctreeIt.Advance())
	{
		Covers.Add(OctreeIt.GetCurrentElement().GetOwner());
	}

	return Covers;
}

bool ACoverGenerator::CoverExistWithinBounds(const FBoxCenterAndExtent& BoundsIn)
{
	FCoverPointOctree::TConstElementBoxIterator<> OctreeIt(*CoverPointOctree, BoundsIn);
	return OctreeIt.HasPendingElements();
}


TArray<UCoverPoint*> ACoverGenerator::GetCoverWithinBox(const FBox& BoxIn)
{
	return GetCoverWithinBounds(FBoxCenterAndExtent(BoxIn));
}

bool ACoverGenerator::CoverExistWithinBox(const FBox& BoxIn)
{
	return CoverExistWithinBounds(FBoxCenterAndExtent(BoxIn));
}

#if WITH_EDITOR

void ACoverGenerator::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	FName PropertyName = (PropertyChangedEvent.Property != NULL) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	if (PropertyName == GET_MEMBER_NAME_CHECKED(ACoverGenerator, ForceRefresh))
	{
		ForceRefresh = false; 
		// in editor not async
		GenerateCovers(true, false); 
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}

#endif