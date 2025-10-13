// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseSkillActor.generated.h"

UCLASS()
class CRPGMULTI_API ABaseSkillActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ABaseSkillActor();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	TArray<FName> HittableTags;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	float SkillLifeTime;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	TArray<AActor*> HittedAct;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	TArray<UAnimMontage*> PlayerHitAnims;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	TArray<float> PlayerTakingDamage;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	bool bIsAttackSkill;

	UFUNCTION()
	void SetHittableForEnemy(TArray<FName> HoldActTag);

	UFUNCTION()
	void SetHittableForFriend(TArray<FName> HoldActTag);

	UFUNCTION(BlueprintCallable)
	void CheckCanHitSingleEnemy(AActor* ActorRef, UAnimMontage*& OutAnim, float& OutDamage, bool& OutHitted);

	UFUNCTION(BlueprintCallable)
    void CheckCanHitMultipleEnemy(AActor* ActorRef, UAnimMontage*& OutAnim, float& OutDamage, bool& OutHitted);

	UFUNCTION(BlueprintCallable)
	void SetHittedActorRotation(AActor* HittedActorRef, float Roll, float Pitch, float Yaw);

	UFUNCTION(BlueprintCallable,NetMulticast,Unreliable)
	void SetHittedActorRotation_MultiCast(AActor* HittedActorRef, float Roll, float Pitch, float Yaw);

	UFUNCTION(BlueprintCallable,Server,Unreliable)
	void SetHittedActorRotation_Server(AActor* HittedActorRef, float Roll, float Pitch, float Yaw);

};
