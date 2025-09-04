// HW09GameStateBase.cpp

#include "Game/HW09GameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Player/HW09PlayerController.h"

void AHW09GameStateBase::MulticastRPCBroadcastLoginMessage_Implementation(const FString& InNameString)
{
	if(HasAuthority()==false)
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if(IsValid(PC) == true)
		{
			AHW09PlayerController* HW09PC = Cast<AHW09PlayerController>(PC);
			if(IsValid(HW09PC) == true)
			{
				FString NotificationString = InNameString + TEXT(" has joined the game.");
				HW09PC->PrintChatMessageString(NotificationString);
			}
		}
	}
}
