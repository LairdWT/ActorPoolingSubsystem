// Laird Shingleton 2023

#include "ActorPoolInterface.h"
#include "ActorPoolSubsystem.h"

void IActorPoolInterface::OnCreate_LowLevel()
{
}

void IActorPoolInterface::OnDestroy_LowLevel()
{
}

void IActorPoolInterface::ReturnActorToPool()
{
	TObjectPtr<AActor> Actor = Cast<AActor>(this);
	if (IsValid(Actor))
	{
		Actor->GetWorld()->GetGameInstance()->GetSubsystem<UActorPoolSubsystem>()->ReturnActorToPool(Actor);
	}
}