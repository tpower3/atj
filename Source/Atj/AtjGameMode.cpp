// Copyright Epic Games, Inc. All Rights Reserved.

#include "AtjGameMode.h"
#include "AtjPlayerController.h"
#include "AtjCharacter.h"
#include "UObject/ConstructorHelpers.h"

AAtjGameMode::AAtjGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = AAtjPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/NpcCharacters/NpcCharacterBp"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}