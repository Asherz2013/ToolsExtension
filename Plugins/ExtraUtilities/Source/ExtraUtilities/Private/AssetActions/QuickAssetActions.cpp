// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetActions/QuickAssetActions.h"
#include "DebugHeader.h"
#include "EditorUtilityLibrary.h"
#include "EditorAssetLibrary.h"
#include "ObjectTools.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"

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

        // Check to see if the Prefix is in the TMap
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

        // If we are looking at a Material Instance
        // If it was created by creating a Child, then it will have "_Inst" on the end
        // Also double check if it was created from a Material already
        if (SelectedObject->IsA<UMaterialInstanceConstant>())
        {
            OldName.RemoveFromStart(TEXT("M_"));
            OldName.RemoveFromEnd(TEXT("_Inst"));
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

void UQuickAssetActions::DeleteUnusedAssets()
{
    // Grab all the Selected Assets Data
    TArray<FAssetData> SelectedAssetsData = UEditorUtilityLibrary::GetSelectedAssetData();
    TArray<FAssetData> UnusedAssetsData;

    FixupRedirectors();

    for (const FAssetData& SelectedAssetData : SelectedAssetsData)
    {
        TArray<FString> NumAssetReferences = UEditorAssetLibrary::FindPackageReferencersForAsset(SelectedAssetData.GetObjectPathString());
        if (NumAssetReferences.Num() == 0)
        {
            UnusedAssetsData.Add(SelectedAssetData);
        }
    }

    if (UnusedAssetsData.Num() == 0)
    {
        ShowMsgDialog(EAppMsgType::Ok, TEXT("No unused assets found amoung selected assets"), true);
        return;
    }

    // Object Tools comes from "UnrealEd module
    // Brings up a Dialogue box to ask the user if they wish to delete the assets rather than forcing them to be deleted
    const int32 NumOfAssetsDeleted = ObjectTools::DeleteAssets(UnusedAssetsData);

    if (NumOfAssetsDeleted == 0) return;

    ShowNotifyInfo(TEXT("Successfully deleted " + FString::FromInt(NumOfAssetsDeleted) + TEXT(" unused Assets.")));
}

void UQuickAssetActions::FixupRedirectors()
{
    TArray<UObjectRedirector*> RedirectorsToFixArray;

    // Loads up a Module and checks that it is valid
    // In this case it will load up the Asset Registry Module
    FAssetRegistryModule& AssetRegistryModule = FModuleManager::Get().LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));

    // Make a Filter todo the following:
    // Look through all folders
    // Check for only items that have NOT been redirected properly
    FARFilter filter;
    filter.bRecursivePaths = true;
    filter.PackagePaths.Emplace("/Game");
    filter.ClassNames.Emplace("ObjectRedirector");

    TArray<FAssetData> OutRedirectors;

    AssetRegistryModule.Get().GetAssets(filter, OutRedirectors);

    for (const FAssetData& RedirectorData : OutRedirectors)
    {
        if(UObjectRedirector* RedirectorToFix = Cast<UObjectRedirector>(RedirectorData.GetAsset()))
        {
            RedirectorsToFixArray.Add(RedirectorToFix);
        }
    }

    // Loads up the Asset Tools Module
    // This is used to look at fixing References
    FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools"));

    AssetToolsModule.Get().FixupReferencers(RedirectorsToFixArray);
}
