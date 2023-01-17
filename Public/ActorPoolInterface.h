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

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Enable Optimizations (Pool)"), Category = "Pool")
		void Pool_EnableOptimizations();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Disable Optimizations (Pool)"), Category = "Pool")
		void Pool_DisableOptimizations();
	//
	// End Bleuprint Events
};