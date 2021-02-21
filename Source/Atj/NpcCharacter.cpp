// Fill out your copyright notice in the Description page of Project Settings.


#include "NpcCharacter.h"
#include "NpcAiController.h"
#include "ObjectActor.h"

// Sets default values
ANpcCharacter::ANpcCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ANpcCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ANpcCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ANpcCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ANpcCharacter::Init_Implementation()
{

}

void ANpcCharacter::PickUp_Implementation(const AItemActor* item)
{

}

void ANpcCharacter::PutDown_Implementation(const AObjectActor* object)
{

}


FString ANpcCharacter::GetName() const {
	return _name;
}

void ANpcCharacter::SetName(const FString& name) {
	_name = name;
}

void ANpcCharacter::RoutineMoveTo(AObjectActor* objectActor) {
	if (nullptr == objectActor) {
		UE_LOG(LogTemp, Error, TEXT("Null objectActor"));
		return;
	}
	FVector location = objectActor->GetObjectInteractLocation();
	ANpcAiController* aiController = Cast<ANpcAiController>(GetController());
	if (nullptr == aiController) {
		UE_LOG(LogTemp, Error, TEXT("Failed to cast to NpcAiController"));
		return;
	}
	aiController->MoveToLocation(location);
}
