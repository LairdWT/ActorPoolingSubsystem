// Laird Shingleton 2023

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/Subsystem.h"

#include <Engine/Engine.h>
#include "ActorPoolInterface.h"

#include "ActorPoolSubsystem.generated.h"

// Broadcast Spawn to listeners
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSpawnDelegate, AActor*, Actor, const FTransform&, Transform)

UCLASS()
class ACTORPOOLINGSUBSYSTEM_API UActorPoolSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:

	// Subsystem Init and DeInit
	//
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;
	//
	// End Init and DeInit

private:
	// Pool params
	//
	FActorSpawnParameters SpawnParameters;

	// OnSpawn and OnReturn Delegate
	UPROPERTY(BlueprintAssignable)
		FSpawnDelegate OnActorSpawnDelegate;

	UPROPERTY(BlueprintAssignable)
		FSpawnDelegate OnActorReturnToPoolDelegate;
	
	// Handle pool reset when level is changed
	void HandleLevelChanged(ULevel*, UWorld*);

	// Make type for Pool
	typedef TArray<TObjectPtr<AActor>> TPoolArray;
	TMap<TSubclassOf<AActor>, TPoolArray> Pool;
	//
	// End Pool params

public:
	// Actor Managment
	//
	// Functions marked "_LowLevel" are for c++ use, can skip some Blueprint required type conversions
	//
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Actor Inactive"), Category = "Pool")
		void SetActorStandby(AActor* Actor);

		void inline SetActorStandby_LowLevel(TObjectPtr<AActor> Actor);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Actor Active"), Category = "Pool")
		void SetActorActive(AActor* Actor, const FTransform& Transform, AActor* Owner);

		void inline SetActorActive_LowLevel(TObjectPtr<AActor> Actor, const FTransform& TransformIn, TWeakObjectPtr<AActor> Owner);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Spawn Actor from Pool"), Category = "Pool")
		AActor* SpawnActor(TSubclassOf<AActor> Class, const FTransform& Transform, const ESpawnActorCollisionHandlingMethod CollisionHandling = ESpawnActorCollisionHandlingMethod::AlwaysSpawn, AActor* Owner = nullptr, bool BroadcastSpawn = false);

		TObjectPtr<AActor> inline SpawnActor_LowLevel(TSubclassOf<AActor> Class, const FTransform& Transform, const ESpawnActorCollisionHandlingMethod CollisionHandling = ESpawnActorCollisionHandlingMethod::AlwaysSpawn, TWeakObjectPtr<AActor> Owner = nullptr, bool BroadcastSpawn = false);
private:
		void BroadcastActorSpawned(AActor* SpawnedActor, const FTransform& SpawnLocation);

		void BroadcastActorReturned(AActor* SpawnedActor, const FTransform& SpawnLocation);
public:
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Return Actor To Pool", DefaultToSelf = "Actor"), Category = "Pool")
		void ReturnActorToPool(AActor* Actor, bool BroadcastReturn = false);
	//
	// End Actor Management


	// Pool management
	//
	// Functions marked "_LowLevel" are for c++ use, can skip some Blueprint required type conversions
	//
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Reset Pool"), Category = "Pool")
		virtual void ResetPool();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Remove all Actors From Pool by Class"), Category = "Pool")
		virtual void RemoveAllActorsFromPoolByClass(TSubclassOf<AActor> Class);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Actor from Pool"), Category = "Pool")
		AActor* GetActorFromPool(TSubclassOf<AActor> Class);

		TObjectPtr<AActor> inline GetActorFromPool_LowLevel(TSubclassOf<AActor> Class);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Initialize Pool"), Category = "Pool")
		void InitPool(TSubclassOf<AActor> Class, int32 Amount);
	//
	// End Pool management
};