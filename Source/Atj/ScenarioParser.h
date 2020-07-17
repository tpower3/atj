// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Templates/SharedPointer.h"

#include "ScenarioParser.generated.h"

USTRUCT(BlueprintType)
struct FCondition
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite)
		FString check;

	UPROPERTY(BlueprintReadWrite)
		FString npc;

	UPROPERTY(BlueprintReadWrite)
		FString location;
};

USTRUCT(BlueprintType)
struct FTrigger
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite)
		TArray<FCondition> conditions;

	UPROPERTY(BlueprintReadWrite)
		TArray<FString> actions;
};

UENUM()
enum SignalTypes {
	BindNpc UMETA(DisplayName = "BindNpc"),
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
		TArray<FString> npcs;

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
		FScenarioData GetScenarioData() const;

private:
	TSharedRef<FSignal> ParseSignalBindNpc(const FJsonObject& signalObject);
	TSharedRef<FSignal> ParseSignalObjectSetState(const FJsonObject& signalObject);

	FScenarioData _scenarioData;

};
