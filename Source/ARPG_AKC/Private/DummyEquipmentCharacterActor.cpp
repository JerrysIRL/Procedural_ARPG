// Fill out your copyright notice in the Description page of Project Settings.


#include "DummyEquipmentCharacterActor.h"

#include "EquipmentComponent.h"
#include "ARPG_AKC/ARPG_AKCCharacter.h"
#include "Components/SceneCaptureComponent2D.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ADummyEquipmentCharacterActor::ADummyEquipmentCharacterActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>("Mesh");
	MeshComponent->SetupAttachment(RootComponent);
	
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	SpringArmComponent->SetupAttachment(MeshComponent);
	
	USceneCaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>("SceneCapturingComponent");
	USceneCaptureComponent->SetupAttachment(SpringArmComponent);

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>("WeaponMesh");
	WeaponMesh->SetupAttachment(MeshComponent);

	HeadMesh = CreateDefaultSubobject<UStaticMeshComponent>("HeadMesh");
	HeadMesh->SetupAttachment(MeshComponent);

	ChestMesh = CreateDefaultSubobject<UStaticMeshComponent>("ChestMesh");
	ChestMesh->SetupAttachment(MeshComponent);
}

// Called when the game starts or when spawned
void ADummyEquipmentCharacterActor::BeginPlay()
{
	Super::BeginPlay();

	AARPG_AKCCharacter* Player = Cast<AARPG_AKCCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	EquipmentComponent = Player->GetEquipmentComponent();
	
	if(Player)
	{
		auto CharacterMeshComponent = Player->GetMesh()->GetSkeletalMeshAsset();
		if(CharacterMeshComponent)
		{
			MeshComponent->SetSkeletalMesh(CharacterMeshComponent);
			MeshComponent->SetAnimInstanceClass(Player->GetMesh()->GetAnimInstance()->GetClass());
		}

		// Bind delegates
		EquipmentComponent->OnGearEquipChangeDelegate.AddDynamic(this, &ADummyEquipmentCharacterActor::OnGearEquipChange);
		EquipmentComponent->OnGearUnEquipChangeDelegate.AddDynamic(this, &ADummyEquipmentCharacterActor::OnGearUnequipChange);
	}
}

void ADummyEquipmentCharacterActor::OnGearEquipChange(UStaticMeshComponent* EquippedItemMesh, EItemSlotType SlotType)
{
	if (EquippedItemMesh && SlotType == EItemSlotType::WeaponSlot)
	{
		WeaponMesh->SetStaticMesh(EquippedItemMesh->GetStaticMesh());
		WeaponMesh->AttachToComponent(MeshComponent, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), FName("Item_R"));
		
	}
	else if (EquippedItemMesh && SlotType == EItemSlotType::HelmetSlot)
	{
		HeadMesh->SetStaticMesh(EquippedItemMesh->GetStaticMesh());
		HeadMesh->AttachToComponent(MeshComponent, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), FName("headSocket"));;
	}
	else if (EquipmentComponent && SlotType == EItemSlotType::ChestSlot)
	{
		ChestMesh->SetStaticMesh(EquippedItemMesh->GetStaticMesh());
		ChestMesh->AttachToComponent(MeshComponent, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), FName("chestSocket"));
	}
}

void ADummyEquipmentCharacterActor::OnGearUnequipChange(UStaticMeshComponent* EquippedItemMesh, EItemSlotType SlotType)
{
	if (EquippedItemMesh && SlotType == EItemSlotType::WeaponSlot)
	{
		WeaponMesh->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
	}
	else if (EquippedItemMesh && SlotType == EItemSlotType::HelmetSlot)
	{
		HeadMesh->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
	}
	else if (EquipmentComponent && SlotType == EItemSlotType::ChestSlot)
	{
		ChestMesh->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
	}
}
