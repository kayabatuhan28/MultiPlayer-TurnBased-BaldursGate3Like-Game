// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillHoverWidget.h"

#include "Components/TextBlock.h"

void USkillHoverWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SkillNameTxt->SetText(SkillName1);
	SkillDescriptionTxt->SetText(SkillDescription1);
	//GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Green,TEXT("Anann12"));
}
