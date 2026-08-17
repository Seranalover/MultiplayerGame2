// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/IHttpRequest.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "CGameInstance.generated.h"

DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnLoginCompleted, bool /*bWasSuccessful*/, const FString& /*PlayerNickName*/, const FString& /*ErrorMsg*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnGlobalSessionSearchCompleted, const TArray<FOnlineSessionSearchResult>& /*SearchResults*/);
DECLARE_MULTICAST_DELEGATE(FOnJoinSessionFailed);
/**
 * 游戏实例类
 * 在游戏运行期间持续存在，伴随整个游戏进程
 * 可用于在其基础上构建网络功能，涉及到大厅的创建和epic在线服务的连接
 * 可用于实现地图加载功能
 */
UCLASS()
class UCGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	void StartMatch(); //点击 开始游戏 时加载游戏关卡
	virtual void Init() override; //游戏启动时调用
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Map")
	TSoftObjectPtr<UWorld> MainMenuLevel; //使用软引用，任何硬引用都会将资源整个加载到内存中
	
	UPROPERTY(EditDefaultsOnly, Category = "Map")
	TSoftObjectPtr<UWorld> LobbyLevel;
	
	UPROPERTY(EditDefaultsOnly, Category = "Map")
	TSoftObjectPtr<UWorld> GameLevel;
	
	void LoadLevelAndListen(TSoftObjectPtr<UWorld> Level);
	
	/*****************************************************/
	/*        Client Session Creation and Search         */
	/*****************************************************/
public:
	void RequestCreateAndJoinSession(const FName& NewSessionName); //创建并加入会话
	void CancelSessionCreation(); //取消创建会话
	void StartGlobalSessionSearch(); //开始搜索全部会话
	bool JoinSessionWithId(const FString& SessionId); //根据session id加入会话
	
	FOnJoinSessionFailed OnJoinSessionFailed; //加入session失败委托事件
	FOnGlobalSessionSearchCompleted OnGlobalSessionSearchCompleted; //查找Global session完成委托事件
	
private:
	void SessionCreationRequestCompleted(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, FGuid SessionSearchId);
	
	FTimerHandle FindCreatedSessionHandle;
	FTimerHandle FindCreatedSessionTimeoutHandle;
	FTimerHandle GlobalSessionSearchHandle;
	UPROPERTY(EditDefaultsOnly, Category = "Session Search")
	float GlobalSessionSearchInterval = 2.f;
	UPROPERTY(EditDefaultsOnly, Category = "Session Search")
	float FindCreatedSessionSearchInterval = 1.f;
	UPROPERTY(EditDefaultsOnly, Category = "Session Search")
	float FindCreatedSessionTimeoutDuration = 60.f;
	TSharedPtr<class FOnlineSessionSearch> SessionSearchPtr; //搜索到的session容器
	
	void StartFindingCreatedSession(const FGuid& SessionSearchId); //开始通过id查询session
	void StopAllSessionFindings(); //停止所有session查找
	void StopFindingCreatedSession(); //停止查找创建的session
	void StopGlobalSessionSearch(); //停止查找所有session
	void FindCreatedSession(FGuid SessionSearchId); //搜索会话
	void FindCreatedSessionTimeout(); //搜索会话超时
	void FindCreatedSessionCompleted(bool bWasSuccessful); //将搜索会话结果加入结果集
	void JoinSessionWithSearchResult(const class FOnlineSessionSearchResult& SearchResult); //根据搜索结果加入会话
	void JoinSessionCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type JoinResult, int Port); //最终加入会话
	void FindGlobalSessions(); //查找全部session
	void GlobalSessionSearchCompleted(bool bWasSuccessful); //global session搜索完成，保存结果集
	
	/*****************************************************/
	/*                  Session Server                   */
	/*****************************************************/
public:
	void PlayerJoined(const FUniqueNetIdRepl& UniqueId); //当有玩家加入会话
	void PlayerLeft(const FUniqueNetIdRepl& UniqueId); //当有玩家离开会话
	
private:
	FString ServerSessionName;
	int ServerSessionPort;
	FTimerHandle WaitPlayerJoinTimeoutHandle; //等待玩家计时器，没有玩家加入时关闭会话
	UPROPERTY(EditDefaultsOnly, Category = "Session")
	float WaitPlayerJoinTimeoutDuration = 60.f;
	TSet<FUniqueNetIdRepl> PlayerRecord; //当前登陆的玩家集合
	
	void CreateSession();
	void OnSessionCreated(FName SessionName, bool bWasSuccessful);
	void TerminateSessionServer(); //当无法创建会话时，关闭会话避免占用资源
	void EndSessionCompleted(FName SessionName, bool bWasSuccessful); //关闭会话
	void WaitPlayerJoinTimeoutReached(); //计时器任务 - 等待结束
	
	/*****************************************************/
	/*                       Login                       */
	/*****************************************************/
public:
	FOnLoginCompleted OnLoginCompleted; //登录完成委托事件
	bool IsLoggedIn(); //已登录？
	bool IsLoggingIn(); //登陆中？
	void ClientAccountPortalLogin();
	
private:
	FDelegateHandle LoggingDelegateHandle; //登录句柄
	void ClientLogin(const FString& Type, const FString& Id, const FString& Token);
	void LoginCompleted(int NumOfLocalPlayer, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error);
};
