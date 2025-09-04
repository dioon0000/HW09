// HW09ChatInput.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HW09ChatInput.generated.h"

class UEditableTextBox;

UCLASS()
class HW09_API UHW09ChatInput : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;
	
protected:
	UFUNCTION()
	void OnChatInputTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);
public:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UEditableTextBox> EditableTextBox_ChatInput;
};
