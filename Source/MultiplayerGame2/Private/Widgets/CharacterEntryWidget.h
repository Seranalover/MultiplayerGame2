// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Blueprint/UserWidget.h"
#include "CharacterEntryWidget.generated.h"

class UPA_CharacterDefination;
/**
 * 英雄选择界面
 */
UCLASS()
class UCharacterEntryWidget : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
	
public:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	FORCEINLINE const UPA_CharacterDefination* GetCharacterDefinition() const { return CharacterDefinition; }
	void SetSelected(bool bIsSelected);
	
private:
	UPROPERTY(meta=(BindWidget))
	class UImage* CharacterIcon;
	
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* CharacterNameText;
	
	UPROPERTY(EditDefaultsOnly, Category="Character")
	FName IconTextureMatPramName = "Icon";
	
	UPROPERTY(EditDefaultsOnly, Category="Character")
	FName SaturationMatPramName = "Saturation";
	
	UPROPERTY()
	const UPA_CharacterDefination* CharacterDefinition;
	
};
