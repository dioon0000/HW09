// HW09TimerWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HW09TimerWidget.generated.h"

class UTextBlock;
class UProgressBar;

UCLASS()
class HW09_API UHW09TimerWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_Timer;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> ProgressBar_Timer;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_CurrentPlayer;

	void UpdateTimerDisplay();
};
