// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ScenarioParser.h"
#include "NpcCharacter.h"
#include "ScenarioRunner.generated.h"

USTRUCT(BlueprintType)
struct FNpcBindingData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite)
	FString routineName;

	float startTime;
};

UCLASS()
class ATJ_API AScenarioRunner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AScenarioRunner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Simulator")
	void SetScenarioData(const FScenarioData& data);

private:
	void InitScene();

	void ProcessAction(const FScenarioData& scenarioData, FString actionName, float startTime);
	void ProcessNpcBindings(UWorld* world, const FScenarioData& data, const TMap<FString, FNpcBindingData>& npcBindings, float currentTime);

	FScenarioData _scenarioData;

	// Used to map NPCs to a routine
	TMap<FString, FNpcBindingData> _npcBindings;
	TMap<FString, FNpcBindingData> _npcBindingsQueue;

	float _previousTickGameTime = 0.0;

	TMap<FString, int> _npcMoodData;
};
