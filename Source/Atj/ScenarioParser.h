// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
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

};
