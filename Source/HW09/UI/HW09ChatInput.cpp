// HW09ChatInput.cpp

#include "UI/HW09ChatInput.h"
#include "Components/EditableTextBox.h"
#include "Player/HW09PlayerController.h"

void UHW09ChatInput::NativeConstruct()
{
	Super::NativeConstruct();

	if(EditableTextBox_ChatInput->OnTextCommitted.IsAlreadyBound(this, &ThisClass::OnChatInputTextCommitted) == false)
	{
		EditableTextBox_ChatInput->OnTextCommitted.AddDynamic(this, &ThisClass::OnChatInputTextCommitted);
	}
}

void UHW09ChatInput::NativeDestruct()
{
	Super::NativeDestruct();

	if(EditableTextBox_ChatInput->OnTextCommitted.IsAlreadyBound(this, &ThisClass::OnChatInputTextCommitted) == true)
	{
		EditableTextBox_ChatInput->OnTextCommitted.RemoveDynamic(this, &ThisClass::OnChatInputTextCommitted);
	}
}

void UHW09ChatInput::OnChatInputTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if(CommitMethod == ETextCommit::OnEnter)
	{
		APlayerController* OwingPlayerController = GetOwningPlayer();
		if(IsValid(OwingPlayerController) == true)
		{
			AHW09PlayerController* OwingHW09PlayerController = Cast<AHW09PlayerController>(OwingPlayerController);
			if(IsValid(OwingHW09PlayerController) == true)
			{
				OwingHW09PlayerController->SetChatMessageString(Text.ToString());

				EditableTextBox_ChatInput->SetText(FText());
			}
		}
	}
}
