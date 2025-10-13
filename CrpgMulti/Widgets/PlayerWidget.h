// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerWidget.generated.h"


UCLASS()
class CRPGMULTI_API UPlayerWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite,meta=(BindWidget))
	class UProgressBar* MovementBar;

	UPROPERTY(BlueprintReadWrite,meta=(BindWidget))
	class UTextBlock* TurnTxt;
	
	UPROPERTY(BlueprintReadWrite,meta=(BindWidget))
	class UImage* ActionImage;

	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	void SetActionImage(bool bActionDone);

	UPROPERTY(BlueprintReadWrite,meta=(BindWidget))
	UImage* BasicAttackImage;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	FText BasicAttackName;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	FText BasicAttackDescription;


	// Skill Slot
	UPROPERTY(BlueprintReadWrite,meta=(BindWidget))
	class UWBP_SkillSlot* Skill1;

	UPROPERTY(BlueprintReadWrite,meta=(BindWidget))
	UWBP_SkillSlot* Skill2;

	UPROPERTY(BlueprintReadWrite,meta=(BindWidget))
	UWBP_SkillSlot* Skill3;

	UPROPERTY(BlueprintReadWrite,meta=(BindWidget))
	UWBP_SkillSlot* Skill4;

	UPROPERTY(BlueprintReadWrite,meta=(BindWidget))
	UWBP_SkillSlot* Skill5;

	
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	void SetBasicAttackHoverData(int32 ClassType);
	// 0 -- Mage
	// 1 -- Archer
	// 2 -- Paladin
	// 3 -- Barbarian      EClassType enumunu kullanmak için crpgmultiplayer.h ı headera dahil etmemek adına int degisken tanımlandı.
	
};
