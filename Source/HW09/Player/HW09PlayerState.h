// HW09PlayerState.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "HW09PlayerState.generated.h"

UCLASS()
class HW09_API AHW09PlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	AHW09PlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	FString GetPlayerInfoString();

public:
	UPROPERTY(Replicated)
	FString PlayerNameString;

	UPROPERTY(Replicated)
	int32 CurrentGuessCount;

	UPROPERTY(Replicated)
	int32 MaxGuessCount;

	// 턴 관리 변수
	UPROPERTY(Replicated, BlueprintReadOnly)
	bool bIsMyTurn;

	UPROPERTY(Replicated, BlueprintReadOnly)
	bool bHasPlayedThisTurn;

	UPROPERTY(Replicated, BlueprintReadOnly)
	float RemainingTurnTime;
};
