// Copyright Epic Games, Inc. All Rights Reserved.

#include "CrpgMultiPlayerController.h"
#include "GameFramework/Pawn.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "CrpgMultiCharacter.h"
#include "CrpgMultiGameMode.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Widgets/PlayerOnHeadWidget.h"
#include "Widgets/PlayerWidget.h"
#include "Widgets/WBP_SkillSlot.h"

ACrpgMultiPlayerController::ACrpgMultiPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
}

void ACrpgMultiPlayerController::BeginPlay()
{
	Super::BeginPlay();
	PlayerRef = Cast<ACrpgMultiCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(),0));
	if(!PlayerRef)
	{
		GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Red,TEXT("PlayerRef Is Not Valid!"));
	}
	MovementPoint = 100.f;
	FTimerHandle MovementHandler;
	GetWorld()->GetTimerManager().SetTimer(MovementHandler,this,&ACrpgMultiPlayerController::ConsumeMovement,0.1f,true);

	CreatePlayerWidget();
	
	
	
}

void ACrpgMultiPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	if(PlayerRef)
	{
		if(PlayerRef->bIsDead == true)
		{
			if(bIsPlayerTurn == true )
			{
				PlayerWidget->TurnTxt->SetText(FText::FromString("Your Turn But You Are Die! Release Turn!"));
			}
			else
			{
				PlayerWidget->TurnTxt->SetText(FText::FromString("Waiting (Possesed Die Character)"));
			}
			
			return;
		}
	}
	

	if(bIsActionDone == true)
	{
		if(PlayerWidget)
		{
			PlayerWidget->SetActionImage(true);
		}
	}
	else
	{
		if(PlayerWidget)
		{
			PlayerWidget->SetActionImage(false);
		}
	}

	if(PlayerRef)
	{
		if(PlayerRef->ActiveState != Empty)
		{
			return;
		}
	}

	

	if(bIsPlayerTurn == false || MovementPoint <= 0)
	{
		bInputPressed = false;
		bIsTouch = false;
		if(PlayerWidget && MovementPoint > 0.f)
		{
			PlayerWidget->TurnTxt->SetText(FText::FromString("Waiting"));
		}
		return;
	}
    PlayerWidget->TurnTxt->SetText(FText::FromString("Your  Turn!"));

	
	if(bInputPressed)
	{
		FollowTime += DeltaTime;

		
		FVector HitLocation = FVector::ZeroVector;
		FHitResult Hit;
		if(bIsTouch)
		{
			GetHitResultUnderFinger(ETouchIndex::Touch1, ECC_Visibility, true, Hit);
		}
		else
		{
			GetHitResultUnderCursor(ECC_Visibility, true, Hit);
		}
		HitLocation = Hit.Location;

		
		APawn* const MyPawn = GetPawn();
		if(MyPawn)
		{
			FVector WorldDirection = (HitLocation - MyPawn->GetActorLocation()).GetSafeNormal();
			MyPawn->AddMovementInput(WorldDirection, 1.f, false);
		}
	}
	else
	{
		FollowTime = 0.f;
	}
}

void ACrpgMultiPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACrpgMultiPlayerController, bIsPlayerTurn)
	DOREPLIFETIME(ACrpgMultiPlayerController, MovementPoint)
	DOREPLIFETIME(ACrpgMultiPlayerController, PlayerWidget)
	DOREPLIFETIME(ACrpgMultiPlayerController, bIsActionDone)
	
	
}

void ACrpgMultiPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("SetDestination", IE_Pressed, this, &ACrpgMultiPlayerController::OnSetDestinationPressed);
	InputComponent->BindAction("SetDestination", IE_Released, this, &ACrpgMultiPlayerController::OnSetDestinationReleased);

	InputComponent->BindAction("ReleaseTurn", IE_Pressed, this, &ACrpgMultiPlayerController::ReleaseTurn);

	InputComponent->BindAction("Skill1", IE_Pressed, this, &ACrpgMultiPlayerController::Pressed1);
	InputComponent->BindAction("Skill2", IE_Pressed, this, &ACrpgMultiPlayerController::Pressed2);
	InputComponent->BindAction("Skill3", IE_Pressed, this, &ACrpgMultiPlayerController::Pressed3);
	InputComponent->BindAction("Skill4", IE_Pressed, this, &ACrpgMultiPlayerController::Pressed4);
	InputComponent->BindAction("Skill5", IE_Pressed, this, &ACrpgMultiPlayerController::Pressed5);
	
	InputComponent->BindAction<FCastSkillDelegate>("Skill1",IE_Released,this, &ACrpgMultiPlayerController::CastSkill,0);
	InputComponent->BindAction<FCastSkillDelegate>("Skill2",IE_Released,this, &ACrpgMultiPlayerController::CastSkill,1);
	InputComponent->BindAction<FCastSkillDelegate>("Skill3",IE_Released,this, &ACrpgMultiPlayerController::CastSkill,2);
	InputComponent->BindAction<FCastSkillDelegate>("Skill4",IE_Released,this, &ACrpgMultiPlayerController::CastSkill,3);
	InputComponent->BindAction<FCastSkillDelegate>("Skill5",IE_Released,this, &ACrpgMultiPlayerController::CastSkill,4);

	InputComponent->BindAction("BasicAttack", IE_Pressed, this, &ACrpgMultiPlayerController::PressedBasicAttack);
	InputComponent->BindAction("BasicAttack", IE_Released, this, &ACrpgMultiPlayerController::BasicAttack);
	
	InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &ACrpgMultiPlayerController::OnTouchPressed);
	InputComponent->BindTouch(EInputEvent::IE_Released, this, &ACrpgMultiPlayerController::OnTouchReleased);

}

void ACrpgMultiPlayerController::CreatePlayerWidget_Implementation()
{
	if(WidgetClass)
	{
		PlayerWidget = Cast<UPlayerWidget>(CreateWidget(GetWorld(),WidgetClass));
		if(PlayerWidget != nullptr)
		{
			PlayerWidget->AddToViewport();
			Update_MovementWidgetBar();
			PlayerWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void ACrpgMultiPlayerController::ReleaseTurn()
{
	if(PlayerRef->GetCharacterMovement()->Velocity.Size() > 0)
		return;
	
	if(bIsPlayerTurn == false)
	{
		bInputPressed = false;
		bIsTouch = false;
		return;
	}
	
	WhenReleaseTurn();
	
	if(HasAuthority())
	{
		//ReleaseTurn_Server();
		ServerReleaseTurn();
	}
	else
	{
		//GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Red,TEXT("Authority Degil!"));
		ClientReleaseTurn();
	}
	MovementPoint = 100.f;
	Update_MovementWidgetBar();
	bIsActionDone = false;
	
}

void ACrpgMultiPlayerController::ClientReleaseTurn_Implementation()
{
	ACrpgMultiGameMode* GameModeRef = Cast<ACrpgMultiGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if(!GameModeRef)
	{
		GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Red,TEXT("GameModeRef is not valid!"));
		return;
	}
	
	if(GameModeRef->WhichPlayerTurn == 0)
	{
		GameModeRef->ControllerRefs[0]->bIsPlayerTurn = false;
		GameModeRef->ControllerRefs[1]->bIsPlayerTurn = true;
		GameModeRef->WhichPlayerTurn = 1;
	}
	else
	{
		GameModeRef->ControllerRefs[0]->bIsPlayerTurn = true;
		GameModeRef->ControllerRefs[1]->bIsPlayerTurn = false;
		GameModeRef->WhichPlayerTurn = 0;
	}
	//GameModeRef->ControllerRefs[0]->MovementPoint = 100.f;
	//GameModeRef->ControllerRefs[1]->MovementPoint = 100.f;
	
}

void ACrpgMultiPlayerController::ServerReleaseTurn()
{
	ACrpgMultiGameMode* GameModeRef = Cast<ACrpgMultiGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if(!GameModeRef)
	{
		GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Red,TEXT("GameModeRef is not valid!"));
		return;
	}
	
	if(GameModeRef->WhichPlayerTurn == 0)
	{
		GameModeRef->ControllerRefs[0]->bIsPlayerTurn = false;
		GameModeRef->ControllerRefs[1]->bIsPlayerTurn = true;
		GameModeRef->WhichPlayerTurn = 1;
	}
	else
	{
		GameModeRef->ControllerRefs[0]->bIsPlayerTurn = true;
		GameModeRef->ControllerRefs[1]->bIsPlayerTurn = false;
		GameModeRef->WhichPlayerTurn = 0;
	}
	//GameModeRef->ControllerRefs[0]->MovementPoint = 100.f;
	//GameModeRef->ControllerRefs[1]->MovementPoint = 100.f;
	
	
}

void ACrpgMultiPlayerController::ConsumeMovement()
{
	if(HasAuthority())
	{
		Server_ConsumeMovement();
	}
	else
	{
		Client_ConsumeMovement();
	}
	
}

void ACrpgMultiPlayerController::Client_ConsumeMovement_Implementation()
{
	if(bIsPlayerTurn == false)
		return;

	if(MovementPoint - 3.f <= 0.f)
	{
		MovementPoint = 0.f;
		StopMovement();
		if(PlayerWidget)
		{
			Update_MovementWidgetBar();
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Red,TEXT("Player Widget Ref Yok!"));
		}
		
		return;
	}

	if(PlayerRef)
	{
		if(PlayerRef->GetCharacterMovement()->Velocity.Length() > 0.f && PlayerRef->ActiveState == EActiveState::Empty)
		{
			MovementPoint -= 3.f;
			if(PlayerWidget)
			{
				Update_MovementWidgetBar();
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Red,TEXT("Player Widget Ref Yok!"));
			}
			
			//GEngine->AddOnScreenDebugMessage(-1,0.1f,FColor::Green,FString::Printf(TEXT("Client Movement Point : %f"),MovementPoint));
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1,1.f,FColor::Red,TEXT("PlayerRef Is Not Valid!"));
	}
}

void ACrpgMultiPlayerController::Server_ConsumeMovement()
{
	if(bIsPlayerTurn == false)
		return;

	if(MovementPoint - 3.f <= 0.f)
	{
		MovementPoint = 0.f;
		StopMovement();
		//GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Green,TEXT("Server Movement Finish!"));
		if(PlayerWidget)
		{
			Update_MovementWidgetBar();
		}
		return;
	}

	if(PlayerRef)
	{
		if(PlayerRef->GetCharacterMovement()->Velocity.Length() > 0.f && PlayerRef->ActiveState == EActiveState::Empty)
		{
			MovementPoint -= 3.f;
			if(PlayerWidget)
			{
				Update_MovementWidgetBar();
			}
			//GEngine->AddOnScreenDebugMessage(-1,0.1f,FColor::Green,FString::Printf(TEXT("Server Movement Point : %f"),MovementPoint));
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1,1.f,FColor::Red,TEXT("PlayerRef Is Not Valid!"));
	}
}


void ACrpgMultiPlayerController::Update_MovementWidgetBar()
{
	if(PlayerWidget)
	{
		PlayerWidget->MovementBar->SetPercent(MovementPoint / 100.f);
	}
}

void ACrpgMultiPlayerController::CastSkill(int SkillIndex)
{
	if(PlayerRef->bIsTargetReleased == true)
	{
		bIsActionDone = false;
		return;
	}
	
	if(PlayerRef->Class == EClassType::Assassin && SkillIndex == 4 && PlayerRef->SkillMaximumUse[4] > 0)
	{
		UpdateMaxUse(SkillIndex);
		PlayerRef->AssassinSkill5();
		Update_MovementWidgetBar();
		return;
	}

	if(PlayerRef)
	{
		if(PlayerRef->bIsDead == true)
		{
			return;
		}
	}
	
	if(PlayerRef->bIsSilence == true || PlayerRef->GetCharacterMovement()->Velocity.Size() > 0 || PlayerRef->bIsKnockDown == true)
		return;
	
	if(bIsPlayerTurn == false || bIsActionDone == true || PlayerRef->SkillMaximumUse[SkillIndex] <= 0)
		return;

	UpdateMaxUse(SkillIndex);
	bIsActionDone = true;
	PlayerRef->UseSkill(SkillIndex);
}

void ACrpgMultiPlayerController::UpdateMaxUse(int SkillIndex)
{
	int NewMaxUse;
	switch (SkillIndex)
	{
	case 0 :
		NewMaxUse =  PlayerRef->SkillMaximumUse[0] -= 1;
		if(NewMaxUse <= 0)
		{
			FLinearColor UnUsableColor;
			UnUsableColor.R = 0.104167f;
			UnUsableColor.G = 0.104167f;
			UnUsableColor.B = 0.104167f;
			UnUsableColor.A = 1.f;
			PlayerWidget->Skill1->SkillIcon->SetColorAndOpacity(UnUsableColor);
		}
		PlayerWidget->Skill1->UpdateMaxUseText(NewMaxUse);
		break;
	case 1 :
		NewMaxUse =  PlayerRef->SkillMaximumUse[1] -= 1;
		if(NewMaxUse <= 0)
		{
			FLinearColor UnUsableColor;
			UnUsableColor.R = 0.104167f;
			UnUsableColor.G = 0.104167f;
			UnUsableColor.B = 0.104167f;
			UnUsableColor.A = 1.f;
			PlayerWidget->Skill2->SkillIcon->SetColorAndOpacity(UnUsableColor);
		}
		PlayerWidget->Skill2->UpdateMaxUseText(NewMaxUse);
		break;
	case 2 :
		NewMaxUse =  PlayerRef->SkillMaximumUse[2] -= 1;
		if(NewMaxUse <= 0)
		{
			FLinearColor UnUsableColor;
			UnUsableColor.R = 0.104167f;
			UnUsableColor.G = 0.104167f;
			UnUsableColor.B = 0.104167f;
			UnUsableColor.A = 1.f;
			PlayerWidget->Skill3->SkillIcon->SetColorAndOpacity(UnUsableColor);
		}
		PlayerWidget->Skill3->UpdateMaxUseText(NewMaxUse);
		break;
	case 3 :
		NewMaxUse =  PlayerRef->SkillMaximumUse[3] -= 1;
		if(NewMaxUse <= 0)
		{
			FLinearColor UnUsableColor;
			UnUsableColor.R = 0.104167f;
			UnUsableColor.G = 0.104167f;
			UnUsableColor.B = 0.104167f;
			UnUsableColor.A = 1.f;
			PlayerWidget->Skill4->SkillIcon->SetColorAndOpacity(UnUsableColor);
		}
		PlayerWidget->Skill4->UpdateMaxUseText(NewMaxUse);
		break;
	case 4 :
		NewMaxUse =  PlayerRef->SkillMaximumUse[4] -= 1;
		if(NewMaxUse <= 0)
		{
			FLinearColor UnUsableColor;
			UnUsableColor.R = 0.104167f;
			UnUsableColor.G = 0.104167f;
			UnUsableColor.B = 0.104167f;
			UnUsableColor.A = 1.f;
			PlayerWidget->Skill5->SkillIcon->SetColorAndOpacity(UnUsableColor);
		}
		PlayerWidget->Skill5->UpdateMaxUseText(NewMaxUse);
		break;		
	default:
		break;
	}
}

void ACrpgMultiPlayerController::Pressed1()
{

	if(PlayerRef)
	{
		if(PlayerRef->bIsDead == true)
		{
			return;
		}
	}
	
	if(bIsPlayerTurn == false || bIsActionDone == true || PlayerRef->bIsSilence == true || PlayerRef->GetCharacterMovement()->Velocity.Size() > 0 || PlayerRef->bIsKnockDown == true) 
		return;

	if(PlayerRef->SkillMaximumUse[0] <= 0)
	{
		return;
	}
	
	PlayerRef->Skill1Pressed(PlayerRef->AreaActors[0],PlayerRef->AreaTypes[0], PlayerRef->MaxTargetAreaDistances[0], false);
}

void ACrpgMultiPlayerController::Pressed2()
{

	if(PlayerRef)
	{
		if(PlayerRef->bIsDead == true)
		{
			return;
		}
	}

	
	if(bIsPlayerTurn == false || bIsActionDone == true || PlayerRef->bIsSilence == true || PlayerRef->GetCharacterMovement()->Velocity.Size() > 0 || PlayerRef->bIsKnockDown == true) 
		return;

	if(PlayerRef->SkillMaximumUse[1] <= 0)
	{
		return;
	}
	
	PlayerRef->Skill1Pressed(PlayerRef->AreaActors[1],PlayerRef->AreaTypes[1], PlayerRef->MaxTargetAreaDistances[1], false);
}

void ACrpgMultiPlayerController::Pressed3()
{

	if(PlayerRef)
	{
		if(PlayerRef->bIsDead == true)
		{
			return;
		}
	}

	
	if(bIsPlayerTurn == false || bIsActionDone == true || PlayerRef->bIsSilence == true || PlayerRef->GetCharacterMovement()->Velocity.Size() > 0 || PlayerRef->bIsKnockDown == true) 
		return;

	if(PlayerRef->SkillMaximumUse[2] <= 0)
	{
		return;
	}
	
	PlayerRef->Skill1Pressed(PlayerRef->AreaActors[2],PlayerRef->AreaTypes[2], PlayerRef->MaxTargetAreaDistances[2], false);
}

void ACrpgMultiPlayerController::Pressed4()
{
	if(PlayerRef)
	{
		if(PlayerRef->bIsDead == true)
		{
			return;
		}
	}

	
	if(bIsPlayerTurn == false || bIsActionDone == true || PlayerRef->bIsSilence == true || PlayerRef->GetCharacterMovement()->Velocity.Size() > 0 || PlayerRef->bIsKnockDown == true) 
		return;

	if(PlayerRef->SkillMaximumUse[3] <= 0)
	{
		return;
	}
	
	PlayerRef->Skill1Pressed(PlayerRef->AreaActors[3],PlayerRef->AreaTypes[3], PlayerRef->MaxTargetAreaDistances[3], false);
}

void ACrpgMultiPlayerController::Pressed5()
{
	if(PlayerRef)
	{
		if(PlayerRef->bIsDead == true)
		{
			return;
		}
	}

	if(PlayerRef->Class == EClassType::Assassin && PlayerRef->SkillMaximumUse[4] > 0)
	{
		bIsActionDone = false;
		MovementPoint = 100.f;
		return;
	}

	if(bIsPlayerTurn == false || bIsActionDone == true || PlayerRef->bIsSilence == true || PlayerRef->GetCharacterMovement()->Velocity.Size() > 0 || PlayerRef->bIsKnockDown == true) 
		return;

	if(PlayerRef->SkillMaximumUse[4] <= 0)
	{
		return;
	}
	
	PlayerRef->Skill1Pressed(PlayerRef->AreaActors[4],PlayerRef->AreaTypes[4], PlayerRef->MaxTargetAreaDistances[4],false);
}

void ACrpgMultiPlayerController::PressedBasicAttack()
{

	if(PlayerRef)
	{
		if(PlayerRef->bIsDead == true)
		{
			return;
		}
	}

	
	if(bIsPlayerTurn == false || bIsActionDone == true || PlayerRef->GetCharacterMovement()->Velocity.Size() > 0 || PlayerRef->bIsKnockDown == true) 
		return;

	PlayerRef->Skill1Pressed(PlayerRef->AreaActors[6],PlayerRef->AreaTypes[6], PlayerRef->MaxTargetAreaDistances[6],true);

}

void ACrpgMultiPlayerController::BasicAttack()
{

	if(PlayerRef)
	{
		if(PlayerRef->bIsDead == true)
		{
			return;
		}
	}
	
	if(bIsPlayerTurn == false || bIsActionDone == true  || PlayerRef->GetCharacterMovement()->Velocity.Size() > 0 || PlayerRef->bIsKnockDown == true) 
		return;

	bIsActionDone = true;
	PlayerRef->Player_BasicAttack();
}




void ACrpgMultiPlayerController::ChangeCharacter_MultiCast_Implementation(int32 ClassType, FName ActorTag)
{
	UGameplayStatics::GetAllActorsOfClass(PlayerRef->GetWorld(), SearchedActors,FoundedActors);
	for(auto actor : FoundedActors)
	{
		if(actor->ActorHasTag(ActorTag))
		{
			ACrpgMultiCharacter* Ref = Cast<ACrpgMultiCharacter>(actor);
			if(Ref)
			{
				SelectNewPawnForClient(Ref);
				PlayerRef = Ref;
				SetNewPossessedCharDatas(Ref->Tags[1]);
			}
			
		}
	}
}

void ACrpgMultiPlayerController::ChangeCharacter_Server_Implementation(int32 ClassType, FName ActorTag)
{
	ChangeCharacter_MultiCast(ClassType,ActorTag);
}

void ACrpgMultiPlayerController::SetNewPossessedCharDatas(FName NewPawnTag)
{
	if(PlayerRef->PlayerDataTable)
	{
		/*
		FName RowName;
		if(ClassIndex == 0)
		{
			RowName = FName(TEXT("Mage"));
		}
		else if(ClassIndex == 1)
		{
			RowName = FName(TEXT("Archer"));
		}
		else if(ClassIndex == 2)
		{
			RowName = FName(TEXT("Barbarian"));
		}
		else if(ClassIndex == 3)
		{
			RowName = FName(TEXT("Paladin"));
		}
		*/
		
		RefreshWidgetDatasWhenPawnChange(NewPawnTag);
		
	}
}

void ACrpgMultiPlayerController::RefreshWidgetDatasWhenPawnChange(FName NewPawnRowName)
{
	FPlayerDataStruct* HoldDatas;
	static const FString ContextString(TEXT("ContextString"));
	HoldDatas = PlayerRef->PlayerDataTable->FindRow<FPlayerDataStruct>(NewPawnRowName,ContextString,true);
	if(HoldDatas)
	{
		PlayerWidget->Skill1->SkillIcon->SetBrushFromTexture(HoldDatas->DT_SkillIcons[0]);
		PlayerWidget->Skill2->SkillIcon->SetBrushFromTexture(HoldDatas->DT_SkillIcons[1]);
		PlayerWidget->Skill3->SkillIcon->SetBrushFromTexture(HoldDatas->DT_SkillIcons[2]);
		PlayerWidget->Skill4->SkillIcon->SetBrushFromTexture(HoldDatas->DT_SkillIcons[3]);
		PlayerWidget->Skill5->SkillIcon->SetBrushFromTexture(HoldDatas->DT_SkillIcons[4]);
		

		PlayerWidget->Skill1->MaxUse->SetText(FText::AsNumber(PlayerRef->SkillMaximumUse[0]));
		PlayerWidget->Skill2->MaxUse->SetText(FText::AsNumber(PlayerRef->SkillMaximumUse[1]));
		PlayerWidget->Skill3->MaxUse->SetText(FText::AsNumber(PlayerRef->SkillMaximumUse[2]));
		PlayerWidget->Skill4->MaxUse->SetText(FText::AsNumber(PlayerRef->SkillMaximumUse[3]));
		PlayerWidget->Skill5->MaxUse->SetText(FText::AsNumber(PlayerRef->SkillMaximumUse[4]));

		CheckSkillUsable(PlayerRef->SkillMaximumUse[0],0);
		CheckSkillUsable(PlayerRef->SkillMaximumUse[1],1);
		CheckSkillUsable(PlayerRef->SkillMaximumUse[2],2);
		CheckSkillUsable(PlayerRef->SkillMaximumUse[3],3);
		CheckSkillUsable(PlayerRef->SkillMaximumUse[4],4);
		
		PlayerWidget->Skill1->SettedHoverSkillName = HoldDatas->DT_SkillName[0];
		PlayerWidget->Skill2->SettedHoverSkillName = HoldDatas->DT_SkillName[1];
		PlayerWidget->Skill3->SettedHoverSkillName = HoldDatas->DT_SkillName[2];
		PlayerWidget->Skill4->SettedHoverSkillName = HoldDatas->DT_SkillName[3];
		PlayerWidget->Skill5->SettedHoverSkillName = HoldDatas->DT_SkillName[4];
		
		PlayerWidget->Skill1->SettedHoverDescription = HoldDatas->DT_SkillDescription[0];
		PlayerWidget->Skill2->SettedHoverDescription = HoldDatas->DT_SkillDescription[1];
		PlayerWidget->Skill3->SettedHoverDescription = HoldDatas->DT_SkillDescription[2];
		PlayerWidget->Skill4->SettedHoverDescription = HoldDatas->DT_SkillDescription[3];
		PlayerWidget->Skill5->SettedHoverDescription = HoldDatas->DT_SkillDescription[4];
		
		PlayerWidget->BasicAttackImage->SetBrushFromTexture(HoldDatas->DT_BasicAttackIcon); 
		PlayerWidget->BasicAttackName = HoldDatas->DT_BasicAttackName;
		PlayerWidget->BasicAttackDescription = HoldDatas->DT_BasicAttackDescription;
			
	}
}

void ACrpgMultiPlayerController::CheckSkillUsable(int NewPawnMaxUseCount,  int SkillIndex)
{
	if(SkillIndex == 0)
	{
		if(NewPawnMaxUseCount <= 0)
		{
			FLinearColor UnUsableColor;
			UnUsableColor.R = 0.104167f;
			UnUsableColor.G = 0.104167f;
			UnUsableColor.B = 0.104167f;
			UnUsableColor.A = 1.f;
			PlayerWidget->Skill1->SkillIcon->SetColorAndOpacity(UnUsableColor);
		}
		else
		{
			FLinearColor UnUsableColor;
			UnUsableColor.R = 255.f;
			UnUsableColor.G = 255.f;
			UnUsableColor.B = 255.f;
			UnUsableColor.A = 1.f;
			PlayerWidget->Skill1->SkillIcon->SetColorAndOpacity(UnUsableColor);
		}
	}
	else if(SkillIndex == 1)
	{
		if(NewPawnMaxUseCount <= 0)
		{
			FLinearColor UnUsableColor;
			UnUsableColor.R = 0.104167f;
			UnUsableColor.G = 0.104167f;
			UnUsableColor.B = 0.104167f;
			UnUsableColor.A = 1.f;
			PlayerWidget->Skill2->SkillIcon->SetColorAndOpacity(UnUsableColor);
		}
		else
		{
			FLinearColor UnUsableColor;
			UnUsableColor.R = 255.f;
			UnUsableColor.G = 255.f;
			UnUsableColor.B = 255.f;
			UnUsableColor.A = 1.f;
			PlayerWidget->Skill2->SkillIcon->SetColorAndOpacity(UnUsableColor);
		}
	}
	else if(SkillIndex == 2)
	{
		if(NewPawnMaxUseCount <= 0)
		{
			FLinearColor UnUsableColor;
			UnUsableColor.R = 0.104167f;
			UnUsableColor.G = 0.104167f;
			UnUsableColor.B = 0.104167f;
			UnUsableColor.A = 1.f;
			PlayerWidget->Skill3->SkillIcon->SetColorAndOpacity(UnUsableColor);
		}
		else
		{
			FLinearColor UnUsableColor;
			UnUsableColor.R = 255.f;
			UnUsableColor.G = 255.f;
			UnUsableColor.B = 255.f;
			UnUsableColor.A = 1.f;
			PlayerWidget->Skill3->SkillIcon->SetColorAndOpacity(UnUsableColor);
		}
	}
	else if(SkillIndex == 3)
	{
		if(NewPawnMaxUseCount <= 0)
		{
			FLinearColor UnUsableColor;
			UnUsableColor.R = 0.104167f;
			UnUsableColor.G = 0.104167f;
			UnUsableColor.B = 0.104167f;
			UnUsableColor.A = 1.f;
			PlayerWidget->Skill4->SkillIcon->SetColorAndOpacity(UnUsableColor);
		}
		else
		{
			FLinearColor UnUsableColor;
			UnUsableColor.R = 255.f;
			UnUsableColor.G = 255.f;
			UnUsableColor.B = 255.f;
			UnUsableColor.A = 1.f;
			PlayerWidget->Skill4->SkillIcon->SetColorAndOpacity(UnUsableColor);
		}
	}
	else if(SkillIndex == 4)
	{
		if(NewPawnMaxUseCount <= 0)
		{
			FLinearColor UnUsableColor;
			UnUsableColor.R = 0.104167f;
			UnUsableColor.G = 0.104167f;
			UnUsableColor.B = 0.104167f;
			UnUsableColor.A = 1.f;
			PlayerWidget->Skill5->SkillIcon->SetColorAndOpacity(UnUsableColor);
		}
		else
		{
			FLinearColor UnUsableColor;
			UnUsableColor.R = 255.f;
			UnUsableColor.G = 255.f;
			UnUsableColor.B = 255.f;
			UnUsableColor.A = 1.f;
			PlayerWidget->Skill5->SkillIcon->SetColorAndOpacity(UnUsableColor);
		}
	}
	
}

void ACrpgMultiPlayerController::SelectNewPawnForClient_Implementation(APawn* NewPawn)
{
	Possess(NewPawn);
}

void ACrpgMultiPlayerController::WhenReleaseTurn()
{
	if(PlayerRef)
	{
		PlayerRef->TurnDebuffCheck();
		PlayerRef->TurnBuffCheck();
		if(HasAuthority())
		{
			if(FlipFlopChose == 0)
			{
				FlipFlopChose = 1;
				FName TagName = FName(TEXT("Server1"));
				ChangeCharacter_Server(1,TagName);
				if(PlayerRef->ActorHasTag("Shadow"))
				{
					PlayerRef->ShadowModeWidgetWhenPawnChanged(true);
				}
				else
				{
					PlayerRef->ShadowModeWidgetWhenPawnChanged(false);
				}

				
				
			}
			else if(FlipFlopChose == 1)
			{
				FlipFlopChose = 2;
				FName TagName = FName(TEXT("Server2"));
				ChangeCharacter_Server(1,TagName);
				if(PlayerRef->ActorHasTag("Shadow"))
				{
					PlayerRef->ShadowModeWidgetWhenPawnChanged(true);
				}
				else
				{
					PlayerRef->ShadowModeWidgetWhenPawnChanged(false);
				}

				
			}
			else
			{
				FlipFlopChose = 0;
				FName TagName = FName(TEXT("Server"));
				ChangeCharacter_Server(2,TagName);
				if(PlayerRef->ActorHasTag("Shadow"))
				{
					PlayerRef->ShadowModeWidgetWhenPawnChanged(true);
				}
				else
				{
					PlayerRef->ShadowModeWidgetWhenPawnChanged(false);
				}

				
			}
		}
		else
		{
			if(FlipFlopChose == 0)
			{
				FlipFlopChose = 1;
				FName TagName = FName(TEXT("Client1"));
				ChangeCharacter_Server(0,TagName);
				if(PlayerRef->ActorHasTag("Shadow"))
				{
					PlayerRef->ShadowModeWidgetWhenPawnChanged(true);
				}
				else
				{
					PlayerRef->ShadowModeWidgetWhenPawnChanged(false);
				}
			}
			else if(FlipFlopChose == 1)
			{
				FlipFlopChose = 2;
				FName TagName = FName(TEXT("Client2"));
				ChangeCharacter_Server(0,TagName);
				if(PlayerRef->ActorHasTag("Shadow"))
				{
					PlayerRef->ShadowModeWidgetWhenPawnChanged(true);
				}
				else
				{
					PlayerRef->ShadowModeWidgetWhenPawnChanged(false);
				}
			}
			else
			{
				FlipFlopChose = 0;
				FName TagName = FName(TEXT("Client"));
				ChangeCharacter_Server(3,TagName);
				if(PlayerRef->ActorHasTag("Shadow"))
				{
					PlayerRef->ShadowModeWidgetWhenPawnChanged(true);
				}
				else
				{
					PlayerRef->ShadowModeWidgetWhenPawnChanged(false);
				}
			}
		}
	}
}



void ACrpgMultiPlayerController::OnSetDestinationPressed()
{

	if(PlayerRef)
	{
		if(PlayerRef->bIsDead == true)
		{
			bInputPressed = false;
			bIsTouch = false;
			StopMovement();
			return;
		}
	}
	
	if(bIsPlayerTurn == false || MovementPoint <= 0 || PlayerRef->ActiveState != Empty || PlayerRef->bIsCrippled == true || PlayerRef->bIsKnockDown == true)
	{
		bInputPressed = false;
		bIsTouch = false;
		StopMovement();
		return;
	}
	
	bInputPressed = true;
	
	StopMovement();
}

void ACrpgMultiPlayerController::OnSetDestinationReleased()
{
	if(PlayerRef)
	{
		if(PlayerRef->bIsDead == true)
		{
			bInputPressed = false;
			bIsTouch = false;
			StopMovement();
			return;
		}
	}

	
	if(bIsPlayerTurn == false || MovementPoint <= 0 || PlayerRef->ActiveState != Empty || PlayerRef->bIsCrippled == true || PlayerRef->bIsKnockDown == true)
	{
		bInputPressed = false;
		bIsTouch = false;
		StopMovement();
		return;
	}
	
	bInputPressed = false;

	
	if(FollowTime <= ShortPressThreshold)
	{
		
		FVector HitLocation = FVector::ZeroVector;
		FHitResult Hit;
		GetHitResultUnderCursor(ECC_Visibility, true, Hit);
		HitLocation = Hit.Location;

		
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, HitLocation);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, HitLocation, FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true, true, ENCPoolMethod::None, true);
	}
}

void ACrpgMultiPlayerController::OnTouchPressed(const ETouchIndex::Type FingerIndex, const FVector Location)
{

	if(PlayerRef)
	{
		if(PlayerRef->bIsDead == true)
		{
			bInputPressed = false;
			bIsTouch = false;
			StopMovement();
			return;
		}
	}

	
	if(bIsPlayerTurn == false || MovementPoint <= 0 || PlayerRef->ActiveState != Empty || PlayerRef->bIsCrippled == true || PlayerRef->bIsKnockDown == true)
	{
		bInputPressed = false;
		bIsTouch = false;
		StopMovement();
		return;
	}
	
	bIsTouch = true;
	OnSetDestinationPressed();
}

void ACrpgMultiPlayerController::OnTouchReleased(const ETouchIndex::Type FingerIndex, const FVector Location)
{

	if(PlayerRef)
	{
		if(PlayerRef->bIsDead == true)
		{
			bInputPressed = false;
			bIsTouch = false;
			StopMovement();
			return;
		}
	}


	
	if(bIsPlayerTurn == false || MovementPoint <= 0 || PlayerRef->ActiveState != Empty || PlayerRef->bIsCrippled == true || PlayerRef->bIsKnockDown == true)
	{
		bInputPressed = false;
		bIsTouch = false;
		StopMovement();
		return;
	}
	
	bIsTouch = false;
	OnSetDestinationReleased();
}


