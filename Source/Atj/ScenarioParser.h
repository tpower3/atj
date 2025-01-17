// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Misc/Optional.h"

#include "Templates/SharedPointer.h"

#include "ScenarioParser.generated.h"

UENUM()
enum ConditionTypes {
	NpcMoodCheck UMETA(DisplayName = "NpcMoodCheck"),
	NpcPositionCheck UMETA(DisplayName = "NpcPositionCheck"),
	ItemInObjectSlotCheck UMETA(DisplayName = "ItemInObjectSlotCheck")
};

USTRUCT(BlueprintType)
struct FCondition
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite)
		TEnumAsByte<ConditionTypes> type;
};

USTRUCT(BlueprintType)
struct FCondition_NpcMoodCheck : public FCondition
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite)
		FString npc;

	UPROPERTY(BlueprintReadWrite)
		FString check;

	UPROPERTY(BlueprintReadWrite)
		FString value;
};

USTRUCT(BlueprintType)
struct FCondition_NpcPositionCheck : public FCondition
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite)
		FString npc;

	UPROPERTY(BlueprintReadWrite)
		FString object;

	UPROPERTY(BlueprintReadWrite)
		FString distance;
};

USTRUCT(BlueprintType)
struct FCondition_ItemInObjectSlotCheck : public FCondition
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite)
		FString item;

	UPROPERTY(BlueprintReadWrite)
		FString object;
};

USTRUCT(BlueprintType)
struct FItemData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite)
		FString name;

	UPROPERTY(BlueprintReadWrite)
		FString initialObject;
};

USTRUCT(BlueprintType)
struct FTrigger
{
	GENERATED_USTRUCT_BODY()

	// Trigger will fire actions when allOf the events are met
	TArray<TSharedRef<FCondition>> allOf;

	UPROPERTY(BlueprintReadWrite)
		TArray<FString> actions;

	UPROPERTY(BlueprintReadWrite)
		bool alwaysEvaluate;
};

UENUM()
enum SignalTypes {
	BindNpc UMETA(DisplayName = "BindNpc"),
	EndGameFailure UMETA(DisplayName = "EndGameFailure"),
	EndGameSuccess UMETA(DisplayName = "EndGameSuccess"),
	IncrementMood UMETA(DisplayName = "IncrementMood"),
	ObjectSetState UMETA(DisplayName = "ObjectSetState")
};

USTRUCT(BlueprintType)
struct FSignal
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite)
		TEnumAsByte<SignalTypes> type;
};

USTRUCT(BlueprintType)
struct FSignal_BindNpc : public FSignal
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite)
		FString npc;

	UPROPERTY(BlueprintReadWrite)
		FString routine;
};


USTRUCT(BlueprintType)
struct FSignal_EndGameFailure : public FSignal
{
	GENERATED_USTRUCT_BODY()
};

USTRUCT(BlueprintType)
struct FSignal_EndGameSuccess : public FSignal
{
	GENERATED_USTRUCT_BODY()
};

USTRUCT(BlueprintType)
struct FSignal_IncrementMood : public FSignal
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY(BlueprintReadWrite)
		FString npc;

	UPROPERTY(BlueprintReadWrite)
		FString value;
};

USTRUCT(BlueprintType)
struct FSignal_ObjectSetState : public FSignal
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite)
		FString object;

	UPROPERTY(BlueprintReadWrite)
		FString state;
};

USTRUCT(BlueprintType)
struct FAction
{
	GENERATED_USTRUCT_BODY()


	TArray<TSharedRef<FSignal>> signals;
};

UENUM()
enum TaskTypes {
	Behavior UMETA(DisplayName = "Behavior"),
	EvaluateTrigger UMETA(DisplayName = "EvaluateTrigger"),
	ExecuteAction UMETA(DisplayName = "ExecuteAction")
};

USTRUCT(BlueprintType)
struct FTask
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite)
		TEnumAsByte<TaskTypes> type;

	UPROPERTY(BlueprintReadWrite)
		FString sync_time;
};

USTRUCT(BlueprintType)
struct FTask_Behavior : public FTask
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite)
		FString behavior;

	UPROPERTY(BlueprintReadWrite)
		FString target;
};

USTRUCT(BlueprintType)
struct FTask_EvaluateTrigger : public FTask
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite)
		FString trigger;

	UPROPERTY(BlueprintReadWrite)
		FString pass_action;

	UPROPERTY(BlueprintReadWrite)
		FString fail_action;
};

USTRUCT(BlueprintType)
struct FTask_ExecuteAction : public FTask
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite)
		FString action;
};

USTRUCT(BlueprintType)
struct FRoutine
{
	GENERATED_USTRUCT_BODY()

	TArray<TSharedRef<FTask>> tasks;
};

USTRUCT(BlueprintType)
struct FScenarioData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite)
		bool isDebugMode = false;

	UPROPERTY(BlueprintReadWrite)
		TArray<FString> npcs;

	UPROPERTY(BlueprintReadWrite)
		TMap<FString, FItemData> items;

	UPROPERTY(BlueprintReadWrite)
		TArray<FString> objects;

	UPROPERTY(BlueprintReadWrite)
		TMap<FString, FTrigger> triggers;

	UPROPERTY(BlueprintReadWrite)
		TMap<FString, FAction> actions;

	UPROPERTY(BlueprintReadWrite)
		TMap<FString, FRoutine> routines;
};


UCLASS()
class ATJ_API AScenarioParser : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AScenarioParser();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Simulator")
		FScenarioData GetScenarioData();

private:
	void ParseScenario();

	TSharedRef<FCondition> ParseConditionNpcMoodCheck(const FJsonObject& conditionObject) const;
	TSharedRef<FCondition> ParseConditionNpcPositionCheck(const FJsonObject& conditionObject) const;
	TSharedRef<FCondition> ParseConditionItemInObjectSlotCheck(const FJsonObject& conditionObject) const;
	TSharedRef<FSignal> ParseSignalBindNpc(const FJsonObject& signalObject);
	TSharedRef<FSignal> ParseSignalEndGameFailure(const FJsonObject& signalObject);
	TSharedRef<FSignal> ParseSignalEndGameSuccess(const FJsonObject& signalObject);
	TSharedRef<FSignal> ParseSignalIncrementMood(const FJsonObject& signalObject);
	TSharedRef<FSignal> ParseSignalObjectSetState(const FJsonObject& signalObject);

	TOptional<FScenarioData> _scenarioData;

};
