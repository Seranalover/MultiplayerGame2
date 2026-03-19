// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TreeNodeInterface.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Components/ListView.h"
#include "Widgets/ItemWidget.h"
#include "ShopItemWidget.generated.h"

class UPA_ShopItem;
class UShopItemWidget;

//声明委托
DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemPurchaseIssued, const UPA_ShopItem*);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnShopItemSelected, const UShopItemWidget*);

/**
 * 商店物品控件类
 * 代表单个售卖的商店物品控件
 */
UCLASS()
class UShopItemWidget : public UItemWidget, public IUserObjectListEntry, public ITreeNodeInterface
{
	GENERATED_BODY()
	
public:
	FOnItemPurchaseIssued OnItemPurchaseIssued; //购买委托事件
	FOnShopItemSelected OnShopItemSelected; //选中物品委托事件
	
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	FORCEINLINE const UPA_ShopItem* GetShopItem() const { return ShopItem; }
	
	virtual UUserWidget* GetWidget() override; //获得节点控件
	virtual TArray<const ITreeNodeInterface*> GetInputs() const override; //连接到该节点的内容
	virtual TArray<const ITreeNodeInterface*> GetOutputs() const override; //连接到下一个节点的内容
	virtual const UObject* GetItemObject() const override; //当前节点的指针
	
private:
	UPROPERTY()
	const UPA_ShopItem* ShopItem;
	const class UListView* ParentListView; //父级列表视图
	
	void InitWithShopItem(const UPA_ShopItem* NewShopItem);
	virtual void RightButtonClicked() override; //点击右键，确认购买，广播委托
	virtual void LeftButtonClicked() override; //点击左键，选中物品，广播委托
	
	void CopyFromOther(const UShopItemWidget* OtherWidget); //拷贝对象内容
	TArray<const ITreeNodeInterface*> ItemsToInterfaces(const TArray<const UPA_ShopItem*>& Items) const; //接收物品列表并转换成接口形式
};
