// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SessionEntryWidget.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnSessionEntrySelected, const FString& /*SelectedSessionId*/)
/**
 * 会话选择控件类
 */
UCLASS()
class USessionEntryWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	FOnSessionEntrySelected OnSessionEntrySelected; //session已选择委托事件
	
	virtual void NativeConstruct() override;
	void InitializeEntry(const FString& Name, const FString& SessionIdStr);
	FORCEINLINE FString GetCachedSessionId() const { return CachedSessionIdStr; }
	
private:
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* SessionNameText;
	
	UPROPERTY(meta=(BindWidget))
	class UButton* SessionBtn;
	
	FString CachedSessionIdStr;
	
	UFUNCTION()
	void SessionEntrySelected();
};
