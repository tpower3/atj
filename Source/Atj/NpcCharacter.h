// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NpcCharacter.generated.h"

class AObjectActor;
class AItemActor;

UCLASS()
class ATJ_API ANpcCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ANpcCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	FString GetName() const;

	UFUNCTION(BlueprintCallable, Category = "Npc")
	void SetName(const FString& name);

	void RoutineMoveTo(AObjectActor* objectActor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Npc")
		void PickUp(const AItemActor* item);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Npc")
		void PutDown(const AObjectActor* object);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Npc")
		void Init();

private:
	FString _name;

};
