// HW09PlayerState.cpp

#include "Player/HW09PlayerState.h"
#include "Net/UnrealNetwork.h"

AHW09PlayerState::AHW09PlayerState()
	: PlayerNameString(TEXT("None"))
	, CurrentGuessCount(0)
	, MaxGuessCount(3)
	, bIsMyTurn(false)
	, bHasPlayedThisTurn(false)
	, RemainingTurnTime(0.0f)
{
	bReplicates = true;
}

void AHW09PlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, PlayerNameString);
	DOREPLIFETIME(ThisClass, CurrentGuessCount);
	DOREPLIFETIME(ThisClass, MaxGuessCount);
	DOREPLIFETIME(ThisClass, bIsMyTurn);
	DOREPLIFETIME(ThisClass, bHasPlayedThisTurn);
	DOREPLIFETIME(ThisClass, RemainingTurnTime);
}

FString AHW09PlayerState::GetPlayerInfoString()
{
	FString PlayerInfoString = PlayerNameString + TEXT("(") + FString::FromInt(CurrentGuessCount) + TEXT("/") + FString::FromInt(MaxGuessCount) + TEXT(")");

	return PlayerInfoString;
}

