// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Player/PlayerInfoTypes.h"
#include "LobbyWidget.generated.h"

/**
 * 游戏大厅界面类
 */
UCLASS()
class ULobbyWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	
private:
	UPROPERTY(meta=(BindWidget))
	class UWidgetSwitcher* MainSwitcher;
	
	UPROPERTY(meta=(BindWidget))
	class UWidget* TeamSelectionRoot;
	
	UPROPERTY(meta=(BindWidget))
	class UButton* StartHeroSelectionButton;
	
	UPROPERTY(meta=(BindWidget))
	class UUniformGridPanel* TeamSelectionSlotGridPanel;
	
	UPROPERTY(EditDefaultsOnly, Category="TeamSelection")
	TSubclassOf<class UTeamSelectionWidget> TeamSelectionWidgetClass;
	
	UPROPERTY()
	TArray<class UTeamSelectionWidget*> TeamSelectionSlots;
	
	void ClearAndPopulateTeamSelectionSlots();
	void SlotSelected(uint8 NewSlotId);
	
	UPROPERTY(meta=(BindWidget))
	class UWidget* HeroSelectionRoot;
	
	UPROPERTY(meta=(BindWidget))
	class UTileView* CharacterSelectionTileView;
	
	UPROPERTY(meta=(BindWidget))
	class UAbilitiesListView* AbilityListView;
	
	UPROPERTY()
	class ALobbyPlayerController* LobbyPlayerController;
	
	UPROPERTY()
	class ACGameState* CGameState;
	
	UPROPERTY()
	class ACPlayerState* CPlayerState;
	
	FTimerHandle ConfigureGameStateTimer;
	
	void ConfigureGameState();
	void UpdatePlayerSelectionDisplay(const TArray<FPlayerSelection>& PlayerSelectionArray);
	
	UFUNCTION()
	void StartHeroSelectionButtonClicked();
	
	void SwitchToHeroSelection();
	void CharacterDefinitionLoaded();
	
	void CharacterSelected(UObject* SelectedUObject);
	
	UPROPERTY(EditDefaultsOnly, Category="Character Display")
	TSubclassOf<class ACharacterDisplay> CharacterDisplayClass;
	
	UPROPERTY()
	class ACharacterDisplay* CharacterDisplay;
	
	void SpawnCharacterDisplay();
	void UpdateCharacterDisplay(const FPlayerSelection& PlayerSelection); //切换角色模型时调用
};
