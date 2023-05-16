#pragma once

#include "Misc/MessageDialog.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "Framework/Notifications/NotificationManager.h"

// Print to the screen
void Print(const FString& Message, const FColor& Colour)
{
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 8.f, Colour, Message);
    }
}

// Print out to the LOG
void PrintLog(const FString& Message)
{
    UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);
}

// Show a Dialog message
EAppReturnType::Type ShowMsgDialog(EAppMsgType::Type MsgType, const FString& Message, bool bShowMsgAsWarning = true)
{
    if (bShowMsgAsWarning)
    {
        FText MsgTitle = FText::FromString(TEXT("Warning"));
        return FMessageDialog::Open(MsgType, FText::FromString(Message), &MsgTitle);
    }

    return FMessageDialog::Open(MsgType, FText::FromString(Message));
}

// Add a notification to the editor popup system
void ShowNotifyInfo(const FString& Message)
{
    FNotificationInfo NotifyInfo(FText::FromString(Message));
    NotifyInfo.bUseLargeFont = true;
    NotifyInfo.FadeOutDuration = 7.f;

    FSlateNotificationManager::Get().AddNotification(NotifyInfo);
}