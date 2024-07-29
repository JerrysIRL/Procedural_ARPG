// Copyright Epic Games, Inc. All Rights Reserved.

#include "ARPG_AKCCharacter.h"
#include "AbilitySystemComponent.h"
#include "AffixDataAsset.h"
#include "ARPG_AKCGameMode.h"
#include "BaseEnemy.h"
#include "EquipmentComponent.h"
#include "FireballAbility.h"
#include "InventoryComponent.h"
#include "LevelTileManager.h"
#include "MainGameHUD.h"
#include "MainHUDWidget.h"
#include "Public\SkillAttributes.h"
#include "SkillTreeManager.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "PlayerAttributeSet.h"
#include "Tile.h"
#include "Components/AudioComponent.h"
#include "Components/BoxComponent.h"
#include "Enemy/EnemyDeathManager.h"
#include "Materials/Material.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

AARPG_AKCCharacter::AARPG_AKCCharacter()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>("AbilitySystemComponent");
	SkillTreeManager = CreateDefaultSubobject<USkillTreeManager>("SkillTreeManager");
	SkillAttributes = CreateDefaultSubobject<USkillAttributes>("SkillAttributes");

	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>("Inventory Component");
	EquipmentComponent = CreateDefaultSubobject<UEquipmentComponent>("Equipment Component");

	ItemChecker = CreateDefaultSubobject<UBoxComponent>("Item Checker");
	ItemChecker->SetupAttachment(RootComponent);

	ItemChecker->SetUsingAbsoluteRotation(true);

	AudioComponent = CreateDefaultSubobject<UAudioComponent>("Audio Component");
	AudioComponent->SetupAttachment(RootComponent);
}

void AARPG_AKCCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetTimerManager().SetTimer(SoundTimerHandle, this, &AARPG_AKCCharacter::CheckTileForSound, 1.f, true);
	
	ItemChecker->SetBoxExtent(FVector(GetGameResolution().X / 1.5, GetGameResolution().Y / 1.5, 100.f));
	
	MainGameHud = Cast<AMainGameHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
	
	ItemChecker->OnComponentBeginOverlap.AddDynamic(this, &AARPG_AKCCharacter::OnComponentBeginOverlap);
	
	ItemChecker->OnComponentEndOverlap.AddDynamic(this, &AARPG_AKCCharacter::OnComponentExitOverlap);
	
	if (IsValid(AbilitySystemComponent))
	{
		PlayerAttributeSet = AbilitySystemComponent->GetSet<UPlayerAttributeSet>();

		for (int i = 0; i < StartAbilities.Num(); ++i)
		{
			UBaseGameplayAbility* Ability = StartAbilities[i].GetDefaultObject();
			Ability->Initialize();
			Ability->SkillTreeIndex = i;
			SkillTreeManager->SkillTreeDataTables.Add(Ability->SkillTreeDataTable);
			Ability->AbilitySystemComponent = GetAbilitySystemComponent();
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(Ability, 0, i));
			Abilities.Add(Ability);
		}

		GiveStartEffects();
	}

	SkillTreeManager->CustomInitialize();
	SkillAttributes->CustomInitialize();

	if(auto level = Cast<ALevelTileManager>(GetWorld()->GetLevelScriptActor()))
	{
		level->EnemyDeathManager->OnEnemyDied.AddUniqueDynamic(this, &AARPG_AKCCharacter::GiveExperience);
	}
	// if (AARPG_AKCGameMode* GameMode = Cast<AARPG_AKCGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
	// {Experience);
	// 	GameMode->EnemyDeathManager->OnEnemyDied.AddUniqueDynamic(this, &AARPG_AKCCharacter::Give
	// }

	SetupComplete();
}

void AARPG_AKCCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
	
}

void AARPG_AKCCharacter::GiveExperience(ABaseEnemy* Sender)
{
	FGameplayEffectSpecHandle ExperienceEffectSpec = AbilitySystemComponent->MakeOutgoingSpec(
		ExperienceGameplayEffect,
		1,
		AbilitySystemComponent->MakeEffectContext()
		);
	ExperienceEffectSpec.Data.Get()->SetSetByCallerMagnitude(
		FGameplayTag::RequestGameplayTag("Calculation.Experience"),
		Sender->GetExperience()
		);

	AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*ExperienceEffectSpec.Data.Get());
}

void AARPG_AKCCharacter::GiveStartEffects()
{
	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	for (const auto Effect : StartEffects)
	{
		FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(Effect, 1, EffectContext);

		GiveEffect(NewHandle);
	}
}

void AARPG_AKCCharacter::GiveEffect(FGameplayEffectSpecHandle& NewHandle) const
{
	AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), AbilitySystemComponent);
}

void AARPG_AKCCharacter::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	FTimerHandle TimerHandle;
	
	if(auto Item = Cast<ABaseItem>(OtherActor))
	{
		// Debug item name
		UE_LOG(LogTemp, Warning, TEXT("Item: %s"), *Item->GetName());

		// Waiting a frame for item to have enough time to generate
		auto DelayedFunction = [this, &TimerHandle, Item]()
		{
			MainGameHud->MainHUDWidget->AddItemNameWidget(Item);
			GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
		};
		
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, DelayedFunction, UE_SMALL_NUMBER, false);
	}
}

void AARPG_AKCCharacter::OnComponentExitOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(auto Item = Cast<ABaseItem>(OtherActor))
		MainGameHud->MainHUDWidget->RemoveItemNameWidget(Item);
}

void AARPG_AKCCharacter::CheckTileForSound()
{
	FHitResult HitResult;
	bool bIsHit = GetWorld()->LineTraceSingleByChannel(HitResult, GetActorLocation(), GetActorLocation() - FVector(0, 0, 1000), ECC_GameTraceChannel4);
	
	if(bIsHit)
	{
		if(ATile* Tile = Cast<ATile>(HitResult.GetActor()))
		{
			if(CurrentlyPlayingSound.Key == Tile->TileType || !TileSounds.Contains(Tile->TileType))
				return;
			
			AudioComponent->Stop();
			
			CurrentlyPlayingSound = { Tile->TileType, TileSounds[Tile->TileType] };

			AudioComponent->SetSound(CurrentlyPlayingSound.Value);
			AudioComponent->Play();
		}
	}
}

