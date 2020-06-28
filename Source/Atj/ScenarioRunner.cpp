// Fill out your copyright notice in the Description page of Project Settings.


#include "ScenarioRunner.h"

#include "Misc/DefaultValueHelper.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Engine/World.h"

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

	// TODO: Verify that DeltaTime is GameTime not WorldTime
	ProcessNpcBindings(GetWorld(), _npcBindings, GetWorld()->GetTimeSeconds());
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

static ANpcCharacter* FindNpcCharacter(UWorld* world, const FString& npcName) {
	for (TActorIterator<ANpcCharacter> It(world); It; ++It)
	{
		const auto name = It->GetName();
		if (name == npcName) {
			return *It;
		}
	}
	return nullptr;
}

static AObjectActor* FindObjectActor(UWorld* world, const FString& objectName) {
	for (TActorIterator<AObjectActor> It(world); It; ++It)
	{
		const auto name = It->GetName();
		if (name == objectName) {
			return *It;
		}
	}
	return nullptr;
}

void AScenarioRunner::ProcessNpcBindings(UWorld* world, const TMap<FString, FNpcBindingData>& npcBindings, float currentTime) {
	for (const auto& npcBinding : npcBindings) {
		const auto& npcName = npcBinding.Key;
		const auto& npcBindingData = npcBinding.Value;
		const FRoutine routine = npcBindingData.routine;
		float startTime = npcBindingData.startTime;

		for (const auto& task : routine.tasks) {
			float syncTime;
			FDefaultValueHelper::ParseFloat(task.sync_time, syncTime);
			const bool isPastTaskSyncTime = (currentTime >= (startTime + syncTime));
			const bool isPreviousTickBeforeSyncTime = ((_previousTickGameTime < (startTime + syncTime)));

			if (isPreviousTickBeforeSyncTime && isPastTaskSyncTime) {
				// This task is ready to be executed
				// This method is not purly deterministic since the time between ticks can vary, and
				// the task is only executed on a frame.

				const auto behavior = task.behavior;
				// We only support "move_to" right now
				if (behavior == "move_to") {
					const auto targetName = task.target;

					ANpcCharacter* npcCharacter = FindNpcCharacter(world, npcName);
					if (!npcCharacter) {
						// TODO: Handle failed lookup
						continue;
					}

					AObjectActor* targetActor = FindObjectActor(world, targetName);
					if (!targetActor) {
						// TODO: Handle failed lookup
						continue;
					}
					npcCharacter->RoutineMoveTo(targetActor);
				}
			}
		}
	}
	_previousTickGameTime = currentTime;
}

void AScenarioRunner::ProcessAction(const FScenarioData& scenarioData, const FAction& action, float startTime) {
	for (const auto& signal : action.signals) {
		switch (signal->type) {
		case SignalTypes::BindNpc:
		{
			const TSharedRef<FSignal_BindNpc> signalBindNpc = StaticCastSharedRef<FSignal_BindNpc>(signal);
			FString routineName = signalBindNpc->routine;
			const FRoutine routine = scenarioData.routines[routineName];
			FString npcName = signalBindNpc->npc;

			// TODO: Data structure to hold all metadata
			FNpcBindingData npcBindingData;
			npcBindingData.routine = routine;
			npcBindingData.startTime = startTime;
			_npcBindings.Add(npcName, npcBindingData);
		}
			break;
		default:
			// TODO: Handle undefined type
			break;
		}
	}
}

void AScenarioRunner::SetScenarioData(const FScenarioData& data) {
	if (data.actions.Contains("simulation_start")) {
		ProcessAction(data, data.actions["simulation_start"], GetWorld()->GetTimeSeconds());
	}
}
