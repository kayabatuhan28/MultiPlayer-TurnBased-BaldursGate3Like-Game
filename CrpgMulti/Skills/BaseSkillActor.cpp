// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseSkillActor.h"

#include "CrpgMulti/CrpgMultiCharacter.h"


ABaseSkillActor::ABaseSkillActor()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
}

void ABaseSkillActor::BeginPlay()
{
	Super::BeginPlay();

	ACrpgMultiCharacter* OwnerPlayer = Cast<ACrpgMultiCharacter>(GetOwner());
	if(OwnerPlayer)
	{
		if(bIsAttackSkill == true)
		{
			SetHittableForEnemy(OwnerPlayer->Tags);
		}
		else
		{
			SetHittableForFriend(OwnerPlayer->Tags);
		}
	}
	
}

void ABaseSkillActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseSkillActor::SetHittableForFriend(TArray<FName> HoldActTag)
{
	
	if(HittableTags.Num() < 1 || HoldActTag.Num() < 1)
		return;
	
	if(HoldActTag[0] == "Server" || HoldActTag[0] == "Server1" || HoldActTag[0] == "Server2" )
	{
		HittableTags[0] = "Server";
		HittableTags[1] = "Server1";
		HittableTags[2] = "Server2";
	}
	else
	{
		HittableTags[0] = "Client";
		HittableTags[1] = "Client1";
		HittableTags[2] = "Client2";
	}
}

void ABaseSkillActor::CheckCanHitSingleEnemy(AActor* ActorRef, UAnimMontage*& OutAnim, float& OutDamage, bool& OutHitted)
{
	OutAnim = nullptr;
	OutDamage = 0.f;
	OutHitted = false;
	
	if(ActorRef->ActorHasTag(HittableTags[0]) || ActorRef->ActorHasTag(HittableTags[1]) || ActorRef->ActorHasTag(HittableTags[2]))
	{
		if(ActorRef->Tags[1] == "Archer")
		{
			OutAnim = PlayerHitAnims[0];
			OutDamage = PlayerTakingDamage[0];
			OutHitted = true;
		}
		else if(ActorRef->Tags[1] == "Mage")
		{
			OutAnim = PlayerHitAnims[1];
			OutDamage = PlayerTakingDamage[1];
			OutHitted = true;
		}
		else if(ActorRef->Tags[1] == "Paladin")
		{
			OutAnim = PlayerHitAnims[2];
			OutDamage = PlayerTakingDamage[2];
			OutHitted = true;
		}
		else if(ActorRef->Tags[1] == "Barbarian")
		{
			OutAnim = PlayerHitAnims[3];
			OutDamage = PlayerTakingDamage[3];
			OutHitted = true;
		}
		else if(ActorRef->Tags[1] == "Assassin")
		{
			OutAnim = PlayerHitAnims[4];
			OutDamage = PlayerTakingDamage[4];
			OutHitted = true;
		}
		else if(ActorRef->Tags[1] == "Monk")
		{
			OutAnim = PlayerHitAnims[5];
			OutDamage = PlayerTakingDamage[5];
			OutHitted = true;
		}
	}
}

void ABaseSkillActor::CheckCanHitMultipleEnemy(AActor* ActorRef, UAnimMontage*& OutAnim, float& OutDamage,
	bool& OutHitted)
{
	
	OutAnim = nullptr;
	OutDamage = 0.f;
	OutHitted = false;
	
	if(ActorRef->ActorHasTag(HittableTags[0]) || ActorRef->ActorHasTag(HittableTags[1]) || ActorRef->ActorHasTag(HittableTags[2]))
	{
		if(ActorRef->Tags[1] == "Archer")
		{
			if(!HittedAct.Contains(ActorRef))
			{
				HittedAct.Add(ActorRef);
				OutAnim = PlayerHitAnims[0];
				OutDamage = PlayerTakingDamage[0];
				OutHitted = true;
			}
		}
		else if(ActorRef->Tags[1] == "Mage")
		{
			if(!HittedAct.Contains(ActorRef))
			{
				HittedAct.Add(ActorRef);
				OutAnim = PlayerHitAnims[1];
				OutDamage = PlayerTakingDamage[1];
				OutHitted = true;
			}
		}
		else if(ActorRef->Tags[1] == "Paladin")
		{
			if(!HittedAct.Contains(ActorRef))
			{
				HittedAct.Add(ActorRef);
				OutAnim = PlayerHitAnims[2];
				OutDamage = PlayerTakingDamage[2];
				OutHitted = true;
			}
		}
		else if(ActorRef->Tags[1] == "Barbarian")
		{
			if(!HittedAct.Contains(ActorRef))
			{
				HittedAct.Add(ActorRef);
				OutAnim = PlayerHitAnims[3];
				OutDamage = PlayerTakingDamage[3];
				OutHitted = true;
			}
		}
		else if(ActorRef->Tags[1] == "Assassin")
		{
			if(!HittedAct.Contains(ActorRef))
			{
				HittedAct.Add(ActorRef);
				OutAnim = PlayerHitAnims[4];
				OutDamage = PlayerTakingDamage[4];
				OutHitted = true;
			}
		}
		else if(ActorRef->Tags[1] == "Monk")
		{
			if(!HittedAct.Contains(ActorRef))
			{
				HittedAct.Add(ActorRef);
				OutAnim = PlayerHitAnims[5];
				OutDamage = PlayerTakingDamage[5];
				OutHitted = true;
			}
		}
	}
	
}

void ABaseSkillActor::SetHittableForEnemy(TArray<FName> HoldActTag)
{
	if(HittableTags.Num() < 1 || HoldActTag.Num() < 1)
		return;
	
	if(HoldActTag[0] == "Server" || HoldActTag[0] == "Server1" || HoldActTag[0] == "Server2")
	{
		HittableTags[0] = "Client";
		HittableTags[1] = "Client1";
		HittableTags[2] = "Client2";
	}
	else
	{
		HittableTags[0] = "Server";
		HittableTags[1] = "Server1";
		HittableTags[2] = "Server2";
	}
}


void ABaseSkillActor::SetHittedActorRotation(AActor* HittedActorRef, float Roll, float Pitch, float Yaw)
{
	SetHittedActorRotation_Server(HittedActorRef,Roll,Pitch,Yaw);
}

void ABaseSkillActor::SetHittedActorRotation_MultiCast_Implementation(AActor* HittedActorRef, float Roll, float Pitch, float Yaw)
{
	HittedActorRef->SetActorRotation(FRotator(Pitch,Yaw,Roll));
}

void ABaseSkillActor::SetHittedActorRotation_Server_Implementation(AActor* HittedActorRef, float Roll, float Pitch, float Yaw)
{
	SetHittedActorRotation_MultiCast(HittedActorRef,Roll,Pitch,Yaw);
}
