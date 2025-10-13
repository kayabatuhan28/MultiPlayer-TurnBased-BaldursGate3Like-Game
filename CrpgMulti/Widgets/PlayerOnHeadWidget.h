// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerOnHeadWidget.generated.h"


UCLASS()
class CRPGMULTI_API UPlayerOnHeadWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite,meta=(BindWidget))
	class UProgressBar* HealthBar;

	UPROPERTY(BlueprintReadWrite,meta=(BindWidget))
	class UTextBlock* CurrentHealthTxt;

	UPROPERTY(BlueprintReadWrite,meta=(BindWidget))
	class UTextBlock* ClassNameTxt;

	


	// Debuff Icon
	UPROPERTY(BlueprintReadWrite,meta=(BindWidget))
	class UImage* BurnEffectImage;

	UPROPERTY(BlueprintReadWrite,meta=(BindWidget))
	class UImage* SilenceEffectImage;

	UPROPERTY(BlueprintReadWrite,meta=(BindWidget))
	class UImage* SlowEffectImage;

	UPROPERTY(BlueprintReadWrite,meta=(BindWidget))
	class UImage* CrippledEffectImage;

	UPROPERTY(BlueprintReadWrite,meta=(BindWidget))
	class UImage* KnockDownEffectImage;
	

	// Buff Icon
	UPROPERTY(BlueprintReadWrite,meta=(BindWidget))
	class UImage* UnTouchableBuffImage;

	UPROPERTY(BlueprintReadWrite,meta=(BindWidget))
	class UImage* IronBodyBuffImage;

	UPROPERTY(BlueprintReadWrite,meta=(BindWidget))
	class UImage* ShadowModeImage;

	UPROPERTY(BlueprintReadWrite,meta=(BindWidget))
	class UImage* RageBuffImage;

	UPROPERTY(BlueprintReadWrite,meta=(BindWidget))
	class UImage* StealLifeImage;
	
	UFUNCTION(BlueprintNativeEvent)
	void PlayOnHeadTextAnim(int32 AnimationType, float TextNumber);
	// 0 - Damage Text
	// 1 - Heal Text
	// 2 - Critical Damage Text
	
	
};
