// Fill out your copyright notice in the Description page of Project Settings.


#include "npcComponent.h"
#include "AIController.h"

// Sets default values for this component's properties
UnpcComponent::UnpcComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UnpcComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UnpcComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UnpcComponent::MoveToLocation(const FVector& location) {
	// TODO
	//auto aiController = GetAiController();
	//aiController.MoveToLocation(location);
}

