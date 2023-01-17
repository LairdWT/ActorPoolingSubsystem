// Laird Shingleton 2023

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ActorPoolInterface.generated.h"

UINTERFACE(MinimalAPI)
class UActorPoolInterface : public UInterface
{
	GENERATED_BODY()
};

class ACTORPOOLINGSUBSYSTEM_API IActorPoolInterface
{
	GENERATED_BODY()

public:

	// Blueprint Events
	//
	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "OnCreate (Pool)"), Category = "Pool")
		void Pool_OnCreate();

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "OnDestroy (Pool)"), Category = "Pool")
		void Pool_OnDestroy();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Enable Optimisations (Pool)"), Category = "Pool")
		void Pool_EnableOptimisations();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Disable Optimisations (Pool)"), Category = "Pool")
		void Pool_DisableOptimisations();
	//
	// End Bleuprint Events
};