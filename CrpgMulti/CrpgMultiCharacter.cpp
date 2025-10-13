// Copyright Epic Games, Inc. All Rights Reserved.

#include "CrpgMultiCharacter.h"

#include "CrpgMultiGameMode.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Materials/Material.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Widgets/PlayerOnHeadWidget.h"
#include "Widgets/PlayerWidget.h"
#include "Widgets/WBP_SkillSlot.h"

ACrpgMultiCharacter::ACrpgMultiCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;
	
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level
	
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
	
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	OnHeadWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("OnHeadWidget"));
	OnHeadWidgetComp->SetupAttachment(RootComponent);
}

void ACrpgMultiCharacter::BeginPlay()
{
	Super::BeginPlay();

	PlayerOnHeadWidget = Cast<UPlayerOnHeadWidget>(OnHeadWidgetComp->GetUserWidgetObject());
	if(!PlayerOnHeadWidget)
	{
		GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Green,TEXT("OnHead Widget Ref Yok!"));
	}
}

void ACrpgMultiCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
	if(bIsTargeting == true)
	{
		if(bIsSpesificLine == false)
		{
			SetTargetActLocation(0);
		}
		else
		{
			SetTargetActLocation(1);
		}
		
		SetActorRotationToTarget();
	}
	
}

void ACrpgMultiCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACrpgMultiCharacter, Class)
	DOREPLIFETIME(ACrpgMultiCharacter, SkillActors)
	DOREPLIFETIME(ACrpgMultiCharacter, Custom_SkillActors)
	DOREPLIFETIME(ACrpgMultiCharacter, PlayerOnHeadWidget)
	DOREPLIFETIME(ACrpgMultiCharacter, OnHeadWidgetComp)
	DOREPLIFETIME(ACrpgMultiCharacter, CurrentHealth)
	DOREPLIFETIME(ACrpgMultiCharacter, MaxHealth)
	DOREPLIFETIME(ACrpgMultiCharacter, DamageReduction)
	DOREPLIFETIME(ACrpgMultiCharacter, MainTargetAct)
	DOREPLIFETIME(ACrpgMultiCharacter, TargetAct)
	DOREPLIFETIME(ACrpgMultiCharacter, bIsSilence)
	DOREPLIFETIME(ACrpgMultiCharacter, bIsSlowed)
	DOREPLIFETIME(ACrpgMultiCharacter, bIsCrippled)
	DOREPLIFETIME(ACrpgMultiCharacter, bIsKnockDown)
	DOREPLIFETIME(ACrpgMultiCharacter, bIsDead)
	DOREPLIFETIME(ACrpgMultiCharacter, bIsTargetReleased)
}



void ACrpgMultiCharacter::SetPlayerData(FName RowName,  ACrpgMultiPlayerController* HoldedControllerRef,  FName AddedTeamTag)
{
	SetPlayerDataServer(RowName, AddedTeamTag);
	SetPlayerWidgetDatas(RowName,HoldedControllerRef);
}

void ACrpgMultiCharacter::SetPlayerWidgetDatas(FName RowName, ACrpgMultiPlayerController* HoldedControllerRef)
{
	if(PlayerDataTable)
	{
		FPlayerDataStruct* HoldDatas;
		static const FString ContextString(TEXT("ContextString"));
		HoldDatas = PlayerDataTable->FindRow<FPlayerDataStruct>(RowName,ContextString,true);
		if(HoldDatas)
		{
			SetPlayerWidgetDataStruct(HoldDatas,HoldedControllerRef);
		}
	}
}

void ACrpgMultiCharacter::SetPlayerDataMulticast_Implementation(FName RowName,  FName AddedTeamTag)
{
	if(PlayerDataTable)
	{
		FPlayerDataStruct* HoldDatas;
		static const FString ContextString(TEXT("ContextString"));
		HoldDatas = PlayerDataTable->FindRow<FPlayerDataStruct>(RowName,ContextString,true);
		if(HoldDatas)
		{
			SetPlayerDataStruct(HoldDatas, AddedTeamTag);
		}
	}
}

void ACrpgMultiCharacter::SetPlayerDataServer_Implementation(FName RowName,  FName AddedTeamTag)
{
	SetPlayerDataMulticast(RowName, AddedTeamTag);
}

void ACrpgMultiCharacter::SetPlayerWidgetDataStruct(FPlayerDataStruct* ReceivedData, ACrpgMultiPlayerController* HoldedControllerRef)
{
	if(HoldedControllerRef)
	{
		BasicAttackName = ReceivedData->DT_BasicAttackName;
		HoldedControllerRef->PlayerWidget->BasicAttackName = BasicAttackName;
		
		BasicAttackDescription = ReceivedData->DT_BasicAttackDescription;
		HoldedControllerRef->PlayerWidget->BasicAttackDescription = BasicAttackDescription;
		
		BasicAttackIcon = ReceivedData->DT_BasicAttackIcon;
		HoldedControllerRef->PlayerWidget->BasicAttackImage->SetBrushFromTexture(BasicAttackIcon);

		
		for(int i = 0; i < ReceivedData->DT_SkillIcons.Num() ; i++)
		{
			if(i == 0)
			{
				HoldedControllerRef->PlayerWidget->Skill1->SkillIcon->SetBrushFromTexture(ReceivedData->DT_SkillIcons[0]);
				HoldedControllerRef->PlayerWidget->Skill1->MaxUse->SetText(FText::AsNumber(ReceivedData->DT_SkillMaximumUse[0]));
				HoldedControllerRef->PlayerWidget->Skill1->SettedHoverSkillName = ReceivedData->DT_SkillName[0];
				HoldedControllerRef->PlayerWidget->Skill1->SettedHoverDescription = ReceivedData->DT_SkillDescription[0];
			}
			if(i == 1)
			{
				HoldedControllerRef->PlayerWidget->Skill2->SkillIcon->SetBrushFromTexture(ReceivedData->DT_SkillIcons[1]);
				HoldedControllerRef->PlayerWidget->Skill2->MaxUse->SetText(FText::AsNumber(ReceivedData->DT_SkillMaximumUse[1]));
				HoldedControllerRef->PlayerWidget->Skill2->SettedHoverSkillName = ReceivedData->DT_SkillName[1];
				HoldedControllerRef->PlayerWidget->Skill2->SettedHoverDescription = ReceivedData->DT_SkillDescription[1];
			}
			if(i == 2)
			{
				HoldedControllerRef->PlayerWidget->Skill3->SkillIcon->SetBrushFromTexture(ReceivedData->DT_SkillIcons[2]);
				HoldedControllerRef->PlayerWidget->Skill3->MaxUse->SetText(FText::AsNumber(ReceivedData->DT_SkillMaximumUse[2]));
				HoldedControllerRef->PlayerWidget->Skill3->SettedHoverSkillName = ReceivedData->DT_SkillName[2];
				HoldedControllerRef->PlayerWidget->Skill3->SettedHoverDescription = ReceivedData->DT_SkillDescription[2];
			}
			if(i == 3)
			{
				HoldedControllerRef->PlayerWidget->Skill4->SkillIcon->SetBrushFromTexture(ReceivedData->DT_SkillIcons[3]);
				HoldedControllerRef->PlayerWidget->Skill4->MaxUse->SetText(FText::AsNumber(ReceivedData->DT_SkillMaximumUse[3]));
				HoldedControllerRef->PlayerWidget->Skill4->SettedHoverSkillName = ReceivedData->DT_SkillName[3];
				HoldedControllerRef->PlayerWidget->Skill4->SettedHoverDescription = ReceivedData->DT_SkillDescription[3];
			}
			if(i == 4)
			{
				HoldedControllerRef->PlayerWidget->Skill5->SkillIcon->SetBrushFromTexture(ReceivedData->DT_SkillIcons[4]);
				HoldedControllerRef->PlayerWidget->Skill5->MaxUse->SetText(FText::AsNumber(ReceivedData->DT_SkillMaximumUse[4]));
				HoldedControllerRef->PlayerWidget->Skill5->SettedHoverSkillName = ReceivedData->DT_SkillName[4];
				HoldedControllerRef->PlayerWidget->Skill5->SettedHoverDescription = ReceivedData->DT_SkillDescription[4];
			}
		}
	}
}

void ACrpgMultiCharacter::SetPlayerDataStruct(FPlayerDataStruct* ReceivedData,  FName AddedTeamTag)
{
	GetMesh()->SetSkeletalMesh(ReceivedData->DT_SkeletalMesh);
	GetMesh()->Stop();
	GetMesh()->SetAnimInstanceClass(ReceivedData->DT_AnimBp);
	
	Class = ReceivedData->DT_ChoisenType;
	
	MaxHealth = ReceivedData->DT_Health;
	CurrentHealth = MaxHealth;

	DamageReduction = ReceivedData->DT_DamageReduction;

	PlayerOnHeadWidget->HealthBar->SetPercent(CurrentHealth / MaxHealth);
	PlayerOnHeadWidget->CurrentHealthTxt->SetText(FText::AsNumber(CurrentHealth));

	GetUpAnimation = ReceivedData->DT_GetUpAnimation;

	DieAnimation = ReceivedData->DT_DieAnimation;
	
	Tags.Add(AddedTeamTag);
	
	for(int i = 0; i < ReceivedData->DT_SkillIcons.Num() ; i++)
	{
		SkillIcon.Add(ReceivedData->DT_SkillIcons[i]);
	}

	for(int i = 0; i < ReceivedData->DT_Skills.Num() ; i++)
	{
		SkillActors.Add(ReceivedData->DT_Skills[i]);
	}

	for(int i = 0; i < ReceivedData->DT_SkillsAnim.Num() ; i++)
	{
		SkillsAnim.Add(ReceivedData->DT_SkillsAnim[i]);
	}

	for(int i = 0; i < ReceivedData->DT_SkillsCastTime.Num() ; i++)
	{
		SkillCastTime.Add(ReceivedData->DT_SkillsCastTime[i]);
	}

	for(int i = 0; i < ReceivedData->DT_SkillType.Num() ; i++)
	{
		SkillTypes.Add(ReceivedData->DT_SkillType[i]);
	}

	for(int i = 0; i < ReceivedData->DT_SkillSpawnSocketName.Num() ; i++)
	{
		SkillSpawnSocketNames.Add(ReceivedData->DT_SkillSpawnSocketName[i]);
	}

	for(int i = 0; i < ReceivedData->DT_SkillMaximumUse.Num() ; i++)
	{
		SkillMaximumUse.Add(ReceivedData->DT_SkillMaximumUse[i]);
	}

	for(int i = 0; i < ReceivedData->DT_SkillName.Num() ; i++)
	{
		SkillName.Add(ReceivedData->DT_SkillName[i]);
	}

	for(int i = 0; i < ReceivedData->DT_SkillDescription.Num() ; i++)
	{
		SkillDescription.Add(ReceivedData->DT_SkillDescription[i]);
	}

	for(int i = 0; i < ReceivedData->DT_AreaActors.Num() ; i++)
	{
		AreaActors.Add(ReceivedData->DT_AreaActors[i]);
	}

	for(int i = 0; i < ReceivedData->DT_AreaTypes.Num() ; i++)
	{
		AreaTypes.Add(ReceivedData->DT_AreaTypes[i]);
	}

	for(int i = 0; i < ReceivedData->DT_MaxTargetAreaDistances.Num() ; i++)
	{
		MaxTargetAreaDistances.Add(ReceivedData->DT_MaxTargetAreaDistances[i]);
	}
	

	if(Class == EClassType::Mage)
	{
		Tags.Add("Mage");
		
	}
	else if(Class == EClassType::Archer)
	{
		Tags.Add("Archer");
	}
	else if(Class == EClassType::Barbarian)
	{
		Tags.Add("Barbarian");
	}
	else if(Class == EClassType::Paladin)
	{
		Tags.Add("Paladin");
	}
	else if(Class == EClassType::Assassin)
	{
		Tags.Add("Assassin");
		GetCharacterMovement()->MaxWalkSpeed = 600.f;
	}
	else if(Class == EClassType::Monk)
	{
		Tags.Add("Monk");
	}



	
}


void ACrpgMultiCharacter::Skill1Pressed(TSubclassOf<AActor> AreaActor, int32 AreaType, float MaxTargetAreaDistance, bool bIsBasicAttack)
{
	if(bIsBasicAttack == false && bIsSilence == true)
	{
		return;
	}
	
	ActiveState = EActiveState::Attacking;
	MaxSkillDistance = MaxTargetAreaDistance;
	//Server_SpawnTargetAct(AreaActor,AreaType);
	SpawnTargetAct(AreaActor,AreaType);
	
	bIsTargeting = true;
}

void ACrpgMultiCharacter::SpawnTargetAct(TSubclassOf<AActor> AreaActor, int32 AreaType)
{
	if(AreaType == 0) 
	{
		APlayerController* HoldController = UGameplayStatics::GetPlayerController(GetWorld(),0);
		if(HoldController)
		{
			bIsSpesificLine = false;
			//GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Green,TEXT("Targeting!!"));
			FHitResult HitResult;
			HoldController->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Visibility),true,HitResult);
			if(HitResult.bBlockingHit == true)
			{
				//GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Green,TEXT("Blocking!!"));
				TargetAct = AreaActor;
				FVector SpawnLoc;
				FRotator SpawnRot;
				FActorSpawnParameters Params;
				Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				Params.Instigator = this;
				SpawnLoc = HitResult.Location;
				SpawnRot = FRotator::ZeroRotator;
				MainTargetAct =	GetWorld()->SpawnActor<AActor>(TargetAct,SpawnLoc,SpawnRot,Params);
				
			}
		}
	}
	else if(AreaType == 1) 
	{
		bIsSpesificLine = true;
		TargetAct = AreaActor;
		FVector SpawnLoc;
		FRotator SpawnRot;
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		Params.Instigator = this;
		SpawnLoc = GetActorLocation();
		SpawnRot = GetActorRotation();
		MainTargetAct =	GetWorld()->SpawnActor<AActor>(TargetAct,SpawnLoc,SpawnRot,Params);
	}
}

void ACrpgMultiCharacter::SetTargetActLocation(int32 AreaType)
{
	if(AreaType == 0) // Target Area locationunu mouse locationuna setler
	{
		APlayerController* HoldController = UGameplayStatics::GetPlayerController(GetWorld(),0);
		if(HoldController)
		{
			if(MainTargetAct)
			{
				FHitResult HitResult;
				HoldController->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Visibility),true,HitResult);
				float CurrentDistance = GetDistanceTo(MainTargetAct);
				FVector NewLoc = HitResult.Location;
				float Distance = NewLoc.Distance(GetActorLocation(),NewLoc);
				//GEngine->AddOnScreenDebugMessage(-1,0.1f,FColor::Green,FString::Printf(TEXT("%f"),Distance));
				if(Distance < MaxSkillDistance)
				{
					MainTargetAct->SetActorLocation(HitResult.Location,true,false,ETeleportType::TeleportPhysics);
				}
				else
				{
					FVector TemporaryLoc = GetActorLocation() + (GetActorForwardVector() * MaxSkillDistance);
					TemporaryLoc.Z = 0.f;
					MainTargetAct->SetActorLocation(TemporaryLoc,true,false,ETeleportType::TeleportPhysics);
				}
				
			}
		}
		
	}
	else if(AreaType == 1) // Skile basılı tutulması halinde oyuncu dönerken target areayıda döndürür
	{
		if(MainTargetAct)
		{
			MainTargetAct->SetActorRotation(GetActorRotation());
		}
	}
	
}





//---------------------------------------------- SKİLL TUŞUNA BASARKEN KARAKTERİ MOUSE LOCATİONA BAKTIR -----------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------------------------------
void ACrpgMultiCharacter::SetActorRotationToTarget()
{
	APlayerController* HoldController = UGameplayStatics::GetPlayerController(GetWorld(),0);
	if(HoldController)
	{
		FHitResult HitResult;
		HoldController->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Visibility),true,HitResult);
		FRotator Rot =  UKismetMathLibrary::FindLookAtRotation(GetActorLocation(),HitResult.Location);
		Server_SetActorRotationToTarget(0,0,Rot.Yaw);
	}	
}


void ACrpgMultiCharacter::MultiCast_SetActorRotationToTarget_Implementation(float HoldedPitch, float HoldedRoll,
                                                                            float HoldedYaw)
{
	FRotator NewRot;
	NewRot.Pitch = HoldedPitch;
	NewRot.Roll = HoldedRoll;
	NewRot.Yaw = HoldedYaw;
	SetActorRotation(NewRot);
}

void ACrpgMultiCharacter::Server_SetActorRotationToTarget_Implementation(float HoldedPitch, float HoldedRoll,
	float HoldedYaw)
{
	MultiCast_SetActorRotationToTarget(HoldedPitch, HoldedRoll, HoldedYaw);
}
//---------------------------------------------- SKİLL TUŞUNA BASARKEN KARAKTERİ MOUSE LOCATİONA BAKTIR -----------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------------------------------





//---------------------------------------------------------------- HASAR ALMA -------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------------------------------
void ACrpgMultiCharacter::TakeHit(float TakingDamage, UAnimMontage* HitReaction,  int HitType)
{
	if(bIsDead == true || GetCharacterMovement()->IsFalling() == true)
		return;
	
	float PercentDamage = TakingDamage *  (DamageReduction / 100.f); 
	float NewDamage = TakingDamage - PercentDamage;
	
	if(HasAuthority())
	{
		MultiCast_UpdateHealthWidget(NewDamage, HitType);
	}

	if(bIsKnockDown == false && CurrentHealth > 0.f)
	{
		Server_PlayReplicatedMontage(HitReaction,1.f);
	}

	if(CurrentHealth <= 0)
	{
		Server_PlayReplicatedMontage(DieAnimation,1.f);
		WhenPlayerDie_Server(true);
	}

}

void ACrpgMultiCharacter::TakeHitLoop(float TakingDamage, int Time)
{
	if(bIsDead == true)
		return;

	
	LoopCount = 0.f;
	MaxLoopCount = Time;
		
	LoopHitDelegate.BindUFunction(this, FName("TakeHitLoopFunction"), TakingDamage);
	GetWorld()->GetTimerManager().SetTimer(LoopHitHandle, LoopHitDelegate, 0.1, true);
	
}

void ACrpgMultiCharacter::TakeHitLoopFunction(float TakingDamage)
{
	if(LoopCount + 0.1f >= MaxLoopCount)
	{
		GetWorld()->GetTimerManager().ClearTimer(LoopHitHandle);
		LoopCount = 0.f;
		return;
	}

	LoopCount += 0.1f;
	if(HasAuthority())
	{
		MultiCast_UpdateHealthWidget(TakingDamage, 0);
	}

	if(CurrentHealth <= 0)
	{
		Server_PlayReplicatedMontage(DieAnimation,1.f);
		WhenPlayerDie_Server(true);
	}
}

void ACrpgMultiCharacter::AssassinSkill5_Implementation()
{
	
}

//---------------------------------------------------------------- HASAR ALMA -------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------------------------------



void ACrpgMultiCharacter::MultiCast_UpdateHealthWidget_Implementation(float HoldTakingDamage,  int DamageType)
{
	CurrentHealth -= HoldTakingDamage;
	PlayerOnHeadWidget->PlayOnHeadTextAnim(DamageType,HoldTakingDamage);
	UpdateHpWidget_NotReplicated(CurrentHealth,MaxHealth);
}




//----------------------------------------------------- ONHEADWİDGETIN HASAR ALINCA GÜNCELLENMESİ -----------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------------------------------
void ACrpgMultiCharacter::UpdateHpWidget_NotReplicated(float CurrentHpRef, float MaxHpRef)
{
	PlayerOnHeadWidget->CurrentHealthTxt->SetText(FText::AsNumber(CurrentHpRef));
	PlayerOnHeadWidget->HealthBar->SetPercent(CurrentHpRef / MaxHpRef);
}
//----------------------------------------------------- ONHEADWİDGETIN HASAR ALINCA GÜNCELLENMESİ -----------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------------------------------



//----------------------------------------------------- SKILL CASTING ONCESİ TARGET ACT DESTROY -----------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------------------------------
void ACrpgMultiCharacter::DestroyTargetAct()
{
	if(MainTargetAct)
	{
		TargetAreaDestroyLoc = MainTargetAct->GetActorLocation();
		MainTargetAct->K2_DestroyActor();
		bIsTargeting = false;
	}
}
//----------------------------------------------------- ONHEADWİDGETIN HASAR ALINCA GÜNCELLENMESİ -----------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------------------------------


//----------------------------------------------------- SKİLL TUŞU BIRAKILDIĞI ZAMANDA SKİLL CASTİNG  -------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------------------------------
void ACrpgMultiCharacter::UseSkill(int SkillIndex)
{
	if(bIsSilence == true || bIsTargetReleased == true)
		return;
	
	Server_PlayReplicatedMontage(SkillsAnim[SkillIndex],1.f);
	FTimerDelegate CastDel;
	FTimerHandle CastHandle;
	
	DestroyTargetAct();
	
	ESkillType SettedType = SkillTypes[SkillIndex];
	FName NameSocketLoc = SkillSpawnSocketNames[SkillIndex];

	if(Class == EClassType::Archer)
	{
		if(SkillIndex == 0)
		{
			FTimerDelegate CastDel2;
			FTimerHandle CastHandle2;
			FTimerDelegate CastDel3;
			FTimerHandle CastHandle3;
		
			CastDel.BindUFunction(this, FName("SpawnSkill"), SettedType, SkillActors[SkillIndex],NameSocketLoc);
			GetWorld()->GetTimerManager().SetTimer(CastHandle, CastDel, SkillCastTime[SkillIndex], false);
			CastDel2.BindUFunction(this, FName("SpawnSkill"), SettedType, SkillActors[SkillIndex],NameSocketLoc);
			GetWorld()->GetTimerManager().SetTimer(CastHandle2, CastDel2, 0.8f, false);
			CastDel3.BindUFunction(this, FName("SpawnSkill"), SettedType, SkillActors[SkillIndex],NameSocketLoc);
			GetWorld()->GetTimerManager().SetTimer(CastHandle3, CastDel3, 1.31f, false);
		}
		else
		{
			CastDel.BindUFunction(this, FName("SpawnSkill"), SettedType, SkillActors[SkillIndex],NameSocketLoc);
			GetWorld()->GetTimerManager().SetTimer(CastHandle, CastDel, SkillCastTime[SkillIndex], false);
		}
		
	}
	else if(Class == EClassType::Assassin)
	{
		if(SkillIndex == 1)
		{
			ShadowMode(true);
			CastDel.BindUFunction(this, FName("SpawnSkill"), SettedType, SkillActors[SkillIndex],NameSocketLoc);
			GetWorld()->GetTimerManager().SetTimer(CastHandle, CastDel, SkillCastTime[SkillIndex], false);
		}
		else
		{
			CastDel.BindUFunction(this, FName("SpawnSkill"), SettedType, SkillActors[SkillIndex],NameSocketLoc);
			GetWorld()->GetTimerManager().SetTimer(CastHandle, CastDel, SkillCastTime[SkillIndex], false);
		}
		
	}
	else if(Class == EClassType::Monk)
	{
		if(SkillIndex == 1)
		{
			FTimerDelegate CastDel2;
			FTimerHandle CastHandle2;
			FTimerDelegate CastDel3;
			FTimerHandle CastHandle3;
			FTimerDelegate CastDel4;
			FTimerHandle CastHandle4;
			CastDel.BindUFunction(this, FName("SpawnSkill"), SettedType, SkillActors[SkillIndex],NameSocketLoc);
			GetWorld()->GetTimerManager().SetTimer(CastHandle, CastDel, SkillCastTime[SkillIndex], false);
			CastDel2.BindUFunction(this, FName("SpawnSkill"), SettedType, SkillActors[SkillIndex],NameSocketLoc);
			GetWorld()->GetTimerManager().SetTimer(CastHandle2, CastDel2, 0.38f, false);
			CastDel3.BindUFunction(this, FName("SpawnSkill"), SettedType, SkillActors[SkillIndex],NameSocketLoc);
			GetWorld()->GetTimerManager().SetTimer(CastHandle3, CastDel3, 0.52f, false);
			CastDel4.BindUFunction(this, FName("SpawnSkill"), SettedType, SkillActors[SkillIndex],NameSocketLoc);
			GetWorld()->GetTimerManager().SetTimer(CastHandle4, CastDel4, 0.65f, false);
		}
		else
		{
			CastDel.BindUFunction(this, FName("SpawnSkill"), SettedType, SkillActors[SkillIndex],NameSocketLoc);
			GetWorld()->GetTimerManager().SetTimer(CastHandle, CastDel, SkillCastTime[SkillIndex], false);
		}
	}
	else if(Class == EClassType::Mage)
	{
		CastDel.BindUFunction(this, FName("SpawnSkill"), SettedType, SkillActors[SkillIndex],NameSocketLoc);
		GetWorld()->GetTimerManager().SetTimer(CastHandle, CastDel, SkillCastTime[SkillIndex], false);
	}
	else if(Class == EClassType::Paladin)
	{
		CastDel.BindUFunction(this, FName("SpawnSkill"), SettedType, SkillActors[SkillIndex],NameSocketLoc);
		GetWorld()->GetTimerManager().SetTimer(CastHandle, CastDel, SkillCastTime[SkillIndex], false);
	}
	else if(Class == EClassType::Barbarian)
	{
		CastDel.BindUFunction(this, FName("SpawnSkill"), SettedType, SkillActors[SkillIndex],NameSocketLoc);
		GetWorld()->GetTimerManager().SetTimer(CastHandle, CastDel, SkillCastTime[SkillIndex], false);
	}
	
}



void ACrpgMultiCharacter::SpawnSkill(ESkillType EType, TSubclassOf<AActor> SkillAct, FName SpawnSocketLoc)
{
	
	FTransform Transform = CheckActorType(EType, SpawnSocketLoc);
	SpawnSkill_Server(SkillAct, Transform, EType, SpawnSocketLoc);
}

FTransform ACrpgMultiCharacter::CheckActorType(ESkillType EType, FName FSocketName)
{
	
	if(EType == ESkillType::NoType)
	{
		FTransform MakeTransform(GetMesh()->GetSocketRotation(FSocketName),GetMesh()->GetSocketLocation(FSocketName),FVector(1,1,1));
		return MakeTransform;
	}
	if(EType == ESkillType::ProjectileRotate)
	{
		APlayerController* HoldController = UGameplayStatics::GetPlayerController(GetWorld(),0);
		FHitResult HitResult;
		HoldController->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Visibility),true,HitResult);
		FRotator NewActorRot = UKismetMathLibrary::FindLookAtRotation(GetMesh()->GetSocketLocation(FSocketName), TargetAreaDestroyLoc);
		FTransform MakeTransform(NewActorRot,GetMesh()->GetSocketLocation(FSocketName),FVector(1,1,1));
		return MakeTransform;
	}
	if(EType == ESkillType::ProjectileNotRotate)
	{
		FTransform MakeTransform(GetActorRotation(),GetMesh()->GetSocketLocation(FSocketName),FVector(1,1,1));
		return MakeTransform;
	}
	if(EType == ESkillType::Attached)
	{
		FTransform MakeTransform(GetActorRotation(),GetMesh()->GetSocketLocation(FSocketName),FVector(1,1,1));
		return MakeTransform;
	}
	if(EType == ESkillType::CursorLocSpawned)
	{
		APlayerController* HoldController = UGameplayStatics::GetPlayerController(GetWorld(),0);
		FHitResult HitResult;
		HoldController->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Visibility),true,HitResult);
		FTransform MakeTransform(GetActorRotation(),TargetAreaDestroyLoc,FVector(1,1,1));
		return MakeTransform;
	}

	
	FTransform MakeTransform(GetActorRotation(),GetMesh()->GetSocketLocation(FSocketName),FVector(1,1,1));
	return MakeTransform;
}



void ACrpgMultiCharacter::Player_BasicAttack_Implementation()
{
	
}


void ACrpgMultiCharacter::SpawnSkill_MultiCast_Implementation(TSubclassOf<AActor> SkillAct, FTransform Transform, ESkillType SkillActorType, FName AttachActorSocket)
{
	if(HasAuthority())
	{
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		Params.Instigator = this;
		Params.Owner = this;
		//GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Green,TEXT("Anan0"));
		AActor* SpawnedAct;
		SpawnedAct = GetWorld()->SpawnActor<AActor>(SkillAct,Transform,Params);
		//GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Green,TEXT("Anan Mal Oc"));
		if(SkillActorType == ESkillType::Attached)
		{
			SpawnedAct->AttachToComponent(GetMesh(),FAttachmentTransformRules::SnapToTargetIncludingScale,AttachActorSocket);
		}
	}
}

void ACrpgMultiCharacter::SpawnSkill_Server_Implementation(TSubclassOf<AActor> SkillAct, FTransform Transform, ESkillType SkillActorType, FName AttachActorSocket)
{
	SpawnSkill_MultiCast(SkillAct,Transform,SkillActorType, AttachActorSocket);
}
//----------------------------------------------------- SKİLL TUŞU BIRAKILDIĞI ZAMANDA SKİLL CASTİNG  -------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------------------------------







//------------------------------------------------------------ TAKE BUFF AND BUFF EFFECTS -------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------------------------------
void ACrpgMultiCharacter::TakeBuff(int32 BuffType, float BuffValue)
{
	if(bIsDead == true)
		return;
	
	if(HasAuthority())
	{
		if(BuffType == 0) // Heal
		{
			MultiCast_HealBuff(BuffValue);
		}
	}

	if(BuffType == 1 && !Tags.Contains("Untouchable"))
	{
		AddDebuffTag_Server(FName(TEXT("Untouchable")));
		SetBuffImage_Server(true,0);
		
		GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Green,TEXT("Untouchable Buff Activated"));
	}

	if(BuffType == 2 && !Tags.Contains("IronBody"))
	{
		AddDebuffTag_Server(FName(TEXT("IronBody")));
		SetBuffImage_Server(true,1);
		DamageReduction = 50.f;
		
		GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Green,TEXT("IronBody Buff Activated"));
	}

	if(BuffType == 3 && !Tags.Contains("Shadow"))
	{
		AddDebuffTag_Server(FName(TEXT("Shadow")));
		SetBuffImage_Server(true,2);
		//DamageReduction = 50.f;
		
		GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Green,TEXT("Shadow Buff Activated"));
	}

	if(BuffType == 4 && !Tags.Contains("Rage"))
	{
		AddDebuffTag_Server(FName(TEXT("Rage")));
		SetBuffImage_Server(true,3);
		//DamageReduction = 50.f;
		
		GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Green,TEXT("Rage Buff Activated"));
	}

	if(BuffType == 5 && !Tags.Contains("PaladinSkill4"))
	{
		AddDebuffTag_Server(FName(TEXT("PaladinSkill4")));
		//SetBuffImage_Server(true,3);
		//DamageReduction = 50.f;
		
		GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Green,TEXT("PaladinSkill4 Activated"));
	}

	if(BuffType == 6 && !Tags.Contains("StealLife"))
	{
		AddDebuffTag_Server(FName(TEXT("StealLife")));
		SetBuffImage_Server(true,4);
		//DamageReduction = 50.f;
		
		GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Green,TEXT("StealLife Activated"));
	}
	
}

void ACrpgMultiCharacter::TurnBuffCheck()
{
	if(ActorHasTag("Untouchable"))
	{
		if(BuffTurnCount[1] <= 0)
		{
			GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Green,TEXT("Untouchable Effects Clear"));
			RemoveDebuffTag_Server(FName(TEXT("Untouchable")));
			BuffTurnCount[1] = 1;
			ClearAttachedActor(0);
			
			SetBuffImage_Server(false,0);
			
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Green,FString::Printf(TEXT("Untouchable Debuff Turn : %d"),BuffTurnCount[1]));
			BuffTurnCount[1] -= 1;
			
		}
	}

	if(ActorHasTag("IronBody"))
	{
		if(BuffTurnCount[2] <= 0)
		{
			GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Green,TEXT("IronBody Effects Clear"));
			RemoveDebuffTag_Server(FName(TEXT("IronBody")));
			BuffTurnCount[2] = 1;
			ClearAttachedActor(1);
			DamageReduction = 0.f;
			
			SetBuffImage_Server(false,1);
			
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Green,FString::Printf(TEXT("IronBody Debuff Turn : %d"),BuffTurnCount[2]));
			BuffTurnCount[2] -= 1;
			
		}
	}

	if(ActorHasTag("Shadow"))
	{
		if(BuffTurnCount[3] <= 0)
		{
			GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Green,TEXT("Shadow Effects Clear"));
			RemoveDebuffTag_Server(FName(TEXT("Shadow")));
			BuffTurnCount[3] = 1;
			//ClearAttachedActor(1);
			
			ShadowMode(false);
			SetBuffImage_Server(false,2);
			
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Green,FString::Printf(TEXT("Shadow Debuff Turn : %d"),BuffTurnCount[3]));
			BuffTurnCount[3] -= 1;
			
		}
	}

	if(ActorHasTag("Rage"))
	{
		if(BuffTurnCount[4] <= 0)
		{
			GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Green,TEXT("Rage Effects Clear"));
			RemoveDebuffTag_Server(FName(TEXT("Rage")));
			BuffTurnCount[4] = 1;
			ClearAttachedActor(2);
			
			SetBuffImage_Server(false,3);
			
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Green,FString::Printf(TEXT("Rage Debuff Turn : %d"),BuffTurnCount[4]));
			BuffTurnCount[4] -= 1;
			
		}
	}

	if(ActorHasTag("PaladinSkill4"))
	{
		if(BuffTurnCount[5] <= 0)
		{
			GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Green,TEXT("PaladinSkill4 Clear"));
			RemoveDebuffTag_Server(FName(TEXT("PaladinSkill4")));
			BuffTurnCount[5] = 1;
			ClearAttachedActor(3);
			//SetBuffImage_Server(false,3);
			
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Green,FString::Printf(TEXT("PaladinSkill4 Turn : %d"),BuffTurnCount[5]));
			BuffTurnCount[5] -= 1;
		}
	}
	
	if(ActorHasTag("StealLife"))
	{
		if(BuffTurnCount[6] <= 0)
		{
			GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Green,TEXT("StealLife Clear"));
			RemoveDebuffTag_Server(FName(TEXT("StealLife")));
			BuffTurnCount[6] = 1;
			
			//ClearAttachedActor(3);
			SetBuffImage_Server(false,4);
			
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Green,FString::Printf(TEXT("StealLife Turn : %d"),BuffTurnCount[6]));
			BuffTurnCount[6] -= 1;
			
		}
	}


	
}

void ACrpgMultiCharacter::ClearAttachedActor_Implementation(int SkillIndex)
{
	// In Bp
}

void ACrpgMultiCharacter::MultiCast_HealBuff_Implementation(float HealAmount)
{
	if(CurrentHealth + HealAmount >= MaxHealth)
	{
		CurrentHealth = MaxHealth;
	}
	else
	{
		CurrentHealth += HealAmount;
	}
	PlayerOnHeadWidget->PlayOnHeadTextAnim(1,HealAmount);
	UpdateHpWidget_NotReplicated(CurrentHealth,MaxHealth);
}

void ACrpgMultiCharacter::SetBuffImage_Server_Implementation(bool IsVisible, int IconType)
{
	SetBuffImage_MultiCast(IsVisible,IconType);
}

void ACrpgMultiCharacter::SetBuffImage_MultiCast_Implementation(bool IsVisible, int IconType)
{
	if(IsVisible == true)
	{
		if(IconType == 0) 
		{
			PlayerOnHeadWidget->UnTouchableBuffImage->SetRenderOpacity(1.f);
		}
		else if(IconType == 1) 
		{
			PlayerOnHeadWidget->IronBodyBuffImage->SetRenderOpacity(1.f);
		}
		else if(IconType == 2) 
		{
			PlayerOnHeadWidget->ShadowModeImage->SetRenderOpacity(1.f);
		}
		else if(IconType == 3) 
		{
			PlayerOnHeadWidget->RageBuffImage->SetRenderOpacity(1.f);
		}
		else if(IconType == 4) 
		{
			PlayerOnHeadWidget->StealLifeImage->SetRenderOpacity(1.f);
		}
	}
	else
	{
		if(IconType == 0)
		{
			PlayerOnHeadWidget->UnTouchableBuffImage->SetRenderOpacity(0.f);
		}
		else if(IconType == 1) 
		{
			PlayerOnHeadWidget->IronBodyBuffImage->SetRenderOpacity(0.f);
		}
		else if(IconType == 2) 
		{
			PlayerOnHeadWidget->ShadowModeImage->SetRenderOpacity(0.f);
		}
		else if(IconType == 3) 
		{
			PlayerOnHeadWidget->RageBuffImage->SetRenderOpacity(0.f);
		}
		else if(IconType == 4) 
		{
			PlayerOnHeadWidget->StealLifeImage->SetRenderOpacity(0.f);
		}
	}
}

//------------------------------------------------------------ TAKE BUFF AND BUFF EFFECTS -------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------------------------------




//------------------------------------------------------------ TAKE DEBUFF AND DEBUFF EFFECTS -------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------------
void ACrpgMultiCharacter::TakeDeBuff(int32 DeBuffType)
{
	if(DeBuffType == 0 && !Tags.Contains("Burn"))
	{
		AddDebuffTag_Server(FName(TEXT("Burn")));
		GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Green,TEXT("Burn Debuff Activated"));
		SetDebuffImage_Server(true,0);
	}
	else if(DeBuffType == 1 && !Tags.Contains("Silence"))
	{
		AddDebuffTag_Server(FName(TEXT("Silence")));
		bIsSilence = true;
		GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Green,TEXT("Silence Debuff Activated"));
		SetDebuffImage_Server(true,1);
	}
	else if(DeBuffType == 2 && !Tags.Contains("Slowed"))
	{
		AddDebuffTag_Server(FName(TEXT("Slowed")));
		bIsSlowed = true;
		SetMoveSpeed_Server(250.f);
		GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Green,TEXT("Slowed Debuff Activated"));
		SetDebuffImage_Server(true,2);
	}
	else if(DeBuffType == 3 && !Tags.Contains("Crippled"))
	{
		AddDebuffTag_Server(FName(TEXT("Crippled")));
		bIsCrippled = true;
		GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Green,TEXT("Crippled Debuff Activated"));
		SetDebuffImage_Server(true,3);
	}
	else if(DeBuffType == 4 && !Tags.Contains("KnockDown"))
	{
		AddDebuffTag_Server(FName(TEXT("KnockDown")));
		SetKnockDown();
		GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Green,TEXT("KnockDown Debuff Activated"));
		SetDebuffImage_Server(true,4);
	}
}

void ACrpgMultiCharacter::TurnDebuffCheck()
{
	if(ActorHasTag("Burn"))
	{
		if(DebuffTurnCount[0] <= 0)
		{
			GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Green,TEXT("Burn Effects Clear"));
			RemoveDebuffTag_Server(FName(TEXT("Burn")));
			DebuffTurnCount[0] = 2;
			SetDebuffImage_Server(false,0);
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Green,FString::Printf(TEXT("Burn Debuff Turn : %d"),DebuffTurnCount[0]));
			DebuffTurnCount[0] -= 1;
			if(HasAuthority())
			{
				BurnEffect_ServerUpdateHealth(10.f);
				BurnEffect_ServerSendDataToAllClient(CurrentHealth, 10.f);
			}
			else
			{
				BurnEffect_ClientUpdateHealth(10.f);
				BurnEffect_ClientSendDataToServer(CurrentHealth, 10.f);
			}
		}
	}

	if(ActorHasTag("Silence"))
	{
		if(DebuffTurnCount[1] <= 0)
		{
			GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Green,TEXT("Silence Effects Clear"));
			RemoveDebuffTag_Server(FName(TEXT("Silence")));
			bIsSilence = false;
			DebuffTurnCount[1] = 1;
			SetDebuffImage_Server(false,1);
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Green,FString::Printf(TEXT("Silence Debuff Turn : %d"),DebuffTurnCount[1]));
			DebuffTurnCount[1] -= 1;
		}
	}
	
	if(ActorHasTag("Slowed"))
	{
		if(DebuffTurnCount[2] <= 0)
		{
			GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Green,TEXT("Slowed Effects Clear"));
			RemoveDebuffTag_Server(FName(TEXT("Slowed")));
			bIsSlowed = false;
			if(ActorHasTag("Assassin"))
			{
				SetMoveSpeed_Server(600.f);
			}
			else
			{
				SetMoveSpeed_Server(500.f);
			}
			DebuffTurnCount[2] = 1;
			SetDebuffImage_Server(false,2);
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Green,FString::Printf(TEXT("Slowed Debuff Turn : %d"),DebuffTurnCount[2]));
			DebuffTurnCount[2] -= 1;
		}
	}

	if(ActorHasTag("Crippled"))
	{
		if(DebuffTurnCount[3] <= 0)
		{
			GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Green,TEXT("Crippled Effects Clear"));
			RemoveDebuffTag_Server(FName(TEXT("Crippled")));
			bIsCrippled = false;
			DebuffTurnCount[3] = 1;
			SetDebuffImage_Server(false,3);
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Green,FString::Printf(TEXT("Crippled Debuff Turn : %d"),DebuffTurnCount[3]));
			DebuffTurnCount[3] -= 1;
		}
	}

	if(ActorHasTag("Knockdown"))
	{
		if(DebuffTurnCount[4] <= 0)
		{
			GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Green,TEXT("Knockdown Effects Clear"));
			RemoveDebuffTag_Server(FName(TEXT("Knockdown")));
			Server_PlayReplicatedMontage(GetUpAnimation,1.f);
			SetKnockDown();
			DebuffTurnCount[4] = 1;
			SetDebuffImage_Server(false,4);
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Green,FString::Printf(TEXT("Knockdown Debuff Turn : %d"),DebuffTurnCount[4]));
			DebuffTurnCount[4] -= 1;
		}
	}
	
	
}

void ACrpgMultiCharacter::ClearAllDebuf()
{
	if(ActorHasTag("Burn"))
	{
		GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Green,TEXT("Burn Effects Clear"));
		RemoveDebuffTag_Server(FName(TEXT("Burn")));
		DebuffTurnCount[0] = 2;
		SetDebuffImage_Server(false,0);
	}

	if(ActorHasTag("Silence"))
	{
		GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Green,TEXT("Silence Effects Clear"));
		RemoveDebuffTag_Server(FName(TEXT("Silence")));
		bIsSilence = false;
		DebuffTurnCount[1] = 1;
		SetDebuffImage_Server(false,1);
	}

	if(ActorHasTag("Slowed"))
	{
		GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Green,TEXT("Slowed Effects Clear"));
		RemoveDebuffTag_Server(FName(TEXT("Slowed")));
		bIsSlowed = false;
		if(ActorHasTag("Assassin"))
		{
			SetMoveSpeed_Server(600.f);
		}
		else
		{
			SetMoveSpeed_Server(500.f);
		}
		DebuffTurnCount[2] = 1;
		SetDebuffImage_Server(false,2);
	}

	if(ActorHasTag("Crippled"))
	{
		GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Green,TEXT("Crippled Effects Clear"));
		RemoveDebuffTag_Server(FName(TEXT("Crippled")));
		bIsCrippled = false;
		DebuffTurnCount[3] = 1;
		SetDebuffImage_Server(false,3);
	}

	if(ActorHasTag("Knockdown"))
	{
		GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Green,TEXT("Knockdown Effects Clear"));
		RemoveDebuffTag_Server(FName(TEXT("Knockdown")));
		Server_PlayReplicatedMontage(GetUpAnimation,1.f);
		SetKnockDown();
		DebuffTurnCount[4] = 1;
		SetDebuffImage_Server(false,4);
	}
	
}




void ACrpgMultiCharacter::SetKnockDown()
{
	if(bIsKnockDown == true)
	{
		SetKnockDown_Server(false);
	}
	else
	{
		SetKnockDown_Server(true);
	}
}



void ACrpgMultiCharacter::WhenPlayerDie_Server_Implementation(bool IsDead)
{
	WhenPlayerDie_NetMultiCast(IsDead);
}

void ACrpgMultiCharacter::WhenPlayerDie_NetMultiCast_Implementation(bool IsDead)
{
	bIsDead = IsDead;
	PlayerOnHeadWidget->SetVisibility(ESlateVisibility::Hidden);
}

void ACrpgMultiCharacter::ChangePlayersName_Implementation(const FText& NewName,  bool bIsServer)
{
	
}

void ACrpgMultiCharacter::RemoveDebuffTag_NetMultiCast_Implementation(FName DebuffTag)
{
	Tags.Remove(DebuffTag);
}

void ACrpgMultiCharacter::RemoveDebuffTag_Server_Implementation(FName DebuffTag)
{
	RemoveDebuffTag_NetMultiCast(DebuffTag);
}

void ACrpgMultiCharacter::AddDebuffTag_NetMultiCast_Implementation(FName DebuffTag)
{
	Tags.Add(DebuffTag);
}

void ACrpgMultiCharacter::AddDebuffTag_Server_Implementation(FName DebuffTag)
{
	AddDebuffTag_NetMultiCast(DebuffTag);
}

void ACrpgMultiCharacter::SetKnockDown_MultiCast_Implementation(bool bNewSet)
{
	bIsKnockDown = bNewSet;
}


void ACrpgMultiCharacter::SetKnockDown_Server_Implementation(bool bNewSet)
{
	SetKnockDown_MultiCast(bNewSet);
}


void ACrpgMultiCharacter::BurnEffect_ServerUpdateHealth(float Damage)
{
	CurrentHealth -= Damage;
	PlayerOnHeadWidget->CurrentHealthTxt->SetText(FText::AsNumber(CurrentHealth));
	PlayerOnHeadWidget->HealthBar->SetPercent(CurrentHealth / MaxHealth);
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),CustomVfxArrays[0],GetActorLocation(),FRotator::ZeroRotator,FVector(0.75f,0.75f,0.75f));
	PlayerOnHeadWidget->PlayOnHeadTextAnim(0,Damage);
}

void ACrpgMultiCharacter::ShadowModeWidgetWhenPawnChanged_Implementation(bool bIsWidgetOpen)
{
	
}

void ACrpgMultiCharacter::ShadowMode_Implementation(bool bIsEnable)
{
	
}


void ACrpgMultiCharacter::BurnEffect_ServerSendDataToAllClient_Implementation(float NewCurrentHealth, float DamageText)
{
	CurrentHealth = NewCurrentHealth;
	PlayerOnHeadWidget->CurrentHealthTxt->SetText(FText::AsNumber(CurrentHealth));
	PlayerOnHeadWidget->HealthBar->SetPercent(CurrentHealth / MaxHealth);
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),CustomVfxArrays[0],GetActorLocation(),FRotator::ZeroRotator,FVector(0.75f,0.75f,0.75f));
	PlayerOnHeadWidget->PlayOnHeadTextAnim(0,DamageText);
}

void ACrpgMultiCharacter::BurnEffect_ClientUpdateHealth_Implementation(float Damage)
{
	CurrentHealth -= Damage;
	PlayerOnHeadWidget->CurrentHealthTxt->SetText(FText::AsNumber(CurrentHealth));
	PlayerOnHeadWidget->HealthBar->SetPercent(CurrentHealth / MaxHealth);
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),CustomVfxArrays[0],GetActorLocation(),FRotator::ZeroRotator,FVector(0.75f,0.75f,0.75f));
	PlayerOnHeadWidget->PlayOnHeadTextAnim(0,Damage);
}

void ACrpgMultiCharacter::BurnEffect_ClientSendDataToServer_Implementation(float NewCurrentHealth, float DamageText)
{
	CurrentHealth = NewCurrentHealth;
	PlayerOnHeadWidget->CurrentHealthTxt->SetText(FText::AsNumber(CurrentHealth));
	PlayerOnHeadWidget->HealthBar->SetPercent(CurrentHealth / MaxHealth);
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),CustomVfxArrays[0],GetActorLocation(),FRotator::ZeroRotator,FVector(0.75f,0.75f,0.75f));
	PlayerOnHeadWidget->PlayOnHeadTextAnim(0,DamageText);
}


void ACrpgMultiCharacter::SetMoveSpeed_Server_Implementation(float NewSpeed)
{
	SetMoveSpeed_MultiCast(NewSpeed);
}

void ACrpgMultiCharacter::SetMoveSpeed_MultiCast_Implementation(float NewSpeed)
{
	GetCharacterMovement()->MaxWalkSpeed = NewSpeed;
}


void ACrpgMultiCharacter::SetDebuffImage_Server_Implementation(bool IsVisible, int IconType)
{
	SetDebuffImage_MultiCast(IsVisible,IconType);
}


void ACrpgMultiCharacter::SetDebuffImage_MultiCast_Implementation(bool IsVisible, int IconType)
{
	if(IsVisible == true)
	{
		if(IconType == 0) 
		{
			PlayerOnHeadWidget->BurnEffectImage->SetRenderOpacity(1.f);
		}
		else if(IconType == 1) 
		{
			PlayerOnHeadWidget->SilenceEffectImage->SetRenderOpacity(1.f);
		}
		else if(IconType == 2) 
		{
			PlayerOnHeadWidget->SlowEffectImage->SetRenderOpacity(1.f);
		}
		else if(IconType == 3) 
		{
			PlayerOnHeadWidget->CrippledEffectImage->SetRenderOpacity(1.f);
		}
		else if(IconType == 4) 
		{
			PlayerOnHeadWidget->KnockDownEffectImage->SetRenderOpacity(1.f);
		}
	}
	else
	{
		if(IconType == 0)
		{
			PlayerOnHeadWidget->BurnEffectImage->SetRenderOpacity(0.f);
		}
		else if(IconType == 1) 
		{
			PlayerOnHeadWidget->SilenceEffectImage->SetRenderOpacity(0.f);
		}
		else if(IconType == 2) 
		{
			PlayerOnHeadWidget->SlowEffectImage->SetRenderOpacity(0.f);
		}
		else if(IconType == 3) 
		{
			PlayerOnHeadWidget->CrippledEffectImage->SetRenderOpacity(0.f);
		}
		else if(IconType == 4) 
		{
			PlayerOnHeadWidget->KnockDownEffectImage->SetRenderOpacity(0.f);
		}
	}
}

//------------------------------------------------------------ TAKE DEBUFF AND DEBUFF EFFECTS -------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------------




void ACrpgMultiCharacter::Server_PlayReplicatedMontage_Implementation(UAnimMontage* AnimMontage, float MontageSpeed)
{
	NetMultiCast_PlayReplicatedMontage(AnimMontage,MontageSpeed);
}

void ACrpgMultiCharacter::NetMultiCast_PlayReplicatedMontage_Implementation(UAnimMontage* AnimMontage,
	float MontageSpeed)
{
	PlayAnimMontage(AnimMontage,MontageSpeed);
}








