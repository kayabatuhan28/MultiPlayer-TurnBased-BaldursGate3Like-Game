// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WBP_SkillSlot.generated.h"


UCLASS()
class CRPGMULTI_API UWBP_SkillSlot : public UUserWidget
{
	GENERATED_BODY()
public:

	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintReadWrite,meta=(BindWidget))
	class UImage* SkillIcon;

	UPROPERTY(BlueprintReadWrite,meta=(BindWidget))
	UImage* Border;

	UPROPERTY(BlueprintReadWrite,meta=(BindWidget))
	class UButton* Button;

	UPROPERTY(BlueprintReadWrite,meta=(BindWidget))
	class UTextBlock* MaxUse;

	UPROPERTY(BlueprintReadWrite,meta=(BindWidget))
	UTextBlock* SkillNumber;

	UFUNCTION(BlueprintNativeEvent)
	void SkillDataHovered();

	UFUNCTION(BlueprintNativeEvent)
	void SkillDataUnHovered();

	UPROPERTY(BlueprintReadWrite)
	FText SettedHoverSkillName;

	UPROPERTY(BlueprintReadWrite)
	FText SettedHoverDescription;

	UFUNCTION()
	void UpdateMaxUseText(int32 NewUsableCount);
	
	
};
