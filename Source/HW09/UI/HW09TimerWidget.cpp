// HW09TimerWidget.cpp
#include "UI/HW09TimerWidget.h"
#include "EngineUtils.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Player/HW09PlayerController.h"
#include "Player/HW09PlayerState.h"

void UHW09TimerWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);
    UpdateTimerDisplay();
}

void UHW09TimerWidget::UpdateTimerDisplay()
{
    APlayerController* PC = GetOwningPlayer();
    if(IsValid(PC))
    {
        UWorld* World = PC->GetWorld();
        if(IsValid(World))
        {
            // 현재 턴인 플레이어 찾기
            AHW09PlayerController* CurrentTurnPlayer = nullptr;
            float RemainingTime = 0.0f;

            for(TActorIterator<AHW09PlayerController> It(World); It; ++It)
            {
                AHW09PlayerController* HW09PlayerController = *It;
                if(IsValid(HW09PlayerController))
                {
                    AHW09PlayerState* HW09PS = HW09PlayerController->GetPlayerState<AHW09PlayerState>();
                    if(IsValid(HW09PS) && HW09PS->bIsMyTurn)
                    {
                        CurrentTurnPlayer = HW09PlayerController;
                        RemainingTime = HW09PS->RemainingTurnTime;
                        break;
                    }
                }
            }

            if(IsValid(CurrentTurnPlayer))
            {
                AHW09PlayerState* HW09PS = CurrentTurnPlayer->GetPlayerState<AHW09PlayerState>();
                if(IsValid(HW09PS))
                {
                    // 타이머 텍스트 업데이트
                    int32 Minutes = FMath::FloorToInt(RemainingTime / 60.0f);
                    int32 Seconds = FMath::FloorToInt(RemainingTime) % 60;
                    FString TimeString = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);

                    if(IsValid(TextBlock_Timer))
                    {
                        TextBlock_Timer->SetText(FText::FromString(TimeString));
                    }

                    // 프로그레스 바 업데이트
                    float Progress = RemainingTime / 20.0f;
                    if(IsValid(ProgressBar_Timer))
                    {
                        ProgressBar_Timer->SetPercent(Progress);
                    }

                    // 현재 플레이어 텍스트 업데이트
                    AHW09PlayerController* HW09PC = Cast<AHW09PlayerController>(PC);
                    if(IsValid(HW09PC))
                    {
                        AHW09PlayerState* MyPlayerState = HW09PC->GetPlayerState<AHW09PlayerState>();
                        if(IsValid(MyPlayerState))
                        {
                            FString CurrentPlayerText;
                            if(MyPlayerState->bIsMyTurn)
                            {
                                CurrentPlayerText = TEXT("Your Turn!");
                            }
                            else
                            {
                                CurrentPlayerText = HW09PS->PlayerNameString + TEXT("'s Turn - Wait your turn");
                            }

                            if(IsValid(TextBlock_CurrentPlayer))
                            {
                                TextBlock_CurrentPlayer->SetText(FText::FromString(CurrentPlayerText));
                            }
                        }
                    }
                }
            }
            else
            {
                if(IsValid(TextBlock_Timer))
                {
                    TextBlock_Timer->SetText(FText::FromString(TEXT("00:00")));
                }
                if(IsValid(ProgressBar_Timer))
                {
                    ProgressBar_Timer->SetPercent(0.0f);
                }
                if(IsValid(TextBlock_CurrentPlayer))
                {
                    TextBlock_CurrentPlayer->SetText(FText::FromString(TEXT("Waiting for players...")));
                }
            }
        }
    }
}
