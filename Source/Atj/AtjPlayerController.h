// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AtjPlayerController.generated.h"

UCLASS()
class AAtjPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AAtjPlayerController();

protected:

	// Begin PlayerController interface
	virtual void PlayerTick(float DeltaTime) override;
	// End PlayerController interface

};


