// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetActions/QuickAssetActions.h"
#include "DebugHeader.h"
#include "EditorUtilityLibrary.h"
#include "EditorAssetLibrary.h"


void UQuickAssetActions::DuplicateAssets(int32 NumOfDuplicates, bool AutoSave)
{
    if (NumOfDuplicates <= 0)
    {
        Print(TEXT("Please enter a valid positive number."), FColor::Red);
        return;
    }

    TArray<FAssetData> SelectedAssetsData = UEditorUtilityLibrary::GetSelectedAssetData();

    uint32 Counter = 0;

    for (const FAssetData& SelectedAssetData : SelectedAssetsData)
    {
        const FString SourceAssetPath = SelectedAssetData.GetObjectPathString();

        for (int32 i = 0; i < NumOfDuplicates; i++)
        {
            const FString NewDuplicatedAssetName = SelectedAssetData.AssetName.ToString() + TEXT("_") + FString::FromInt(i + 1);
            const FString NewPathName = FPaths::Combine(SelectedAssetData.PackagePath.ToString(), NewDuplicatedAssetName);

            if (UEditorAssetLibrary::DuplicateAsset(SourceAssetPath, NewPathName))
            {
                if (AutoSave)
                {
                    UEditorAssetLibrary::SaveAsset(NewPathName, false);
                }
                ++Counter;
            }
        }
    }

    if (Counter > 0)
    {
        Print(TEXT("Successfully duplicated " + FString::FromInt(Counter) + " files"), FColor::Green);
    }
}
