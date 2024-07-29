// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseEnemy.h"
#include "AbilitySystemComponent.h"
#include "BrainComponent.h"
#include "EnemyAIController.h"
#include "EnemyAttributeSet.h"
#include "LevelTileManager.h"
#include "ARPG_AKC/Enemy/EnemyDeathManager.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABaseEnemy::ABaseEnemy()
{
	PrimaryActorTick.bCanEverTick = false;
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>("AbilitySystemComponent");
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCapsuleComponent()->SetCollisionProfileName("EnemyPawn");
}

void ABaseEnemy::OnDead(ABaseEnemy* BaseEnemy)
{
	if(auto level = Cast<ALevelTileManager>(GetWorld()->GetLevelScriptActor()))
	{
		level->EnemyDeathManager->OnEnemyDied.Broadcast(BaseEnemy);
	}
}

bool ABaseEnemy::IsWithinAttackRange()
{
	if (Player)
	{
		FVector Direction = Player->GetActorLocation() - GetActorLocation();
		if (Direction.Length() < AttackRange + HitDistanceDeviation)
		{
			Direction.Normalize();
			float Angle = FMath::RadiansToDegrees(FMath::Atan2(Direction.Y, Direction.X)) - FMath::RadiansToDegrees(FMath::Atan2(GetActorForwardVector().Y, GetActorForwardVector().X));
			Angle = FMath::Abs(Angle);

			if (Angle <= HitAngle)
			{
				return true;
			}
		}
	}

	return false;
}

void ABaseEnemy::BeginPlay()
{
	GetCharacterMovement()->SetRVOAvoidanceWeight(0.5);
	GetCharacterMovement()->SetAvoidanceEnabled(true);
	OnPlayerDead.AddUniqueDynamic(this, &ABaseEnemy::HandleDeath);
	OnPlayerDead.AddUniqueDynamic(this, &ABaseEnemy::OnDead);
	OnPlayerHit.AddUniqueDynamic(this, &ABaseEnemy::OnHit);

	Player = Cast<AARPG_AKCCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	if (IsValid(AbilitySystemComponent))
	{
		Attributes = AbilitySystemComponent->GetSet<UEnemyAttributeSet>();

		for (int i = 0; i < StartAbilities.Num(); ++i)
		{
			UGameplayAbility* Ability = StartAbilities[i].GetDefaultObject();
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(Ability, 0, i));
		}
	}
	
	Super::BeginPlay();
}


FGameplayEffectSpecHandle ABaseEnemy::GetDamageSpec(float Damage, const FGameplayTagContainer TagsToAdd)
{
	if (!Player)
		return nullptr;

	auto DamageSpecHandle = AbilitySystemComponent->MakeOutgoingSpec(DamageEffect, 1, AbilitySystemComponent->MakeEffectContext());

	DamageSpecHandle.Data.Get()->SetSetByCallerMagnitude(DamageTag, -Damage);
	FGameplayTagContainer& CapturedSourceTags = DamageSpecHandle.Data.Get()->CapturedSourceTags.GetSpecTags();
	CapturedSourceTags.AppendTags(TagsToAdd);
	return DamageSpecHandle;
}

void ABaseEnemy::ApplyDamage(FGameplayEffectSpecHandle SpecHandle)
{
	if (SpecHandle.IsValid())
		AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data, Player->GetAbilitySystemComponent());
}

void ABaseEnemy::StopLogic()
{
	if (auto AIController = Cast<AEnemyAIController>(GetController()))
	{
		if(auto Brain = AIController->GetBrainComponent())
			Brain->StopLogic("");
		AIController->UnPossess();
		AIController->Destroy();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetMesh()->Stop();
	}
}

void ABaseEnemy::Ragdoll()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->Stop();
	GetMesh()->SetCollisionProfileName(FName("Ragdoll"));
	GetMesh()->SetSimulatePhysics(true);
}

void ABaseEnemy::OnHit()
{
	auto AIController = Cast<AEnemyAIController>(GetController());
	
	if(AIController)
	{
		AIController->GetBlackboardComponent()->SetValueAsBool("IsHit", true);
	}
}

UBehaviorTree* ABaseEnemy::GetBehaviourTree() const
{
	return BehaviorTree;
}
