// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerTeamSlotWidget.generated.h"

class UPA_CharacterDefination;
/**
 * 大厅顶部 玩家选择英雄展示
 */
UCLASS()
class UPlayerTeamSlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	void UpdateSlot(const FString& PlayerName, const UPA_CharacterDefination* CharacterDefination);
	
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	
private:
	UPROPERTY(Transient, meta=(BindWidget))
	class UWidgetAnimation* HoverAnim; //悬停时的角色动画
	
	UPROPERTY(meta=(BindWidget))
	class UImage* PlayerCharacterIcon;
	
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* NameText;
	
	UPROPERTY(EditDefaultsOnly, Category="Visual")
	FName CharacterIconMatParamName = "Icon"; //英雄选择后的头像材质
	
	UPROPERTY(EditDefaultsOnly, Category="Visual")
	FName CharacterEmptyMatParamName = "Empty"; //英雄未选择时的头像材质，置灰
	
	FString CachedPlayerNameStr;
	FString CachedCharacterNameStr;
	
	void UpdateNameText();
};
