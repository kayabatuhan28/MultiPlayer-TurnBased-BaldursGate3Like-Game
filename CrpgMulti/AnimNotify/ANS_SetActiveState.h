// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "CrpgMulti/CrpgMultiCharacter.h"
#include "ANS_SetActiveState.generated.h"

/**
 * 
 */
UCLASS()
class CRPGMULTI_API UANS_SetActiveState : public UAnimNotifyState
{
	
	GENERATED_BODY()
public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,meta=(ExposeOnSpawn = true))
	TEnumAsByte<EActiveState> NewState;
	
};
