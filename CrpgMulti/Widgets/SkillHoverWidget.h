// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SkillHoverWidget.generated.h"


UCLASS()
class CRPGMULTI_API USkillHoverWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct() override;
	

	UPROPERTY(BlueprintReadWrite,meta=(BindWidget))
	class UTextBlock* SkillNameTxt;

	UPROPERTY(BlueprintReadWrite,meta=(BindWidget))
	class UTextBlock* SkillDescriptionTxt;

	UPROPERTY(BlueprintReadWrite,meta=(ExposeOnSpawn = true))
	FText SkillName1;

	UPROPERTY(BlueprintReadWrite,meta=(ExposeOnSpawn = true))
	FText SkillDescription1;
	
};
