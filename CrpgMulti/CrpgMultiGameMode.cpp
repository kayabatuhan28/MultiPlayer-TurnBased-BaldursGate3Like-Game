// Copyright Epic Games, Inc. All Rights Reserved.

#include "CrpgMultiGameMode.h"
#include "CrpgMultiPlayerController.h"
#include "CrpgMultiCharacter.h"
#include "UObject/ConstructorHelpers.h"

ACrpgMultiGameMode::ACrpgMultiGameMode()
{
	PlayerControllerClass = ACrpgMultiPlayerController::StaticClass();
	
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownCharacter"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
	
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownPlayerController"));
	if(PlayerControllerBPClass.Class != NULL)
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}
}


void ACrpgMultiGameMode::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);

	ACrpgMultiPlayerController* HoldController = Cast<ACrpgMultiPlayerController>(NewPlayer);
	if(HoldController)
	{
		ControllerRefs.Add(HoldController);
		PlayerCount++;
		if(PlayerCount == 2)
		{
			WhichPlayerTurn = 0;
			ControllerRefs[0]->bIsPlayerTurn = true;
			ControllerRefs[1]->bIsPlayerTurn = false;
			//GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Green,FString::Printf(TEXT("Server %hhd"),ControllerRefs[0]->bIsPlayerTurn));
			//GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Green,FString::Printf(TEXT("Client %hhd"),ControllerRefs[1]->bIsPlayerTurn));
		}
	}
}




