// Laird Shingleton 2023

#include "ActorPoolSubsystem.h"
#include "Engine/World.h"

void UActorPoolSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// Set FActorSpawnParameters
	SpawnParameters.Owner = nullptr;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParameters.bNoFail = true;

	// Bind to level change delegate to make sure pool is cleared between levels
	UWorld* World = GetWorld();
	if (IsValid(World))
	{
		FWorldDelegates::LevelRemovedFromWorld.AddUObject(this, &UActorPoolSubsystem::HandleLevelChanged);
	}

}

void UActorPoolSubsystem::Deinitialize()
{
	// Cleanup Pool before DeInit
	ResetPool();

	// Unbind from UWorld delegate
	FWorldDelegates::LevelRemovedFromWorld.RemoveAll(this);

	Super::Deinitialize();
}

void UActorPoolSubsystem::InitPool(TSubclassOf<AActor> Class, int32 Amount)
{
	// set to default state
	ResetPool();

	// Reserve the memory
	TPoolArray& ClassArray = Pool.FindOrAdd(Class);
	ClassArray.Reserve(Amount);

	// Spawn actor(s) and push them to their ClassArray
	for (int32 Index = 0; Index < Amount; Index++)
	{
		TObjectPtr<AActor> Actor = this->GetWorld()->SpawnActor<AActor>(Class, FTransform::Identity, SpawnParameters);
		if ((Actor))
		{
			SetActorStandby_LowLevel(Actor);
			ClassArray.Push(Actor);
		}
	}
}

void UActorPoolSubsystem::HandleLevelChanged(ULevel*, UWorld*)
{
	UE_LOG(LogTemp, Warning, TEXT("Actor Pool Reset"));
	ResetPool();
}

void UActorPoolSubsystem::ResetPool()
{
	// Reset to default state
	Pool.Reset();
}

void UActorPoolSubsystem::AddActorsByClass(TSubclassOf<AActor> Class, int32 Amount)
{
	// Reserve the memory
	TPoolArray& ClassArray = Pool.FindOrAdd(Class);
	ClassArray.SetNum(ClassArray.Num() + Amount);

	// Spawn actor(s) and push them to their ClassArray
	for (int32 Index = 0; Index < Amount; Index++)
	{
		TObjectPtr<AActor> Actor = this->GetWorld()->SpawnActor<AActor>(Class, FTransform::Identity, SpawnParameters);
		if ((Actor))
		{
			SetActorStandby_LowLevel(Actor);
			ClassArray.Push(Actor);
		}
	}
}

void UActorPoolSubsystem::RemoveAllActorsFromPoolByClass(TSubclassOf<AActor> Class)
{
	// Find the array of type Class, then empty it
	TObjectPtr<TPoolArray> ClassArray = Pool.Find(Class);
	if (ClassArray)
	{
		ClassArray->Empty();
	}
}

AActor* UActorPoolSubsystem::GetActorFromPool(TSubclassOf<AActor> Class)
{
	// TObjectPtr to Raw pointer
	return GetActorFromPool_LowLevel(Class).Get();
}

TObjectPtr<AActor> UActorPoolSubsystem::GetActorFromPool_LowLevel(TSubclassOf<AActor> Class)
{
	// Find the array of type Class
	TObjectPtr<TPoolArray> ClassArray = Pool.Find(Class);

	// If array has contents, pop the top queued actor
	if (ClassArray && ClassArray->Num() > 0)
	{
		return ClassArray->Pop(false);
	}

	// nullptr return if empty
	TObjectPtr<AActor> Actor = { nullptr };
	return Actor;
}

AActor* UActorPoolSubsystem::SpawnActor(TSubclassOf<AActor> Class, const FTransform& Transform, const ESpawnActorCollisionHandlingMethod CollisionHandling, AActor* Owner, bool BroadcastSpawn)
{
	TWeakObjectPtr<AActor> OwnerPtr(Owner);
	return SpawnActor_LowLevel(Class, Transform, CollisionHandling, OwnerPtr, BroadcastSpawn).Get();
}

TObjectPtr<AActor> UActorPoolSubsystem::SpawnActor_LowLevel(TSubclassOf<AActor> Class, const FTransform& Transform, const ESpawnActorCollisionHandlingMethod CollisionHandling, TWeakObjectPtr<AActor> Owner, bool BroadcastSpawn)
{
	// get actor of type Class
	TObjectPtr<AActor> Actor = { nullptr };
	Actor = GetActorFromPool_LowLevel(Class);

	// make sure it's not a nullptr
	if (IsValid(Actor))
	{
		// Set Actor Active
		SetActorActive_LowLevel(Actor, Transform, Owner);

		// Try to fire OnCreate event from interface
		if (Actor->GetClass()->ImplementsInterface(UActorPoolInterface::StaticClass()))
		{
			IActorPoolInterface::Execute_Pool_OnCreate(Actor);
		}
	}

	else
	{
		// Spawn a new actor
		Actor = this->GetWorld()->SpawnActor<AActor>(Class, Transform, SpawnParameters);

		// Try to fire OnCreate event from interface
		if (IsValid(Actor) && Actor->GetClass()->ImplementsInterface(UActorPoolInterface::StaticClass()))
		{
			IActorPoolInterface::Execute_Pool_OnCreate(Actor);
		}
	}

	if (BroadcastSpawn)
	{
		BroadcastActorSpawned(Actor.Get(), Transform);
	}

	return Actor;
}

void UActorPoolSubsystem::BroadcastActorSpawned(AActor* SpawnedActor, const FTransform& SpawnLocation)
{
	OnActorSpawnDelegate.Broadcast(SpawnedActor, SpawnLocation);
}

void UActorPoolSubsystem::BroadcastActorReturned(AActor* SpawnedActor, const FTransform& SpawnLocation)
{
	OnActorReturnToPoolDelegate.Broadcast(SpawnedActor, SpawnLocation);
}

void UActorPoolSubsystem::ReturnActorToPool(AActor* Actor, bool BroadcastReturn)
{
	if (IsValid(Actor))
	{
		TObjectPtr<AActor> ActorIn(Actor);

		if (BroadcastReturn)
		{
			BroadcastActorReturned(ActorIn.Get(), ActorIn->GetTransform());
		}

		// Try to fire OnDestroy event from interface
		if (ActorIn->GetClass()->ImplementsInterface(UActorPoolInterface::StaticClass()))
		{
			IActorPoolInterface::Execute_Pool_OnDestroy(ActorIn);
		}

		// Set incative - find or create ClassArray - then push actor onto the ClassArray
		SetActorStandby_LowLevel(ActorIn);
		TPoolArray& ClassArray = Pool.FindOrAdd(ActorIn->GetClass());
		ClassArray.Push(ActorIn);
	}
}

void UActorPoolSubsystem::SetActorStandby(AActor* Actor)
{
	if (IsValid(Actor))
	{
		TObjectPtr<AActor> ActorPtr(Actor);
		SetActorStandby_LowLevel(ActorPtr);
	}
}

void UActorPoolSubsystem::SetActorStandby_LowLevel(TObjectPtr<AActor> Actor)
{
	// turn off tick, disable collision, turn off visibility, reset transform, clear owner
	Actor->SetActorTickEnabled(false);
	Actor->SetActorEnableCollision(false);
	Actor->SetActorHiddenInGame(true);
	Actor->SetActorTransform(FTransform::Identity);
	Actor->SetOwner(nullptr);

	// Try to fire Enable Optimizations event from interface
	if (Actor->GetClass()->ImplementsInterface(UActorPoolInterface::StaticClass()))
	{
		IActorPoolInterface::Execute_Pool_EnableOptimizations(Actor);
	}
}

void UActorPoolSubsystem::SetActorActive(AActor* Actor, const FTransform& Transform, AActor* Owner)
{
	if (IsValid(Actor))
	{
		TObjectPtr<AActor> ActorPtr(Actor);
		TObjectPtr<AActor> OwnerPtr(Owner);
		SetActorActive_LowLevel(ActorPtr, Transform, OwnerPtr);
	}
}

void UActorPoolSubsystem::SetActorActive_LowLevel(TObjectPtr<AActor> Actor, const FTransform& TransformIn, TWeakObjectPtr<AActor> Owner)
{
	// Enable tick if set by default
	Actor->SetActorTickEnabled(Actor->PrimaryActorTick.bStartWithTickEnabled);

	// Move to spawn location, make visible, re-enable collision, set new owner
	Actor->SetActorTransform(TransformIn);
	Actor->SetActorHiddenInGame(false);
	Actor->SetActorEnableCollision(true);
	Actor->SetOwner(Owner.Get());

	// Try to fire Enable Optimizations event from interface
	if (Actor->GetClass()->ImplementsInterface(UActorPoolInterface::StaticClass()))
	{
		IActorPoolInterface::Execute_Pool_DisableOptimizations(Actor);
	}
}