// Fill out your copyright notice in the Description page of Project Settings.


#include "ScenarioRunner.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

// Sets default values
AScenarioRunner::AScenarioRunner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AScenarioRunner::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AScenarioRunner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

static FVector GetObjectMoveToLocation(FString targetName) {
	// TODO
	return FVector(0, 0, 0);
}

static void SignalNpcMoveToLocation(FString npcName, FVector location) {
	// TODO
	/*
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), Npc::StaticClass(), FoundActors);
	for (auto* actor : FoundActors) {
		if (actor->name != npcName) {
			continue;
		}
		auto npcComponent = npc.getComponent<NpcComponent>();
		npcComponent->MoveToLocation(location);
	}
	*/
}

void AScenarioRunner::SetScenarioData(const FScenarioData& data) {
	// TODO: Implement this function

	// Fake test implementation for now
	const auto behavior = data.routines["radio_check_routine"].tasks[0].behavior;
	if (behavior == "move_to") {
		const auto target = data.routines["radio_check_routine"].tasks[0].target;

		FVector location = GetObjectMoveToLocation(target);
		SignalNpcMoveToLocation("Larry", location);
	}
}
