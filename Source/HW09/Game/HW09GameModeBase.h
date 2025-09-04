// HW09GameModeBase.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "HW09GameModeBase.generated.h"

class AHW09PlayerController;

UCLASS()
class HW09_API AHW09GameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AHW09GameModeBase();

	virtual void OnPostLogin(AController* NewPlayer) override;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	FString GenerateSecretNumber();

	bool IsGuessNumberString(const FString& InNumberString);

	FString JudgeResult(const FString& InSecretNumberString, const FString& InGuessNumberString);

	void PrintChatMessageString(AHW09PlayerController* InChattingPlayerController, const FString& InChatMessageString);

	void IncreaseGuessCount(AHW09PlayerController* InChattingPlayerController);

	void ResetGame();

	void JudgeGame(AHW09PlayerController* InChattingPlayerController, int InStrikeCount);

	// 턴 관리 로직 함수
	void StartNextTurn();

	void EndCurrentTurn();

	void HandleTurnTimeout();

	void setCurrentPlayer(AHW09PlayerController* NEWCurrentPlayer);

protected:
	FString SecretNumberString;

	TArray<TObjectPtr<AHW09PlayerController>> AllPlayerControllers;

	// 턴 관리 로직 변수
	UPROPERTY()
	TObjectPtr<AHW09PlayerController> CurrentTurnPlayer;

	UPROPERTY()
	int32 CurrentPlayerIndex;

	UPROPERTY()
	float TurnTimeLimit;

	UPROPERTY()
	float CurrentTurnTime;

	UPROPERTY()
	bool bGameActive;

	FTimerHandle TurnTimerHandle;
};
