// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemActor.generated.h"

UCLASS()
class ATJ_API AItemActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemActor();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Item")
		FString GetName() const;

	UFUNCTION(BlueprintCallable, Category = "Item")
		void SetName(const FString& name);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	FString _name;

};
