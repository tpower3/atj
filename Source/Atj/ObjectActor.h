// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ObjectActor.generated.h"

class AItemActor;

UCLASS()
class ATJ_API AObjectActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AObjectActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FString GetName() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Object")
		void Init();

	UFUNCTION(BlueprintCallable, Category = "Object")
		void SetName(const FString& name);

	FVector GetObjectInteractLocation() const;

	UFUNCTION(BlueprintCallable, Category = "Object")
		void SetObjectInteractLocation(const FVector& location);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Object")
		void AttachItemActor(const AItemActor* item);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Object")
		void GetAttachedItemName(FString& result);

private:
	FString _name;
	FVector _interactLocation;
};
