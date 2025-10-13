// Fill out your copyright notice in the Description page of Project Settings.


#include "ANS_SetMovementMode.h"

#include "CrpgMulti/CrpgMultiCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UANS_SetMovementMode::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                       float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	ACrpgMultiCharacter* PlayerRef = Cast<ACrpgMultiCharacter>(MeshComp->GetOwner());
	if(PlayerRef)
	{
		PlayerRef->GetCharacterMovement()->MovementMode = MovementMode;
	}
}

void UANS_SetMovementMode::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	ACrpgMultiCharacter* PlayerRef = Cast<ACrpgMultiCharacter>(MeshComp->GetOwner());
	if(PlayerRef)
	{
		PlayerRef->GetCharacterMovement()->MovementMode = EMovementMode::MOVE_Walking;
	}
}
