// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CrpgMultiPlayerController.h"
#include "Components/WidgetComponent.h"
#include "Engine/DataTable.h"
#include "GameFramework/Character.h"
#include "Interfaces/PlayerInterface.h"
#include "CrpgMultiCharacter.generated.h"

UENUM(BlueprintType)
enum EClassType
{
	Default			 UMETA(DisplayName = "Default"),
	Mage			 UMETA(DisplayName = "Mage"),
	Archer			 UMETA(DisplayName = "Archer"),
	Barbarian		 UMETA(DisplayName = "Barbarian"),
	Paladin			 UMETA(DisplayName = "Paladin"),
	Assassin		 UMETA(DisplayName = "Assassin"),
	Monk		     UMETA(DisplayName = "Monk"),
};

UENUM(BlueprintType)
enum ESkillType
{
	NoType									UMETA(DisplayName = "NoType"),
	ProjectileRotate						UMETA(DisplayName = "ProjectileRotate"),
	ProjectileNotRotate						UMETA(DisplayName = "ProjectileNotRotate"),
	CursorLocSpawned						UMETA(DisplayName = "CursorLocSpawned"),
	Attached								UMETA(DisplayName = "Attached")
};

UENUM(BlueprintType)
enum EActiveState
{
	Empty				UMETA(DisplayName = "Empty"),
	Hitted				UMETA(DisplayName = "Hitted"),
	Attacking			UMETA(DisplayName = "Attacking"),
};

USTRUCT(BlueprintType)
struct FPlayerDataStruct : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	USkeletalMesh* DT_SkeletalMesh;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UClass*  DT_AnimBp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* DT_ClassIcon;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TEnumAsByte<EClassType> DT_ChoisenType;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float DT_Health;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float DT_DamageReduction;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TArray<UTexture2D*> DT_SkillIcons;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TArray<TSubclassOf<AActor>> DT_Skills;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UAnimMontage*> DT_SkillsAnim;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<float> DT_SkillsCastTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TEnumAsByte<ESkillType>> DT_SkillType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> DT_SkillSpawnSocketName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int> DT_SkillMaximumUse;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FText> DT_SkillName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FText> DT_SkillDescription;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<AActor>> DT_AreaActors;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int32> DT_AreaTypes;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<float> DT_MaxTargetAreaDistances;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText DT_BasicAttackName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText DT_BasicAttackDescription;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* DT_BasicAttackIcon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* DT_GetUpAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* DT_DieAnimation;
	
};

UCLASS(Blueprintable)
class ACrpgMultiCharacter : public ACharacter, public IPlayerInterface
{
	GENERATED_BODY()

public:
	ACrpgMultiCharacter();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	// Replication
	void GetLifetimeReplicatedProps( TArray< FLifetimeProperty > & OutLifetimeProps ) const;

	



	
	// Player İçin DataTableden Başlangıç Data Setlemeleri
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category = "Data Table")
	UDataTable* PlayerDataTable;
	UFUNCTION(BlueprintCallable)
	void SetPlayerData(FName RowName,  ACrpgMultiPlayerController* HoldedControllerRef, FName AddedTeamTag);
	UFUNCTION(NetMulticast,Unreliable)
	void SetPlayerDataMulticast(FName RowName,  FName AddedTeamTag);
	UFUNCTION(Server,Unreliable)
	void SetPlayerDataServer(FName RowName,  FName AddedTeamTag);
	void SetPlayerDataStruct(FPlayerDataStruct* ReceivedData,  FName AddedTeamTag);

	UFUNCTION()
	void SetPlayerWidgetDatas(FName RowName, ACrpgMultiPlayerController* HoldedControllerRef);
	void SetPlayerWidgetDataStruct(FPlayerDataStruct* ReceivedData, ACrpgMultiPlayerController* HoldedControllerRef);
	
	// Karakterlerin Class Enumu
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Class)
	TEnumAsByte<EClassType> Class;

	// Karakterlerin durumlarını belirten enum
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Class)
	TEnumAsByte<EActiveState> ActiveState;


	// On Head Widget Variables
	UPROPERTY(Replicated, EditAnywhere,Category="Class Types")
	UWidgetComponent* OnHeadWidgetComp;
	UPROPERTY(Replicated, BlueprintReadWrite,Category="Widget")
	class UPlayerOnHeadWidget* PlayerOnHeadWidget;

	// Stat Variables
	UPROPERTY(Replicated, EditDefaultsOnly,BlueprintReadWrite, Category = "Character Variables")
	float CurrentHealth;
	UPROPERTY(Replicated, EditDefaultsOnly,BlueprintReadWrite, Category = "Character Variables")
	float MaxHealth;
	UPROPERTY(Replicated, EditDefaultsOnly,BlueprintReadWrite, Category = "Character Variables")
	float DamageReduction;
	UPROPERTY(Replicated, EditDefaultsOnly,BlueprintReadWrite, Category = "Character Variables")
	UTexture2D* ClassIcon;

	// Character And Skill Variables
	UPROPERTY(Replicated, EditAnywhere,BlueprintReadWrite, Category="Character Variables")
	TArray<UTexture2D*> SkillIcon;
	UPROPERTY(Replicated,EditDefaultsOnly,BlueprintReadWrite, Category = "Character Variables")
	TArray<TSubclassOf<AActor>> SkillActors;
	UPROPERTY(Replicated,EditDefaultsOnly,BlueprintReadWrite, Category = "Character Variables")
	TArray<TSubclassOf<AActor>> Custom_SkillActors;
	UPROPERTY(Replicated, EditAnywhere,BlueprintReadWrite, Category="Character Variables")
	TArray<UAnimMontage* > SkillsAnim;
	UPROPERTY(Replicated, EditAnywhere,BlueprintReadWrite, Category="Character Variables")
	TArray<float> SkillCastTime;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Character Variables")
	TArray<TEnumAsByte<ESkillType>> SkillTypes;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Character Variables")
	TArray<FName> SkillSpawnSocketNames;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Character Variables")
	TArray<int> SkillMaximumUse;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Character Variables")
	TArray<FText> SkillName;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Character Variables")
	TArray<FText> SkillDescription;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Character Variables")
	TArray<TSubclassOf<AActor>> AreaActors;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Character Variables")
	TArray<int32> AreaTypes;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Character Variables")
	TArray<float> MaxTargetAreaDistances;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Character Variables")
	FText BasicAttackName;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Character Variables")
	FText BasicAttackDescription;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Character Variables")
	UTexture2D* BasicAttackIcon;
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	UAnimMontage* GetUpAnimation;
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	UAnimMontage* DieAnimation;
	

	
	
	UFUNCTION(BlueprintCallable)
	void Skill1Pressed(TSubclassOf<AActor> AreaActor, int32 AreaType, float MaxTargetAreaDistance, bool bIsBasicAttack);

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadWrite)
	bool bIsTargeting;
	
	
	UFUNCTION(BlueprintCallable)
	void SpawnTargetAct(TSubclassOf<AActor> AreaActor, int32 AreaType);

	UFUNCTION(BlueprintCallable)
	void SetTargetActLocation(int32 AreaType);

	UFUNCTION(BlueprintCallable)
	void DestroyTargetAct();
	

	UPROPERTY()
	bool bIsSpesificLine;

	UPROPERTY()
	FVector TargetAreaDestroyLoc;

	UPROPERTY(BlueprintReadWrite)
	float MaxSkillDistance;
	
	
	UFUNCTION(BlueprintCallable)
	void SetActorRotationToTarget();
	
	UFUNCTION(BlueprintCallable, Server,Unreliable)
	void Server_SetActorRotationToTarget(float HoldedPitch, float HoldedRoll, float HoldedYaw);

	UFUNCTION(BlueprintCallable, NetMulticast,Unreliable)
	void MultiCast_SetActorRotationToTarget(float HoldedPitch, float HoldedRoll, float HoldedYaw);

	
	
	UPROPERTY(Replicated,EditDefaultsOnly,BlueprintReadWrite)
	TSubclassOf<AActor> TargetAct;

	UPROPERTY(Replicated, EditDefaultsOnly,BlueprintReadWrite)
	AActor* MainTargetAct;


	
	
	UFUNCTION(BlueprintCallable)
	virtual void TakeHit(float TakingDamage, UAnimMontage* HitReaction,  int HitType) override;
	

	
	UFUNCTION(BlueprintCallable)
	virtual void TakeBuff(int32 BuffType, float BuffValue) override;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TArray<int32> BuffTurnCount;
	

	UFUNCTION(BlueprintCallable)
	virtual void TakeDeBuff(int32 DeBuffType) override;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TArray<int32> DebuffTurnCount;
	UFUNCTION(BlueprintCallable)
	void TurnDebuffCheck();
	UFUNCTION(BlueprintCallable)
	void TurnBuffCheck();

	UPROPERTY(EditDefaultsOnly)
	TArray<UParticleSystem*> CustomVfxArrays;

	UFUNCTION(BlueprintNativeEvent)
	void ClearAttachedActor(int SkillIndex);

	
	// Some Buff Conditions
	UPROPERTY(Replicated, EditDefaultsOnly,BlueprintReadWrite, Category = "Debuff Boolean")
	bool bIsProtectiveShieldActivated;// Becomes invulnerable for 1 turn

	
	
	

	// Some Debuff Conditions
	UPROPERTY(Replicated, EditDefaultsOnly,BlueprintReadWrite, Category = "Debuff Boolean")
	bool bIsSilence; // Can't use skill
	UPROPERTY(Replicated, EditDefaultsOnly,BlueprintReadWrite, Category = "Debuff Boolean")
	bool bIsSlowed; // Can Move but slow
	UPROPERTY(Replicated, EditDefaultsOnly,BlueprintReadWrite, Category = "Debuff Boolean")
	bool bIsCrippled; // Can't Move
	UPROPERTY(Replicated, EditDefaultsOnly,BlueprintReadWrite, Category = "Debuff Boolean")
	bool bIsKnockDown; // Can't Use Skill, Can't Move

	
	//-------------------------------------------------------GAMEPLAY TURN BASE BUFF AND DEBUFF EFFECTS-----------------------------------------------------
	// -----------------------------------------------------------------------------------------------------------------------------------------------------

	// Burn Effect --  Her Turn salımında damage verir
	UFUNCTION()
	void BurnEffect_ServerUpdateHealth(float Damage);
	UFUNCTION(NetMulticast,Unreliable)
	void BurnEffect_ServerSendDataToAllClient(float NewCurrentHealth, float DamageText);
	UFUNCTION(Client,Unreliable)
	void BurnEffect_ClientUpdateHealth(float Damage);
	UFUNCTION(Server,Unreliable)
	void BurnEffect_ClientSendDataToServer(float NewCurrentHealth, float DamageText);
	
	// Heal Effect
	UFUNCTION(NetMulticast,Reliable)
	void MultiCast_HealBuff(float HealAmount);

	// Assassin Shadow Effect
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	void ShadowMode(bool bIsEnable);
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	void ShadowModeWidgetWhenPawnChanged(bool bIsWidgetOpen);

	// Slow Effect
	UFUNCTION(Server,Unreliable)
	void SetMoveSpeed_Server(float NewSpeed);
	UFUNCTION(NetMulticast,Unreliable)
	void SetMoveSpeed_MultiCast(float NewSpeed);

	UFUNCTION(Server,Unreliable)
	void SetDebuffImage_Server(bool IsVisible, int IconType);
	UFUNCTION(NetMulticast,Unreliable)
	void SetDebuffImage_MultiCast(bool IsVisible, int IconType);

	UFUNCTION(Server,Unreliable)
	void SetBuffImage_Server(bool IsVisible, int IconType);
	UFUNCTION(NetMulticast,Unreliable)
	void SetBuffImage_MultiCast(bool IsVisible, int IconType);
	
	// -------------------------------------------------- GAMEPLAY TURN BASE BUFF AND DEBUFF EFFECTS------------------------------------------------------
	//----------------------------------------------------------------------------------------------------------------------------------------------------

	
	// Hasar Alma Sonrasında Canın OnHeadWidgetların Client ve Serverlerda Güncellendiği Kısım
	UFUNCTION(NetMulticast,Reliable)
	void MultiCast_UpdateHealthWidget(float HoldTakingDamage, int DamageType);
	
	UFUNCTION()
	void UpdateHpWidget_NotReplicated(float CurrentHpRef, float MaxHpRef);


	
	// Hasar Alma Sonrasında Canın OnHeadWidgetların Client ve Serverlerda Güncellendiği Kısım
	UFUNCTION(BlueprintCallable, Server,Reliable)
	void Server_PlayReplicatedMontage(UAnimMontage* AnimMontage, float MontageSpeed);

	UFUNCTION(BlueprintCallable, NetMulticast,Reliable)
	void NetMultiCast_PlayReplicatedMontage(UAnimMontage* AnimMontage, float MontageSpeed);
	

	//Try
	// Aim Sonrası skill Casting
	UFUNCTION(BlueprintCallable)
	void UseSkill(int SkillIndex);
	
	UFUNCTION(BlueprintCallable)
	void SpawnSkill(ESkillType EType, TSubclassOf<AActor> SkillAct, FName SpawnSocketLoc);
	
	UFUNCTION(BlueprintCallable, NetMulticast,Unreliable)
	void SpawnSkill_MultiCast(TSubclassOf<AActor> SkillAct, FTransform Transform, ESkillType SkillActorType, FName AttachActorSocket);

	UFUNCTION(BlueprintCallable, Server,Unreliable)
	void SpawnSkill_Server(TSubclassOf<AActor> SkillAct, FTransform Transform, ESkillType SkillActorType, FName AttachActorSocket);

	UFUNCTION(BlueprintCallable)
	FTransform CheckActorType(ESkillType EType, FName FSocketName);

	
	UFUNCTION(BlueprintNativeEvent)
	void Player_BasicAttack();


	UFUNCTION()
	void SetKnockDown();

	UFUNCTION(NetMulticast,Unreliable)
	void SetKnockDown_MultiCast(bool bNewSet);

	UFUNCTION(Server,Unreliable)
	void SetKnockDown_Server(bool bNewSet);

	

	UFUNCTION(NetMulticast,Unreliable)
	void AddDebuffTag_NetMultiCast(FName DebuffTag);

	UFUNCTION(Server,Unreliable)
	void AddDebuffTag_Server(FName DebuffTag);

	UFUNCTION(NetMulticast,Unreliable)
	void RemoveDebuffTag_NetMultiCast(FName DebuffTag);

	UFUNCTION(Server,Unreliable)
	void RemoveDebuffTag_Server(FName DebuffTag);

	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	void ChangePlayersName(const FText& NewName, bool bIsServer);

	UPROPERTY(Replicated, EditDefaultsOnly,BlueprintReadWrite,Category="Main Variables")
	bool bIsDead;

	UFUNCTION(Server,Unreliable)
	void WhenPlayerDie_Server(bool IsDead);

	UFUNCTION(NetMulticast,Unreliable)
	void WhenPlayerDie_NetMultiCast(bool IsDead);

	UFUNCTION(BlueprintCallable)
	virtual void ClearAllDebuf() override;


	FTimerDelegate LoopHitDelegate;
	FTimerHandle LoopHitHandle;
	
	UFUNCTION(BlueprintCallable)
	virtual void TakeHitLoop(float TakingDamage, int Time) override;

	UFUNCTION(BlueprintCallable)
	void TakeHitLoopFunction(float TakingDamage);

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	float LoopCount;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	float MaxLoopCount;

	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	void AssassinSkill5();

	UPROPERTY(Replicated, EditDefaultsOnly,BlueprintReadWrite)
	bool bIsTargetReleased;

	

private:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;
};







