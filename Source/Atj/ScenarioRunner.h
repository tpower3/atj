// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ScenarioParser.h"
#include "ObjectActor.h"
#include "NpcCharacter.h"
#include "ScenarioRunner.generated.h"

USTRUCT(BlueprintType)
struct FNpcBindingData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite)
	FRoutine routine;

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

	void ProcessAction(const FScenarioData& scenarioData, const FAction& action, float startTime);
	void ProcessNpcBindings(UWorld* world, TMap<FString, FNpcBindingData>& npcBindings, float currentTime);

	// Used to map NPCs to a routine
	TMap<FString, FNpcBindingData> _npcBindings;

	float _previousTickGameTime = 0.0;
};
