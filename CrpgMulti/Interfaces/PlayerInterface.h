// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerInterface.generated.h"

UINTERFACE(NotBlueprintable)
class UPlayerInterface : public UInterface
{
	GENERATED_BODY()
};


class CRPGMULTI_API IPlayerInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	virtual void TakeHit(float TakingDamage, UAnimMontage* HitReaction, int HitType) = 0;

	UFUNCTION(BlueprintCallable)
	virtual void TakeBuff(int32 BuffType, float BuffValue) = 0;

	UFUNCTION(BlueprintCallable)
	virtual void TakeDeBuff(int32 DeBuffType) = 0;

	UFUNCTION(BlueprintCallable)
	virtual void ClearAllDebuf() = 0;

	UFUNCTION(BlueprintCallable)
	virtual void TakeHitLoop(float TakingDamage, int Time) = 0;
};
