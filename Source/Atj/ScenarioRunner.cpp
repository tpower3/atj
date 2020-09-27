// Fill out your copyright notice in the Description page of Project Settings.


#include "ScenarioRunner.h"

#include "Misc/DefaultValueHelper.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Engine/World.h"

#include "ItemActor.h"
#include "ObjectActor.h"

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
	ProcessNpcBindings(GetWorld(), _scenarioData, _npcBindings, GetWorld()->GetTimeSeconds());
	for (const auto& elem : _npcBindingsQueue) {
		_npcBindings.Add(elem);
	}
	_npcBindingsQueue.Empty();
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

static AItemActor* FindItemActor(UWorld* world, const FString& itemName) {
	for (TActorIterator<AItemActor> It(world); It; ++It)
	{
		const auto name = It->GetName();
		if (name == itemName) {
			return *It;
		}
	}
	return nullptr;
}

void AScenarioRunner::ProcessNpcBindings(UWorld* world, const FScenarioData& scenarioData, const TMap<FString, FNpcBindingData>& npcBindings, float currentTime) {
	for (const auto& npcBinding : npcBindings) {
		const auto& npcName = npcBinding.Key;
		const auto& npcBindingData = npcBinding.Value;
		const FRoutine& routine = scenarioData.routines[npcBindingData.routineName];
		float startTime = npcBindingData.startTime;

		// Get sync time of current task
		int currentTaskIdx = 0;
		float accumulatedSyncTime = 0.0;
		for (const auto& task : routine.tasks) {
			float syncTime;
			FDefaultValueHelper::ParseFloat(task->sync_time, syncTime);
			accumulatedSyncTime += syncTime;
			if ((_previousTickGameTime - startTime) <= accumulatedSyncTime) {
				break;
			}
			++currentTaskIdx;
		}

		if (currentTaskIdx >= routine.tasks.Num()) {
			continue;
		}
		const auto& task = routine.tasks[currentTaskIdx];

		const bool isPastTaskSyncTime = (currentTime > (startTime + accumulatedSyncTime));
		const bool isPreviousTickBeforeSyncTime = ((_previousTickGameTime <= (startTime + accumulatedSyncTime)));

		if (isPreviousTickBeforeSyncTime && isPastTaskSyncTime) {
			// This task is ready to be executed
			// This method is not purly deterministic since the time between ticks can vary, and
			// the task is only executed on a frame.

			switch (task->type) {
			case TaskTypes::Behavior:
			{
				const TSharedRef<FTask_Behavior> taskBehavior = StaticCastSharedRef<FTask_Behavior>(task);
				const auto behavior = taskBehavior->behavior;

				ANpcCharacter* npcCharacter = FindNpcCharacter(world, npcName);
				if (!npcCharacter) {
					// TODO: Handle failed lookup
					UE_LOG(LogTemp, Warning, TEXT("Failed to find: %s"), *(npcName));
					continue;
				}

				if (behavior == "move_to") {
					const auto targetName = taskBehavior->target;

					AObjectActor* targetActor = FindObjectActor(world, targetName);
					if (!targetActor) {
						// TODO: Handle failed lookup
						UE_LOG(LogTemp, Warning, TEXT("Failed to find for 'move_to' behavior: %s"), *(targetName));
						continue;
					}
					npcCharacter->RoutineMoveTo(targetActor);
				}
				else if (behavior == "pick_up") {
					const auto targetName = taskBehavior->target;
					AItemActor* targetActor = FindItemActor(world, targetName);
					if (!targetActor) {
						// TODO: Handle failed lookup
						UE_LOG(LogTemp, Warning, TEXT("Failed to find for 'pick_up' behavior: %s"), *(targetName));
						continue;
					}
					npcCharacter->PickUp(targetActor);
				}
				else if (behavior == "put_down") {
					const auto targetName = taskBehavior->target;
					AObjectActor* targetActor = FindObjectActor(world, targetName);
					if (!targetActor) {
						// TODO: Handle failed lookup
						UE_LOG(LogTemp, Warning, TEXT("Failed to find for 'put_down' behavior: %s"), *(targetName));
						continue;
					}
					npcCharacter->PutDown(targetActor);
				}
			}
			break;
			case TaskTypes::ExecuteAction:
			{
				const TSharedRef<FTask_ExecuteAction> taskExecuteAction = StaticCastSharedRef<FTask_ExecuteAction>(task);
				ProcessAction(scenarioData, taskExecuteAction->action, GetWorld()->GetTimeSeconds());
				break;
			}
			}
		}
	}
	_previousTickGameTime = currentTime;
}

void AScenarioRunner::ProcessAction(const FScenarioData& scenarioData, FString actionName, float startTime) {
	FAction action = scenarioData.actions[actionName];
	for (const auto& signal : action.signals) {
		switch (signal->type) {
		case SignalTypes::BindNpc:
		{
			const TSharedRef<FSignal_BindNpc> signalBindNpc = StaticCastSharedRef<FSignal_BindNpc>(signal);
			FString routineName = signalBindNpc->routine;
			FString npcName = signalBindNpc->npc;

			// TODO: Data structure to hold all metadata
			FNpcBindingData npcBindingData;
			npcBindingData.routineName = routineName;
			npcBindingData.startTime = startTime;

			_npcBindingsQueue.Add(npcName, npcBindingData);
		}
			break;
		case SignalTypes::IncrementMood:
		{
			const TSharedRef<FSignal_IncrementMood> signalIncrementMood = StaticCastSharedRef<FSignal_IncrementMood>(signal);
			int value;
			FDefaultValueHelper::ParseInt(signalIncrementMood->value, value);

			FString npcName = signalIncrementMood->npc;

			const auto iter = _npcMoodData.Find(npcName);
			if (nullptr == iter) {
				UE_LOG(LogTemp, Warning, TEXT("IncrementMood npc lookup error: %s"), *(npcName));
				// TODO: Handle error
				return;
			}

			// Increment the Npc Mood by the desired value, clamping the final result
			constexpr int MIN_MOOD_VALUE = 0;
			constexpr int MAX_MOOD_VALUE = 5;
			*iter = FMath::Clamp(*iter + value, MIN_MOOD_VALUE, MAX_MOOD_VALUE);
		}
		break;
		default:
			// TODO: Handle undefined type
			UE_LOG(LogTemp, Warning, TEXT("undefined"));
			break;
		}
	}
}

void AScenarioRunner::SetScenarioData(const FScenarioData& data) {
	_scenarioData = data;
	constexpr int DEFAULT_MOOD_VALUE = 3;
	for (const FString& npc : _scenarioData.npcs) {
		_npcMoodData.Add(npc, DEFAULT_MOOD_VALUE);
	}
	if (data.actions.Contains("simulation_start")) {
		ProcessAction(data, "simulation_start", GetWorld()->GetTimeSeconds());
	}
}
