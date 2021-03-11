// Fill out your copyright notice in the Description page of Project Settings.


#include "ScenarioRunner.h"

#include "Math/Vector.h"
#include "Misc/DefaultValueHelper.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Engine/World.h"

#include "AtjGameMode.h"
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

	if (!_scenarioData.IsSet()) {
		return;
	}

	// TODO: Verify that DeltaTime is GameTime not WorldTime
	ProcessTriggers(GetWorld(), _scenarioData.GetValue());
	ProcessNpcBindings(GetWorld(), _scenarioData.GetValue(), _npcBindings, GetWorld()->GetTimeSeconds());
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

static bool ProcessNpcMoodCheck(const TMap<FString, int>& npcMoodData, const TSharedRef<FCondition_NpcMoodCheck> conditionNpcMoodCheck) {
	const auto iter = npcMoodData.Find(conditionNpcMoodCheck->npc);
	if (nullptr == iter) {
		// TODO: Handle error state
		return false;
	}
	const int npcMoodValue = *iter;
	int checkValue;
	FDefaultValueHelper::ParseInt(conditionNpcMoodCheck->value, checkValue);
	if (conditionNpcMoodCheck->check == "LTorEQ") {
		return npcMoodValue <= checkValue;
	}
	if (conditionNpcMoodCheck->check == "LT") {
		return npcMoodValue < checkValue;
	}
	if (conditionNpcMoodCheck->check == "GTorEQ") {
		return npcMoodValue >= checkValue;
	}
	if (conditionNpcMoodCheck->check == "GT") {
		return npcMoodValue > checkValue;
	}
	if (conditionNpcMoodCheck->check == "EQ") {
		return npcMoodValue == checkValue;
	}

	// TODO: Handle unhandled check
	return false;
}

static bool ProcessNpcPositionCheck(UWorld* world, const TSharedRef<FCondition_NpcPositionCheck> conditionNpcPositionCheck) {
	ANpcCharacter* npcCharacter = FindNpcCharacter(world, conditionNpcPositionCheck->npc);
	if (!npcCharacter) {
		UE_LOG(LogTemp, Error, TEXT("Npc does not exist: %s"), *(conditionNpcPositionCheck->npc));
		return false;
	}

	AObjectActor* targetActor = FindObjectActor(world, conditionNpcPositionCheck->object);
	if (!targetActor) {
		UE_LOG(LogTemp, Error, TEXT("ObjectActor does not exist: %s"), *(conditionNpcPositionCheck->object));
		return false;
	}

	float checkDistance;
	FDefaultValueHelper::ParseFloat(conditionNpcPositionCheck->distance, checkDistance);


	const auto distance = FVector::Dist(npcCharacter->GetActorLocation(), targetActor->GetActorLocation());
	return distance <= checkDistance;
}

static bool ProcessItemInObjectSlotCheck(UWorld* world, const TSharedRef<FCondition_ItemInObjectSlotCheck> conditionItemInObjectSlotCheck) {
	AItemActor* itemActor = FindItemActor(world, conditionItemInObjectSlotCheck->item);
	if (!itemActor) {
		UE_LOG(LogTemp, Error, TEXT("Item does not exist: %s"), *(conditionItemInObjectSlotCheck->item));
		return false;
	}

	AObjectActor* objectActor = FindObjectActor(world, conditionItemInObjectSlotCheck->object);
	if (!objectActor) {
		UE_LOG(LogTemp, Error, TEXT("ObjectActor does not exist: %s"), *(conditionItemInObjectSlotCheck->object));
		return false;
	}

	FString result;
	objectActor->GetAttachedItemName(result);
	return conditionItemInObjectSlotCheck->item == result;
}

void AScenarioRunner::ProcessTriggers(UWorld* world, const FScenarioData& scenarioData) {
	for (const auto& trigger : scenarioData.triggers) {
		const bool previousTriggerState = _triggerState[trigger.Key];
		bool triggerFired = true;
		for (const auto& condition : trigger.Value.allOf) {
			switch (condition->type) {
			case ConditionTypes::NpcMoodCheck:
			{
				const TSharedRef<FCondition_NpcMoodCheck> conditionNpcMoodCheck = StaticCastSharedRef<FCondition_NpcMoodCheck>(condition);
				const bool result = ProcessNpcMoodCheck(_npcMoodData, conditionNpcMoodCheck);
				if (!result) {
					triggerFired = false;
				}
				break;
			}
			case ConditionTypes::NpcPositionCheck:
			{
				const TSharedRef<FCondition_NpcPositionCheck> conditionNpcPositionCheck = StaticCastSharedRef<FCondition_NpcPositionCheck>(condition);
				const bool result = ProcessNpcPositionCheck(world, conditionNpcPositionCheck);
				if (!result) {
					triggerFired = false;
				}
				break;
			}
			case ConditionTypes::ItemInObjectSlotCheck:
			{
				const TSharedRef<FCondition_ItemInObjectSlotCheck> conditionItemInObjectSlotCheck = StaticCastSharedRef<FCondition_ItemInObjectSlotCheck>(condition);
				const bool result = ProcessItemInObjectSlotCheck(world, conditionItemInObjectSlotCheck);
				if (!result) {
					triggerFired = false;
				}
				break;
			}
			}

			if (!triggerFired) {
				break;
			}
		}
		if (!previousTriggerState && triggerFired) {
			UE_LOG(LogTemp, Warning, TEXT("Trigger fired"));
			for (const auto& action : trigger.Value.actions) {
				UE_LOG(LogTemp, Warning, TEXT("ProcessAction: %s"), *(action));
				ProcessAction(world, scenarioData, action, world->GetTimeSeconds());
			}
		}
		_triggerState[trigger.Key] = triggerFired;
	}
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
			if (syncTime == 0.0) {
				UE_LOG(LogTemp, Error, TEXT("Npc `%s` with routine `%s` has task with SyncTime of 0.0. This is unsupported."), *npcName, *npcBindingData.routineName);
			}
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
					UE_LOG(LogTemp, Error, TEXT("Npc does not exist: %s"), *(npcName));
					continue;
				}

				if (behavior == "move_to") {
					const auto targetName = taskBehavior->target;

					AObjectActor* targetActor = FindObjectActor(world, targetName);
					if (!targetActor) {
						UE_LOG(LogTemp, Error, TEXT("Failed '%s' 'move_to'. %s does not exist."), *(npcName), *(targetName));
						continue;
					}
					npcCharacter->RoutineMoveTo(targetActor);
				}
				else if (behavior == "pick_up") {
					const auto targetName = taskBehavior->target;
					AItemActor* targetActor = FindItemActor(world, targetName);
					if (!targetActor) {
						UE_LOG(LogTemp, Error, TEXT("Failed '%s' 'pick_up'. %s does not exist."), *(npcName), *(targetName));
						continue;
					}
					npcCharacter->PickUp(targetActor);
				}
				else if (behavior == "put_down") {
					const auto targetName = taskBehavior->target;
					AObjectActor* targetActor = FindObjectActor(world, targetName);
					if (!targetActor) {
						UE_LOG(LogTemp, Error, TEXT("Failed '%s' 'put_down'. %s does not exist."), *(npcName), *(targetName));
						continue;
					}
					npcCharacter->PutDown(targetActor);
				}
			}
			break;
			case TaskTypes::ExecuteAction:
			{
				const TSharedRef<FTask_ExecuteAction> taskExecuteAction = StaticCastSharedRef<FTask_ExecuteAction>(task);
				ProcessAction(world, scenarioData, taskExecuteAction->action, world->GetTimeSeconds());
				break;
			}
			}
		}
	}
	_previousTickGameTime = currentTime;
}

void AScenarioRunner::ProcessAction(UWorld* world, const FScenarioData& scenarioData, FString actionName, float startTime) {
	FAction action = scenarioData.actions[actionName];
	for (const auto& signal : action.signals) {
		switch (signal->type) {
		case SignalTypes::BindNpc:
		{
			const TSharedRef<FSignal_BindNpc> signalBindNpc = StaticCastSharedRef<FSignal_BindNpc>(signal);
			FString routineName = signalBindNpc->routine;
			FString npcName = signalBindNpc->npc;

			/*if (!scenarioData.isDebugMode && npcName == "player") {
				// Skip processing player if debug mode is not enabled
				continue;
			}*/

			// TODO: Data structure to hold all metadata
			FNpcBindingData npcBindingData;
			npcBindingData.routineName = routineName;
			npcBindingData.startTime = startTime;

			_npcBindingsQueue.Add(npcName, npcBindingData);
		}
		break;
		case SignalTypes::EndGameFailure:
		{
			AAtjGameMode* gameMode = StaticCast<AAtjGameMode*>(world->GetAuthGameMode());
			gameMode->OnEndGameFailure();
		}
		break;
		case SignalTypes::EndGameSuccess:
		{
			AAtjGameMode* gameMode = StaticCast<AAtjGameMode*>(world->GetAuthGameMode());
			gameMode->OnEndGameSuccess();
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
				UE_LOG(LogTemp, Error, TEXT("Npc does not exist: %s"), *(npcName));
				return;
			}

			// Increment the Npc Mood by the desired value, clamping the final result
			constexpr int MIN_MOOD_VALUE = 1;
			constexpr int MAX_MOOD_VALUE = 4;
			*iter = FMath::Clamp(*iter + value, MIN_MOOD_VALUE, MAX_MOOD_VALUE);
		}
		break;
		default:
			UE_LOG(LogTemp, Error, TEXT("Undefined SignalType"));
			break;
		}
	}
}

void AScenarioRunner::SetScenarioData(const FScenarioData& data) {
	_scenarioData = data;
	constexpr int DEFAULT_MOOD_VALUE = 3;
	for (const FString& npc : _scenarioData.GetValue().npcs) {
		_npcMoodData.Add(npc, DEFAULT_MOOD_VALUE);
	}
	for (const auto& trigger : _scenarioData.GetValue().triggers) {
		_triggerState.Add(trigger.Key, false);
	}
	if (data.actions.Contains("simulation_start")) {
		ProcessAction(GetWorld(), data, "simulation_start", GetWorld()->GetTimeSeconds());
	}
}


FDebugInfo AScenarioRunner::getDebugInfo() const {
	FDebugInfo debugInfo;
	for (const auto& npcBindingData : _npcBindings) {
		FDebugNpcData debugNpcData;
		debugNpcData.npcName = npcBindingData.Key;
		debugNpcData.routineName = npcBindingData.Value.routineName;
		debugNpcData.routineStartTime = npcBindingData.Value.startTime;
		debugNpcData.currentTime = GetWorld()->GetTimeSeconds();

		const bool hasMoodData = _npcMoodData.Contains(npcBindingData.Key);
		debugNpcData.mood = hasMoodData ? _npcMoodData[npcBindingData.Key] : -1;

		debugInfo.npcData.Add(debugNpcData);
	}

	debugInfo.triggerData = _triggerState;

	return debugInfo;
}

bool AScenarioRunner::isDebugEnabled() const {
	if (_scenarioData.IsSet()) {
		return _scenarioData.GetValue().isDebugMode;
	}
	return false;
}