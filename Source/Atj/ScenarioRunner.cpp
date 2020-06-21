// Fill out your copyright notice in the Description page of Project Settings.


#include "ScenarioRunner.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

#include "EngineUtils.h"

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
	InitScene();
}

// Called every frame
void AScenarioRunner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AScenarioRunner::InitScene()
{
	for (TActorIterator<ANpcCharacter> It(GetWorld()); It; ++It)
	{
		It->Init();
	}
	for (TActorIterator<AObjectActor> It(GetWorld()); It; ++It)
	{
		It->Init();
	}

}

ANpcCharacter* AScenarioRunner::FindNpcCharacter(const FString& npcName) {
	for (TActorIterator<ANpcCharacter> It(GetWorld()); It; ++It)
	{
		const auto name = It->GetName();
		if (name == npcName) {
			return *It;
		}
	}
	return nullptr;
}

AObjectActor* AScenarioRunner::FindObjectActor(const FString& objectName) {
	for (TActorIterator<AObjectActor> It(GetWorld()); It; ++It)
	{
		const auto name = It->GetName();
		if (name == objectName) {
			return *It;
		}
	}
	return nullptr;
}

void AScenarioRunner::SetScenarioData(const FScenarioData& data) {
	// TODO: Implement this function

	// Fake test implementation for now
	const auto behavior = data.routines["radio_check_routine"].tasks[0].behavior;
	if (behavior == "move_to") {
		const auto target = data.routines["radio_check_routine"].tasks[0].target;

		ANpcCharacter* npcCharacter = FindNpcCharacter("Larry");
		if (!npcCharacter) {
			return;
		}

		AObjectActor* targetActor = FindObjectActor(target);
		npcCharacter->RoutineMoveTo(targetActor);
	}
}
