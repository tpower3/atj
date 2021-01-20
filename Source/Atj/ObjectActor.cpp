// Fill out your copyright notice in the Description page of Project Settings.


#include "ObjectActor.h"

// Sets default values
AObjectActor::AObjectActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AObjectActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AObjectActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AObjectActor::Init_Implementation()
{

}

FString AObjectActor::GetName() const {
	return _name;
}

void AObjectActor::SetName(const FString& name) {
	_name = name;
}

FVector AObjectActor::GetObjectInteractLocation() const {
	return _interactLocation;
}

void AObjectActor::SetObjectInteractLocation(const FVector& location) {
	_interactLocation = location;
}

void AObjectActor::AttachItemActor_Implementation(const AItemActor* item) {

}

void AObjectActor::GetAttachedItemName_Implementation(FString& result) {

}