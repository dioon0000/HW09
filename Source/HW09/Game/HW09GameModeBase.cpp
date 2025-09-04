// HW09GameModeBase.cpp

#include "Game/HW09GameModeBase.h"
#include "Game/HW09GameStateBase.h"
#include "Player/HW09PlayerController.h"
#include "Player/HW09PlayerState.h"
#include "EngineUtils.h"

AHW09GameModeBase::AHW09GameModeBase()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void AHW09GameModeBase::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);

	AHW09PlayerController* HW09PlayerController = Cast<AHW09PlayerController>(NewPlayer);
	if(IsValid(HW09PlayerController) == true)
	{
		HW09PlayerController->NotificationText = FText::FromString(TEXT("Connected to the game server."));
		AllPlayerControllers.Add(HW09PlayerController);

		AHW09PlayerState* HW09PS = HW09PlayerController->GetPlayerState<AHW09PlayerState>();
		if(IsValid(HW09PS) == true)
		{
			HW09PS->PlayerNameString = TEXT("Player") + FString::FromInt(AllPlayerControllers.Num());
			HW09PS->bIsMyTurn = false;
			HW09PS->bHasPlayedThisTurn = false;
			HW09PS->RemainingTurnTime = 0.0f;
		}

		AHW09GameStateBase* HW09GameStateBase = GetGameState<AHW09GameStateBase>();
		if(IsValid(HW09GameStateBase) == true)
		{
			HW09GameStateBase->MulticastRPCBroadcastLoginMessage(HW09PS->PlayerNameString);
		}

		// 게임이 활성화되지 않았고 플레이어가 2명 이상일 때
		if(!bGameActive && AllPlayerControllers.Num() >=2)
		{
			bGameActive = true;
			StartNextTurn();
		}
	}
}

void AHW09GameModeBase::BeginPlay()
{
	Super::BeginPlay();

	SecretNumberString = GenerateSecretNumber(); 
	CurrentPlayerIndex = 0;
	TurnTimeLimit = 20.0f;
	CurrentTurnTime = TurnTimeLimit;
	bGameActive = false;
	UE_LOG(LogTemp, Error, TEXT("%s"), *SecretNumberString);
}

void AHW09GameModeBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(bGameActive && IsValid(CurrentTurnPlayer))
	{
		CurrentTurnTime -= DeltaTime;

		// 현재 플레이어의 남은 시간 업데이트
		AHW09PlayerState* HW09PS = CurrentTurnPlayer->GetPlayerState<AHW09PlayerState>();
		if(IsValid(HW09PS))
		{
			HW09PS->RemainingTurnTime = FMath::Max(0.0f, CurrentTurnTime);

		}

		if(CurrentTurnTime <= 0.0f)
		{
			HandleTurnTimeout();
		}
	}
}

FString AHW09GameModeBase::GenerateSecretNumber()
{
	TArray<int32> Numbers;
	for(int32 i=1; i<=9; ++i)
	{
		Numbers.Add(i);
	}

	FMath::RandInit(FDateTime::Now().GetTicks());
	Numbers = Numbers.FilterByPredicate([](int32 Num) {return Num > 0; });

	FString Result;
	for(int32 i=0; i<3; ++i)
	{
		int32 Index = FMath::RandRange(0, Numbers.Num()-1);
		Result.Append(FString::FromInt(Numbers[Index]));
		Numbers.RemoveAt(Index);
	}
	return Result;
}

bool AHW09GameModeBase::IsGuessNumberString(const FString& InNumberString)
{
	bool bCanPlay = false;

	do {
		if(InNumberString.Len() != 3)
		{
			break;
		}

		bool bIsUnique = true;
		TSet<TCHAR> UniqueDigits;

		for(TCHAR C : InNumberString)
		{
			if(FChar::IsDigit(C) == false || C == '0')
			{
				bIsUnique = false;
				break;
			}

			UniqueDigits.Add(C);
		}

		if(bIsUnique == false)
		{
			break;
		}

		bCanPlay = true;

	} while(false);
	return bCanPlay;
}

FString AHW09GameModeBase::JudgeResult(const FString& InSecretNumberString, const FString& InGuessNumberString)
{
	int32 StrikeCount = 0, BallCount = 0;

	for(int32 i=0; i<3; ++i)
	{
		if(InSecretNumberString[i] == InGuessNumberString[i])
		{
			StrikeCount++;
		}
		else
		{
			FString PlayerGuessChar = FString::Printf(TEXT("%c"), InGuessNumberString[i]);
			if(InSecretNumberString.Contains(PlayerGuessChar))
			{
				BallCount++;
			}
		}
	}

	if(StrikeCount == 0 && BallCount == 0)
	{
		return TEXT("OUT");
	}
	return FString::Printf(TEXT("%dS%dB"), StrikeCount, BallCount);
}

void AHW09GameModeBase::PrintChatMessageString(AHW09PlayerController* InChattingPlayerController, const FString& InChatMessageString)
{
	// 현재 턴이 아닌 플레이어는 게임플레이가 불가능 하도록 설정
	if(CurrentTurnPlayer != InChattingPlayerController)
	{
		InChattingPlayerController->ClientRPCPrintChatMessageString(TEXT("Wait Opponent Turn!"));
		return;
	}

	AHW09PlayerState* HW09PS = InChattingPlayerController->GetPlayerState<AHW09PlayerState>();
	if(IsValid(HW09PS) && HW09PS->CurrentGuessCount >= HW09PS->MaxGuessCount)
	{
		InChattingPlayerController->ClientRPCPrintChatMessageString(TEXT("It's Your Turn!"));
		return;
	}

	int Index = InChatMessageString.Len() - 3;
	FString GuessNumberString = InChatMessageString.RightChop(Index);

	if(IsGuessNumberString(GuessNumberString) == true)
	{
		// 이번 턴에 플레이했다는 사실을 기록
		if(IsValid(HW09PS))
		{
			HW09PS->bHasPlayedThisTurn = true;
		}

		FString JudgeResultString = JudgeResult(SecretNumberString, GuessNumberString);
		IncreaseGuessCount(InChattingPlayerController);

		for(TActorIterator<AHW09PlayerController> It(GetWorld()); It; ++It)
		{
			AHW09PlayerController* HW09PlayerController = *It;
			if(IsValid(HW09PlayerController) == true)
			{
				FString CombinedMessageString = HW09PS->GetPlayerInfoString() + TEXT(" -> ") + JudgeResultString;
				HW09PlayerController->ClientRPCPrintChatMessageString(CombinedMessageString);
			}
		}

		int32 StrikeCount = FCString::Atoi(*JudgeResultString.Left(1));
		JudgeGame(InChattingPlayerController, StrikeCount);
		// 게임이 끝나지 않았다면 다음 턴으로
		if(StrikeCount != 3)
		{
			EndCurrentTurn();
		}
	}
	else
	{
		for(TActorIterator<AHW09PlayerController> It(GetWorld()); It; ++It)
		{
			AHW09PlayerController* HW09PlayerController = *It;
			if(IsValid(HW09PlayerController) == true)
			{
				HW09PlayerController->ClientRPCPrintChatMessageString(InChatMessageString);
			}
		}
	}
}

void AHW09GameModeBase::IncreaseGuessCount(AHW09PlayerController* InChattingPlayerController)
{
	AHW09PlayerState* HW09PS = InChattingPlayerController->GetPlayerState<AHW09PlayerState>();
	if(IsValid(HW09PS) == true)
	{
		HW09PS->CurrentGuessCount++;
	}
}

void AHW09GameModeBase::ResetGame()
{
	SecretNumberString = GenerateSecretNumber();
	CurrentPlayerIndex = -1;
	bGameActive = true;

	for(const auto& HW09PlayerController : AllPlayerControllers)
	{
		AHW09PlayerState* HW09PS = HW09PlayerController->GetPlayerState<AHW09PlayerState>();
		if(IsValid(HW09PS) == true)
		{
			HW09PS->CurrentGuessCount = 0;
			HW09PS->bIsMyTurn = false;
			HW09PS->bHasPlayedThisTurn = false;
		}
	}

	StartNextTurn();
}

void AHW09GameModeBase::JudgeGame(AHW09PlayerController* InChattingPlayerController, int InStrikeCount)
{
	if(3 == InStrikeCount)
	{
		bGameActive = false;

		AHW09PlayerState* HW09PS = InChattingPlayerController->GetPlayerState<AHW09PlayerState>();
		for(const auto& HW09PlayerController : AllPlayerControllers)
		{
			if(IsValid(HW09PS) == true)
			{
				FString CombinedMessageString = HW09PS->PlayerNameString + TEXT(" has won the game.");
				HW09PlayerController->NotificationText = FText::FromString(CombinedMessageString);

				// 모든 플레이어의 턴 종료
				AHW09PlayerState* PlayerHWPS = HW09PlayerController->GetPlayerState<AHW09PlayerState>();
				if(IsValid(PlayerHWPS))
				{
					PlayerHWPS->bIsMyTurn = false;
				}
			}
		}

		// 게임이 끝나면 5초 뒤 재시작
		GetWorld()->GetTimerManager().SetTimer(TurnTimerHandle, this, &AHW09GameModeBase::ResetGame, 5.f, false);
	}
	else
	{
		bool bIsDraw = true;
		for(const auto& HW09PlayerController : AllPlayerControllers)
		{
			AHW09PlayerState* HW09PS = HW09PlayerController->GetPlayerState<AHW09PlayerState>();
			if(IsValid(HW09PS) == true)
			{
				if(HW09PS->CurrentGuessCount < HW09PS->MaxGuessCount)
				{
					bIsDraw = false;
					break;
				}
			}
		}

		if(true == bIsDraw)
		{
			bGameActive = false;

			for(const auto& HW09PlayerController : AllPlayerControllers)
			{
				HW09PlayerController->NotificationText = FText::FromString(TEXT("Draw..."));

				// 모든 플레이어의 턴 종료
				AHW09PlayerState* HW09PS = HW09PlayerController->GetPlayerState<AHW09PlayerState>();
				if(IsValid(HW09PS))
				{
					HW09PS->bIsMyTurn = false;
				}
			}
			// 게임이 끝나면 5초 뒤 재시작
			GetWorld()->GetTimerManager().SetTimer(TurnTimerHandle, this, &AHW09GameModeBase::ResetGame, 5.f, false);
		}
	}
}

void AHW09GameModeBase::StartNextTurn()
{
	if(AllPlayerControllers.Num() == 0) return;

	AHW09PlayerController* NextPlayer = nullptr;
	int32 StartIndex = CurrentPlayerIndex;

	for(int32 i = 0; i < AllPlayerControllers.Num(); ++i)
	{
		int32 TestIndex = (StartIndex + 1 + i) % AllPlayerControllers.Num();
		AHW09PlayerController* TestPlayer = AllPlayerControllers[TestIndex];

		if(IsValid(TestPlayer))
		{
			AHW09PlayerState* TestHWPS = TestPlayer->GetPlayerState<AHW09PlayerState>();
			if(IsValid(TestHWPS) && TestHWPS->CurrentGuessCount < TestHWPS->MaxGuessCount)
			{
				CurrentPlayerIndex = TestIndex;
				NextPlayer = TestPlayer;
				break;
			}
		}
	}

	if(!IsValid(NextPlayer))
	{
		bGameActive = false;
		for(const auto& HW09PlayerController : AllPlayerControllers)
		{
			HW09PlayerController->NotificationText = FText::FromString(TEXT("Draw..."));

			AHW09PlayerState* PlayerHWPS = HW09PlayerController->GetPlayerState<AHW09PlayerState>();
			if(IsValid(PlayerHWPS))
			{
				PlayerHWPS->bIsMyTurn = false;
			}
		}

		GetWorld()->GetTimerManager().SetTimer(TurnTimerHandle, this, &AHW09GameModeBase::ResetGame, 5.0f, false);
		return;
	}

	CurrentTurnPlayer = NextPlayer;
	CurrentTurnTime = TurnTimeLimit;

	// 모든 플레이어에게 턴 변경 알림
	for(const auto& HW09PlayerController : AllPlayerControllers)
	{
		if(IsValid(HW09PlayerController))
		{
			AHW09PlayerState* HW09PS = HW09PlayerController->GetPlayerState<AHW09PlayerState>();
			if(IsValid(HW09PS))
			{
				HW09PS->bIsMyTurn = (HW09PlayerController == CurrentTurnPlayer);
				HW09PS->bHasPlayedThisTurn = false;

				if(HW09PS->bIsMyTurn)
				{
					FString YourTurnMessage = FString::Printf(TEXT("Your turn! You have %.0f seconds."), TurnTimeLimit);
					HW09PlayerController->NotificationText = FText::FromString(YourTurnMessage);
				}
				else
				{
					FString TurnMessage = CurrentTurnPlayer->GetPlayerState<AHW09PlayerState>()->PlayerNameString + TEXT("'s turn - Wait your turn");
					HW09PlayerController->NotificationText = FText::FromString(TurnMessage);
				}
			}
		}
	}
}

void AHW09GameModeBase::EndCurrentTurn()
{
	StartNextTurn();
}

void AHW09GameModeBase::HandleTurnTimeout()
{
	if(IsValid(CurrentTurnPlayer))
	{
		AHW09PlayerState* HW09PS = CurrentTurnPlayer->GetPlayerState<AHW09PlayerState>();
		if(IsValid(HW09PS))
		{
			// 이번 턴에 플레이하지 않았다면 기회 소진
			if(!HW09PS->bHasPlayedThisTurn)
			{
				HW09PS->CurrentGuessCount++;

				// 모든 플레이어에게 타임아웃 알림
				for(const auto& NBPlayerController : AllPlayerControllers)
				{
					if(IsValid(NBPlayerController))
					{
						FString TimeoutMessage = HW09PS->PlayerNameString + TEXT(" timed out! Chance lost.");
						NBPlayerController->ClientRPCPrintChatMessageString(TimeoutMessage);
					}
				}
			}
		}
	}

	EndCurrentTurn();
}

void AHW09GameModeBase::setCurrentPlayer(AHW09PlayerController* NEWCurrentPlayer)
{
	CurrentTurnPlayer = NEWCurrentPlayer;
}
