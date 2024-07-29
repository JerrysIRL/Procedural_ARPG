// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "AbilitySystemInterface.h"
#include "CoreMinimal.h"
#include "AffixDataAsset.h"
#include "BaseGameplayAbility.h"
#include "GameplayEffectTypes.h"
#include "GameFramework/Character.h"
#include "ARPG_AKCCharacter.generated.h"

class ABaseEnemy;
enum class ETileType : uint8;
class AMainGameHUD;
class UBoxComponent;
class ABaseItem;
class UEquipmentComponent;
class UInventoryComponent;
class UCameraComponent;
class USpringArmComponent;
class UGameplayEffect;

UCLASS(Blueprintable)
class AARPG_AKCCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AARPG_AKCCharacter();

	virtual void BeginPlay() override;
	
	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(EditAnywhere)
	UBoxComponent* ItemChecker;
protected:
	UPROPERTY(EditAnywhere, Category = "GAS")
	class USkillAttributes* SkillAttributes;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS")
	class USkillTreeManager* SkillTreeManager;
	
	UPROPERTY(EditAnywhere, Category ="GAS", meta = (AllowPrivateAccess = "true"))
	TArray<TSubclassOf<UBaseGameplayAbility>> StartAbilities;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite, Category = "GAS")
	TArray<UBaseGameplayAbility*> Abilities;
	
	UPROPERTY(EditAnywhere, Category ="GAS", meta = (AllowPrivateAccess = "true"))
	TArray<TSubclassOf<UGameplayEffect>> StartEffects;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category ="GAS", meta = (AllowPrivateAccess = "true"))
	class UAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	UInventoryComponent* InventoryComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment", meta = (AllowPrivateAccess = "true"))
	UEquipmentComponent* EquipmentComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category ="GAS", meta = (AllowPrivateAccess = "true"))
	const class UPlayerAttributeSet* PlayerAttributeSet;

	UFUNCTION()
	void GiveExperience(ABaseEnemy* Sender);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Experience")
	TSubclassOf<UGameplayEffect> ExperienceGameplayEffect;
	
public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override
	{
		return AbilitySystemComponent;	
	}

	UEquipmentComponent* GetEquipmentComponent() const { return EquipmentComponent; }
private:
	
	void GiveStartEffects();

	void GiveEffect(FGameplayEffectSpecHandle& NewHandle) const;

	UPROPERTY()
	AMainGameHUD* MainGameHud;

	FORCEINLINE FVector2D GetGameResolution()
	{
		FVector2D Result = FVector2D( 1, 1 );

		Result.X = GSystemResolution.ResX;
		Result.Y = GSystemResolution.ResY;

		return Result;
	}

	UPROPERTY(EditAnywhere, Category = "Sound")
	TMap<ETileType, USoundWave*> TileSounds;
	
	TKeyValuePair<ETileType, USoundWave*> CurrentlyPlayingSound;

	UPROPERTY(EditAnywhere, Category = "Sound")
	UAudioComponent* AudioComponent;

	FTimerHandle SoundTimerHandle;
	
protected:
	UFUNCTION(BlueprintImplementableEvent)
	void SetupComplete();

	UFUNCTION()
	void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnComponentExitOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void CheckTileForSound();
	
	UPROPERTY(EditAnywhere)
	bool bShowItems = false;
};

