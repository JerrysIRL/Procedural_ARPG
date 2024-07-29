#include "EnemyActor.h"
#include "LevelTileManager.h"

AEnemyActor::AEnemyActor()
{
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	SetRootComponent(MeshComponent);
	
}

void AEnemyActor::BeginPlay()
{
	Super::BeginPlay();

	LevelScriptActor = Cast<ALevelTileManager>(GetWorld()->GetLevelScriptActor());
}


void AEnemyActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if(!bGenerate) return;

	if(!IsValid(LevelScriptActor))
	{
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, "Level Script Actor not valid. Probably in wrong level.");
		bGenerate = false;
		return;
	}
	
	FVector2D EnemyLocation = FVector2D(GetActorLocation().X, GetActorLocation().Y);
	
	Destroy();
	
}