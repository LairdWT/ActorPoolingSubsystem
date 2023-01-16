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

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "OnDestroy (Pool)"), Category = "Pool")
		void Pool_OnCreate();

	UFUNCTION(BlueprintNativeEvent, meta = (DisplayName = "OnCreate (Pool)"), Category = "Pool")
		void Pool_OnDestroy();

	virtual void OnCreate_LowLevel();

	virtual void OnDestroy_LowLevel();

	virtual void ReturnActorToPool();
};
