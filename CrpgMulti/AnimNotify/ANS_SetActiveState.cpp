// Fill out your copyright notice in the Description page of Project Settings.


#include "ANS_SetActiveState.h"
#include "CrpgMulti/CrpgMultiCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UANS_SetActiveState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                      float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	ACrpgMultiCharacter* PlayerRef = Cast<ACrpgMultiCharacter>(MeshComp->GetOwner());
	if(PlayerRef)
	{
		PlayerRef->ActiveState = NewState;
	}
}

void UANS_SetActiveState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	ACrpgMultiCharacter* PlayerRef = Cast<ACrpgMultiCharacter>(MeshComp->GetOwner());
	if(PlayerRef)
	{
		PlayerRef->ActiveState = EActiveState::Empty;
		PlayerRef->GetCharacterMovement()->Velocity = FVector(0,0,0); // Sadece Clientlerde olan animasyon sonrası süzülme sorunu için
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Green,TEXT("Player Ref Is Not Valid!"));
	}
}
