// Laird Shingleton 2023

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "ActorPoolSubsystem.h"
#include "Engine/EngineTypes.h"

#include "ActorPoolSpawnComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent, DisplayName="Actor Spawning Pool"))
class ACTORPOOLINGSUBSYSTEM_API UActorPoolSpawnComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UActorPoolSpawnComponent();

private:
	// Pool Subsystem Ref
	UPROPERTY()
	TWeakObjectPtr<UActorPoolSubsystem> PoolSubsystem = { nullptr };

protected:
	virtual void BeginPlay() override;

	void inline GetPoolSubsystemReference();
public:		

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Spawn Actor from Pool"), Category = "Spawning Pool")
		virtual AActor* SpawnActor(TSubclassOf<AActor> Class, const FTransform& Transform, const ESpawnActorCollisionHandlingMethod CollisionHandling = ESpawnActorCollisionHandlingMethod::AlwaysSpawn, bool BroadcastSpawn = false);
};