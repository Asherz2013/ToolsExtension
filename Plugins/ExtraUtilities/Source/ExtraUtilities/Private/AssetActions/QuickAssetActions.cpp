// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetActions/QuickAssetActions.h"
#include "DebugHeader.h"
#include "EditorUtilityLibrary.h"
#include "EditorAssetLibrary.h"

void UQuickAssetActions::DuplicateAssets(int32 NumOfDuplicates, bool bAutoSave)
{
    if (NumOfDuplicates <= 0)
    {
        ShowMsgDialog(EAppMsgType::Ok, TEXT("Please enter a valid positive number."));
        //Print(TEXT("Please enter a valid positive number."), FColor::Red);
        return;
    }

    // Grab all the Selected Assets Data
    TArray<FAssetData> SelectedAssetsData = UEditorUtilityLibrary::GetSelectedAssetData();

    uint32 Counter = 0;

    // Loop through all of our assets
    for (const FAssetData& SelectedAssetData : SelectedAssetsData)
    {
        // This will return something like:
        // /Game/MyFolder/BP_NewBlueprint.BP_NewBlueprint
        const FString SourceAssetPath = SelectedAssetData.GetObjectPathString();

        for (int32 i = 0; i < NumOfDuplicates; i++)
        {
            // New Asset name will be: 
            // BP_NewBlueprint_i
            // I will be the loop
            const FString NewDuplicatedAssetName = SelectedAssetData.AssetName.ToString() + TEXT("_") + FString::FromInt(i + 1);
            // Package path will return the following:
            // /Game/MyFolder/
            // COBINE will make sure all the "slashes" are added as expected
            const FString NewPathName = FPaths::Combine(SelectedAssetData.PackagePath.ToString(), NewDuplicatedAssetName);

            // If we have successfully duplicated the asset
            if (UEditorAssetLibrary::DuplicateAsset(SourceAssetPath, NewPathName))
            {
                // Do we want to autosave?
                if (bAutoSave)
                {
                    UEditorAssetLibrary::SaveAsset(NewPathName, false);
                }
                ++Counter;
            }
        }
    }

    if (Counter > 0)
    {
        ShowNotifyInfo(TEXT("Successfully duplicated " + FString::FromInt(Counter) + " file(s) out of " + FString::FromInt(NumOfDuplicates)));
        //Print(TEXT("Successfully duplicated " + FString::FromInt(Counter) + " files"), FColor::Green);
    }
}

void UQuickAssetActions::AddPrefixes()
{
    TArray<UObject*> SelectedObjects = UEditorUtilityLibrary::GetSelectedAssets();
    uint32 Counter = 0;

    for (UObject* SelectedObject : SelectedObjects)
    {
        if (!SelectedObject)
        {
            continue;
        }

        FString* PrefixFound = PrefixMap.Find(SelectedObject->GetClass());
        if (!PrefixFound || PrefixFound->IsEmpty())
        {
            Print(TEXT("Failed to find prefix for class") + SelectedObject->GetClass()->GetName(), FColor::Red);
            continue;
        }

        FString OldName = SelectedObject->GetName();

        if (OldName.StartsWith(*PrefixFound))
        {
            Print(OldName + TEXT(" already has prefix added"), FColor::Red);
            continue;
        }

        const FString NewNameWithPrefix = *PrefixFound + OldName;

        UEditorUtilityLibrary::RenameAsset(SelectedObject, NewNameWithPrefix);

        ++Counter;
    }

    if (Counter > 0)
    {
        ShowNotifyInfo(TEXT("Successfully renamed " + FString::FromInt(Counter) + " object(s) out of " + FString::FromInt(SelectedObjects.Num())));
        //Print(TEXT("Successfully duplicated " + FString::FromInt(Counter) + " files"), FColor::Green);
    }
}
