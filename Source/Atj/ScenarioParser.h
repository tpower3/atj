// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Templates/SharedPointer.h"

#include "ScenarioParser.generated.h"

USTRUCT()
struct FCondition
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
		FString check;

	UPROPERTY()
		FString npc;

	UPROPERTY()
		FString location;
};

USTRUCT()
struct FTrigger
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
		TArray<FCondition> conditions;

	UPROPERTY()
		TArray<FString> actions;
};

UENUM()
enum SignalTypes {
	BindNpc UMETA(DisplayName = "BindNpc"),
	ObjectSetState UMETA(DisplayName = "ObjectSetState")
};

USTRUCT()
struct FSignal
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
		TEnumAsByte<SignalTypes> type;
};

USTRUCT()
struct FSignal_BindNpc : public FSignal
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
		FString npc;

	UPROPERTY()
		FString routine;
};

USTRUCT()
struct FSignal_ObjectSetState : public FSignal
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
		FString object;

	UPROPERTY()
		FString state;
};

USTRUCT()
struct FAction
{
	GENERATED_USTRUCT_BODY()


	TArray<TSharedRef<FSignal>> signals;
};

// TODO Inherit multiple task types from FTask
USTRUCT()
struct FTask
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
		FString type;

	UPROPERTY()
		FString sync_time;

	UPROPERTY()
		FString behavior;

	UPROPERTY()
		FString target;
};

USTRUCT()
struct FRoutine
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY()
		TArray<FTask> tasks;
};

USTRUCT()
struct FSimulatorData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
		TArray<FString> npcs;

	UPROPERTY()
		TArray<FString> objects;

	UPROPERTY()
		TMap<FString, FTrigger> triggers;

	UPROPERTY()
		TMap<FString, FAction> actions;

	UPROPERTY()
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

private:
	TSharedRef<FSignal> ParseSignalBindNpc(const FJsonObject& signalObject);
	TSharedRef<FSignal> ParseSignalObjectSetState(const FJsonObject& signalObject);

};
