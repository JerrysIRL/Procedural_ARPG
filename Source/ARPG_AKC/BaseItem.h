// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AffixLibrary.h"
#include "AttributeSet.h"
#include "GameFramework/Actor.h"
#include "DataLibrary.h"
#include "BaseItem.generated.h"

class UAbilitySystemComponent;
class UGameplayEffect;
class UInventoryComponent;
class AMainGameHUD;
class UItemInfoWidget;
class UWidgetComponent;
class UAffixDataAsset;
class UItemDataAsset;
class UBoxComponent;
class UNiagaraComponent;
class UNiagaraSystem;


UCLASS()
class ARPG_AKC_API ABaseItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseItem();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;
	
	void GenerateItem(FItemGenericInfo& ItemData);

	UPROPERTY(VisibleAnywhere, Category = "Item Info")
	FItemGenericInfo GenerationInfo;

	UPROPERTY(EditAnywhere, Category = "Item Info|AffixData")
	TArray<UAffixDataAsset*> AffixDataAssets;
	
	UPROPERTY(VisibleAnywhere, Category = "Item Info|AffixData")
	FAffixesInfo AffixesInfo;

	UStaticMeshComponent* GetMeshComponent() const { return MeshComponent; }

	UBoxComponent* GetBoxComponent() const { return BoxComponent; }
	
	UFUNCTION()
	void AddToInventory(UPrimitiveComponent* TouchedComponent = nullptr, FKey ButtonPressed = FKey());

	UPROPERTY(VisibleAnywhere, Category = "Item Info")
	bool bAffixesGenerated = false;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditAnywhere, Category = "Component")
	UBoxComponent* BoxComponent;

	UPROPERTY(EditAnywhere, Category = "Item Info|AffixData")
	UDataTable* AffixTable;
	
	UPROPERTY(VisibleAnywhere, Category = "Item Info|ParticleEffect")
	UNiagaraComponent* NiagaraComponent;
	
	UPROPERTY()
	UItemInfoWidget* ItemInfoWidget;

	UPROPERTY()
	AMainGameHUD* MainGameHUD;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Inventory")
	UInventoryComponent* Inventory;
	
	

	UFUNCTION()
	void BindHoverEvents();
	
	UFUNCTION()
	void ShowWidget(UPrimitiveComponent* Component);
	
	UFUNCTION()
	void HideWidget(UPrimitiveComponent* Component);

	UFUNCTION()
	void GenerateParticleEffect();
	
private:
	UFUNCTION()
	void DestroyItem();
};
