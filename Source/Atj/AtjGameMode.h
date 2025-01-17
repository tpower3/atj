// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AtjGameMode.generated.h"

UCLASS(minimalapi)
class AAtjGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AAtjGameMode();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Simulator")
		void OnEndGameFailure();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Simulator")
		void OnEndGameSuccess();
};



