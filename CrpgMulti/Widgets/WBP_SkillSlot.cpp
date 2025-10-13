// Fill out your copyright notice in the Description page of Project Settings.


#include "WBP_SkillSlot.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

void UWBP_SkillSlot::NativeConstruct()
{
	Super::NativeConstruct();

	Button->OnHovered.AddDynamic(this,&UWBP_SkillSlot::SkillDataHovered);
	Button->OnUnhovered.AddDynamic(this, &UWBP_SkillSlot::SkillDataUnHovered);
}

void UWBP_SkillSlot::UpdateMaxUseText(int32 NewUsableCount)
{
	MaxUse->SetText(FText::AsNumber(NewUsableCount));
}

void UWBP_SkillSlot::SkillDataUnHovered_Implementation()
{
	
}

void UWBP_SkillSlot::SkillDataHovered_Implementation()
{
	
}


