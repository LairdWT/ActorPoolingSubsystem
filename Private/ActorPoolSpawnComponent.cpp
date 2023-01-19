// Laird Shingleton 2023

#include "ActorPoolSpawnComponent.h"

// Sets default values for this component's properties
UActorPoolSpawnComponent::UActorPoolSpawnComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UActorPoolSpawnComponent::BeginPlay()
{
	Super::BeginPlay();

	GetPoolSubsystemReference();
}

void UActorPoolSpawnComponent::GetPoolSubsystemReference()
{
	PoolSubsystem = this->GetWorld()->GetGameInstance()->GetSubsystem<UActorPoolSubsystem>();
}

AActor* UActorPoolSpawnComponent::SpawnActor(TSubclassOf<AActor> Class, const FTransform& Transform, const ESpawnActorCollisionHandlingMethod CollisionHandling, bool BroadcastSpawn)
{
	if (PoolSubsystem.IsValid())
	{
		// AActor* Owner = (this->GetOwner());
		TWeakObjectPtr<AActor> Owner(this->GetOwner());
		TObjectPtr<AActor> Actor = PoolSubsystem->SpawnActor_LowLevel(Class, Transform, CollisionHandling, Owner, BroadcastSpawn);
		return Actor.Get();
	}
	return nullptr;
}