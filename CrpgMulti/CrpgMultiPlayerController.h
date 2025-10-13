// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"
#include "GameFramework/PlayerController.h"
#include "CrpgMultiPlayerController.generated.h"


class UNiagaraSystem;

UCLASS()
class ACrpgMultiPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ACrpgMultiPlayerController();

	virtual void BeginPlay() override;

	void GetLifetimeReplicatedProps( TArray< FLifetimeProperty > & OutLifetimeProps ) const;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	float ShortPressThreshold;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UNiagaraSystem* FXCursor;

	UPROPERTY(Replicated, EditDefaultsOnly,BlueprintReadWrite,Category="Turn Variables")
	bool bIsPlayerTurn;

	UPROPERTY(Replicated, EditDefaultsOnly,BlueprintReadWrite,Category="Turn Variables")
	bool bIsActionDone;

	UPROPERTY(Replicated, EditDefaultsOnly,BlueprintReadWrite,Category="Turn Variables")
	float MovementPoint;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	class ACrpgMultiCharacter* PlayerRef;

	
	UFUNCTION()
	void ReleaseTurn();
	
	UFUNCTION(Server,Unreliable)
	void ClientReleaseTurn();

	UFUNCTION()
	void ServerReleaseTurn();

	

	UFUNCTION()
	void ConsumeMovement();

	UFUNCTION(Client,Unreliable)
	void Client_ConsumeMovement();

	UFUNCTION()
	void Server_ConsumeMovement();
	
	UPROPERTY(EditAnywhere,Category="Class Types")
	TSubclassOf<UUserWidget> WidgetClass;
	UPROPERTY(Replicated, BlueprintReadWrite,Category="Widget")
	class UPlayerWidget* PlayerWidget;

	UFUNCTION(Client,Unreliable)
	void CreatePlayerWidget();
	
    UFUNCTION()
	void Update_MovementWidgetBar();

	
	DECLARE_DELEGATE_OneParam(FCastSkillDelegate, int);
	UFUNCTION()
	void CastSkill(int SkillIndex);

	void UpdateMaxUse(int SkillIndex);
	
	UFUNCTION()
	void Pressed1();
	UFUNCTION()
	void Pressed2();
	UFUNCTION()
	void Pressed3();
	UFUNCTION()
	void Pressed4();
	UFUNCTION()
	void Pressed5();

	UFUNCTION()
	void PressedBasicAttack();

	UFUNCTION()
	void BasicAttack();


	UFUNCTION()
	void WhenReleaseTurn();

	UPROPERTY()
	int FlipFlopChose;

	UFUNCTION()
	void RefreshWidgetDatasWhenPawnChange(FName NewPawnRowName);
	

	
	UFUNCTION()
	void CheckSkillUsable(int NewPawnMaxUseCount, int SkillIndex);

	

	UFUNCTION(BlueprintCallable, NetMulticast,Unreliable)
	void ChangeCharacter_MultiCast(int32 ClassType, FName ActorTag);

	UFUNCTION(BlueprintCallable, NetMulticast,Unreliable)
    void ChangeCharacter_Server(int32 ClassType, FName ActorTag);

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	TSubclassOf<AActor> SearchedActors;
	UPROPERTY(VisibleDefaultsOnly,BlueprintReadWrite)
	TArray<AActor*> FoundedActors;

	UFUNCTION(BlueprintCallable)
	void SetNewPossessedCharDatas(FName NewPawnTag);
	
	UFUNCTION(Server,Unreliable)
	void SelectNewPawnForClient(APawn* NewPawn);
	

protected:
	uint32 bMoveToMouseCursor : 1;
	
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;
	
	void OnSetDestinationPressed();
	void OnSetDestinationReleased();
	void OnTouchPressed(const ETouchIndex::Type FingerIndex, const FVector Location);
	void OnTouchReleased(const ETouchIndex::Type FingerIndex, const FVector Location);

private:
	bool bInputPressed; 
	bool bIsTouch; 
	float FollowTime; 
};


