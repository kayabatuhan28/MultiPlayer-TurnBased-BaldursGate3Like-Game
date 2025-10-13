// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CrpgMultiPlayerController.h"
#include "GameFramework/GameModeBase.h"
#include "CrpgMultiGameMode.generated.h"

UCLASS(minimalapi)
class ACrpgMultiGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ACrpgMultiGameMode();

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	TArray<ACrpgMultiPlayerController*> ControllerRefs;

	virtual void OnPostLogin(AController* NewPlayer) override;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	int PlayerCount;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	int WhichPlayerTurn;
	
};



