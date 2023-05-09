#pragma once

void Print(const FString& Message, const FColor& Colour)
{
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 8.f, Colour, Message);
    }
}

void PrintLog(const FString& Message)
{
    UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);
}