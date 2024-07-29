// Fill out your copyright notice in the Description page of Project Settings.


#include "BTDecorator_IsFacing.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"

UBTDecorator_IsFacing::UBTDecorator_IsFacing()
{
	NodeName = "IsFacing";
}

bool UBTDecorator_IsFacing::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	if (const auto EnemyChar = OwnerComp.GetAIOwner()->GetCharacter())
	{
		if(const auto PlayerObject = OwnerComp.GetBlackboardComponent()
			->GetValueAsObject(BlackboardKey.SelectedKeyName))
		{
			const auto EnemyPos = EnemyChar->GetActorLocation();
			const auto PlayerPosition = Cast<AActor>(PlayerObject)->GetActorLocation();
			FVector ToPlayer = PlayerPosition - EnemyPos;
			ToPlayer.Normalize();

			float DotProduct = FVector::DotProduct(ToPlayer, EnemyChar->GetActorForwardVector());
			float Angle = FMath::Abs(FMath::RadiansToDegrees(FMath::Acos(DotProduct)));
			
			return Angle <= FieldOfView;
		}
	}
	return false;
}
