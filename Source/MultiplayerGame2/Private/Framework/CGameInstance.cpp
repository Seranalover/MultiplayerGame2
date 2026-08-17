// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/CGameInstance.h"

#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Network/CNetStatics.h"

void UCGameInstance::StartMatch()
{
	if (GetWorld()->GetNetMode() == ENetMode::NM_DedicatedServer || GetWorld()->GetNetMode() == ENetMode::NM_ListenServer)
		LoadLevelAndListen(GameLevel);
}

void UCGameInstance::Init()
{
	Super::Init();
	if (GetWorld()->IsEditorWorld()) return;
	if (UCNetStatics::IsSessionServer(this))
	{
		CreateSession();	
	}
}

void UCGameInstance::CreateSession()
{
	IOnlineSessionPtr SessionPtr = UCNetStatics::GetSessionPtr();
	if (SessionPtr)
	{
		ServerSessionName = UCNetStatics::GetSessionNameStr();
		FString SessionSearchId = UCNetStatics::GetSessionSearchIdStr();
		ServerSessionPort = UCNetStatics::GetSessionPort();
		UE_LOG(LogTemp, Warning, TEXT("### Create Session with Name: %s, ID: %s, Port: %d"), *ServerSessionName, *SessionSearchId, ServerSessionPort);
	
		FOnlineSessionSettings OnlineSessionSettings = UCNetStatics::GenerateOnlineSessionSettings(FName(ServerSessionName), SessionSearchId, ServerSessionPort);
		SessionPtr->OnCreateSessionCompleteDelegates.RemoveAll(this);
		SessionPtr->OnCreateSessionCompleteDelegates.AddUObject(this, &UCGameInstance::OnSessionCreated);
		if (!SessionPtr->CreateSession(0, FName(ServerSessionName), OnlineSessionSettings))
		{
			UE_LOG(LogTemp, Warning, TEXT("Session Creating Failed Right away!!!!."));
			SessionPtr->OnCreateSessionCompleteDelegates.RemoveAll(this);
			TerminateSessionServer();
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't find session ptr, terminating..."));
		TerminateSessionServer();
	}
}

void UCGameInstance::OnSessionCreated(FName SessionName, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("-------------------- Session Created!"));
		GetWorld()->GetTimerManager().SetTimer(WaitPlayerJoinTimeoutHandle, this, &UCGameInstance::WaitPlayerJoinTimeoutReached, WaitPlayerJoinTimeoutDuration);
		LoadLevelAndListen(LobbyLevel);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("-------------------- Session Creation Failed!"));
		TerminateSessionServer();
	}
	
	//取消监听
	if (IOnlineSessionPtr SessionPtr = UCNetStatics::GetSessionPtr())
	{
		SessionPtr->OnCreateSessionCompleteDelegates.RemoveAll(this);
	}
}

void UCGameInstance::TerminateSessionServer()
{
	if (IOnlineSessionPtr SessionPtr = UCNetStatics::GetSessionPtr())
	{
		SessionPtr->OnEndSessionCompleteDelegates.RemoveAll(this);
		SessionPtr->OnEndSessionCompleteDelegates.AddUObject(this, &UCGameInstance::EndSessionCompleted);
		// SessionPtr->OnDestroySessionCompleteDelegates.RemoveAll(this);
		if (!SessionPtr->EndSession(FName{ServerSessionName}))
		{
			FGenericPlatformMisc::RequestExit(false);
		}
	}
	else
	{
		FGenericPlatformMisc::RequestExit(false);
	}
}

void UCGameInstance::EndSessionCompleted(FName SessionName, bool bWasSuccessful)
{
	FGenericPlatformMisc::RequestExit(false);
}

void UCGameInstance::WaitPlayerJoinTimeoutReached()
{
	UE_LOG(LogTemp, Warning, TEXT("Session Server shut down after %f seconds without player joining "), WaitPlayerJoinTimeoutDuration);
	TerminateSessionServer();
}

bool UCGameInstance::IsLoggedIn()
{
	if (IOnlineIdentityPtr IdentityPtr = UCNetStatics::GetIdentityPtr())
		return IdentityPtr->GetLoginStatus(0) == ELoginStatus::LoggedIn;
	return false;
}

bool UCGameInstance::IsLoggingIn()
{
	return LoggingDelegateHandle.IsValid();
}

void UCGameInstance::ClientAccountPortalLogin()
{
	ClientLogin("AccountPortal", "", "");
}

void UCGameInstance::ClientLogin(const FString& Type, const FString& Id, const FString& Token)
{
	if (IOnlineIdentityPtr IdentityPtr = UCNetStatics::GetIdentityPtr())
	{
		if (LoggingDelegateHandle.IsValid())
		{
			IdentityPtr->OnLoginCompleteDelegates->Remove(LoggingDelegateHandle);
			LoggingDelegateHandle.Reset();
		}
		LoggingDelegateHandle = IdentityPtr->OnLoginCompleteDelegates->AddUObject(this, &UCGameInstance::LoginCompleted);
		if (!IdentityPtr->Login(0, FOnlineAccountCredentials(Type, Id, Token)))
		{
			UE_LOG(LogTemp, Warning, TEXT("Login Failed Right Away!"));
			if (LoggingDelegateHandle.IsValid())
			{
				IdentityPtr->OnLoginCompleteDelegates->Remove(LoggingDelegateHandle);
				LoggingDelegateHandle.Reset();
			}
			OnLoginCompleted.Broadcast(false, "", "Login Failed Right Away!");
		}
	}
}

void UCGameInstance::LoginCompleted(int NumOfLocalPlayer, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error)
{
	if (IOnlineIdentityPtr IdentityPtr = UCNetStatics::GetIdentityPtr())
	{
		if (LoggingDelegateHandle.IsValid())
		{
			IdentityPtr->OnLoginCompleteDelegates->Remove(LoggingDelegateHandle);
			LoggingDelegateHandle.Reset();
		}
		FString PlayerNickname = "";
		if (bWasSuccessful)
		{
			PlayerNickname = IdentityPtr->GetPlayerNickname(UserId);
			UE_LOG(LogTemp, Warning, TEXT("Logged in successfully as: %s"), *PlayerNickname);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Logged in Failed: %s"), *Error);
		}
		OnLoginCompleted.Broadcast(bWasSuccessful, PlayerNickname, Error);
	}
	else
	{
		OnLoginCompleted.Broadcast(false, "", "Can't find the Identity Pointer");
	}
}

void UCGameInstance::LoadLevelAndListen(TSoftObjectPtr<UWorld> Level)
{
	const FName LevelURL = FName(*FPackageName::ObjectPathToPackageName(Level.ToString()));
	
	if (LevelURL != "")
	{
		FString TravelStr = FString::Printf(TEXT("%s?listen?port=%d"), *LevelURL.ToString(), ServerSessionPort);
		UE_LOG(LogTemp, Warning, TEXT("Server Traveling to: %s"), *TravelStr);
		GetWorld()->ServerTravel(TravelStr);
	}
}

void UCGameInstance::RequestCreateAndJoinSession(const FName& NewSessionName)
{
	UE_LOG(LogTemp, Warning, TEXT("Requesting create and join session: %s"), *(NewSessionName.ToString()));
	
	FHttpRequestRef Request = FHttpModule::Get().CreateRequest(); //创建请求
	FGuid SessionSearchId = FGuid::NewGuid(); //创建GUID
	FString CoordinatorURL = UCNetStatics::GetCoordinatorURL(); //获得协调器地址
	FString URL = FString::Printf(TEXT("%s/Sessions"), *CoordinatorURL);
	UE_LOG(LogTemp, Warning, TEXT("Sending request session creation to URL: %s"), *URL);
	
	Request->SetURL(URL); //设置请求地址
	Request->SetVerb("POST"); //设置为POST请求
	Request->SetHeader("Content-Type", "application/json"); //设置请求头
	
	//设置Json请求对象
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject()); //创建Json对象的共享指针，共享指针好处是可以自动管理内存
	JsonObject->SetStringField(UCNetStatics::GetSessionNameKey().ToString(), NewSessionName.ToString());
	JsonObject->SetStringField(UCNetStatics::GetSessionSearchIdKey().ToString(), SessionSearchId.ToString());
	
	//设置请求体
	FString RequestBody;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer); //序列化Json Object
	Request->SetContentAsString(RequestBody);
	
	Request->OnProcessRequestComplete().BindUObject(this, &UCGameInstance::SessionCreationRequestCompleted, SessionSearchId); //发送请求，创建会话，并绑定事件
	if (!Request->ProcessRequest())
	{
		UE_LOG(LogTemp, Warning, TEXT("Session Creation Request Failed right away!"));
	}
}

void UCGameInstance::CancelSessionCreation()
{
	UE_LOG(LogTemp, Warning, TEXT("Canceling session creation"));
}

void UCGameInstance::SessionCreationRequestCompleted(FHttpRequestPtr Request, FHttpResponsePtr Response,
	bool bWasSuccessful, FGuid SessionSearchId)
{
	if (!bWasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("Connection responded with connection was not successful!"));
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("Connection to Coordinator successfully!"));
	
	//处理response
	int32 ResponseCode = Response->GetResponseCode(); //获得返回code
	if (ResponseCode != 200)
	{
		UE_LOG(LogTemp, Warning, TEXT("Session creation failed with code: %d"), ResponseCode);
		return;
	}
	FString ResponseStr = Response->GetContentAsString(); //获得返回体
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseStr); 
	int32 Port = 0;
	if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid()) //反序列化请求体
	{
		Port = JsonObject->GetIntegerField(*(UCNetStatics::GetPortKey().ToString()));
	}
	UE_LOG(LogTemp, Warning, TEXT("Connected to Coordinator successfully and the new session created is on port: %d"), Port);
	StartFindingCreatedSession(SessionSearchId);
}

void UCGameInstance::StartFindingCreatedSession(const FGuid& SessionSearchId)
{
	if (!SessionSearchId.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Session Search Id is invalid, can't start finding!"));
		return;
	}
	
	StopAllSessionFindings();
	UE_LOG(LogTemp, Warning, TEXT("Starting finding created session with ID: %s"), *(SessionSearchId.ToString()));
	
	GetWorld()->GetTimerManager().SetTimer(
		FindCreatedSessionHandle,
		FTimerDelegate::CreateUObject(this, &UCGameInstance::FindCreatedSession, SessionSearchId),
		FindCreatedSessionSearchInterval,
		true,
		0.f
		);
	GetWorld()->GetTimerManager().SetTimer(
		FindCreatedSessionTimeoutHandle,
		this,
		&UCGameInstance::FindCreatedSessionTimeout,
		FindCreatedSessionTimeoutDuration
		);
}

void UCGameInstance::StopAllSessionFindings()
{
	UE_LOG(LogTemp, Warning, TEXT("Stoping all Session Search"));
	StopFindingCreatedSession();
	StopGlobalSessionSearch();
}

void UCGameInstance::StopFindingCreatedSession()
{
	UE_LOG(LogTemp, Warning, TEXT("Stop finding Created Session"));
}

void UCGameInstance::StopGlobalSessionSearch()
{
	UE_LOG(LogTemp, Warning, TEXT("Stop global Session Search"));
}

void UCGameInstance::FindCreatedSession(FGuid SessionSearchId)
{
	UE_LOG(LogTemp, Warning, TEXT("Trying to find created session..."));
	IOnlineSessionPtr SessionPtr = UCNetStatics::GetSessionPtr();
	if (!SessionPtr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not find session Ptr, canceling session search!"));
		return;
	}
	SessionSearchPtr = MakeShareable(new FOnlineSessionSearch);
	if (!SessionSearchPtr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Unable to create session search, canceling session search"));
		return;
	}
	SessionSearchPtr->bIsLanQuery = false; //不搜索本地局域网
	SessionSearchPtr->MaxSearchResults = 1;
	SessionSearchPtr->QuerySettings.Set(UCNetStatics::GetSessionSearchIdKey(), SessionSearchId.ToString(), EOnlineComparisonOp::Equals);
	SessionPtr->OnFindSessionsCompleteDelegates.RemoveAll(this);
	SessionPtr->OnFindSessionsCompleteDelegates.AddUObject(this, &UCGameInstance::FindCreatedSessionCompleted);
	if (!SessionPtr->FindSessions(0, SessionSearchPtr.ToSharedRef()))
	{
		UE_LOG(LogTemp, Warning, TEXT("Find session failed right away."));
		SessionPtr->OnFindSessionsCompleteDelegates.RemoveAll(this);
	}
}

void UCGameInstance::FindCreatedSessionTimeout()
{
	UE_LOG(LogTemp, Warning, TEXT("Finding created session timeout!"));
	StopFindingCreatedSession();
}

void UCGameInstance::FindCreatedSessionCompleted(bool bWasSuccessful)
{
	if (!bWasSuccessful || !SessionSearchPtr->SearchResults.Num() == 0)
		return;
	
	StopFindingCreatedSession();
	JoinSessionWithSearchResult(SessionSearchPtr->SearchResults[0]);
}

void UCGameInstance::JoinSessionWithSearchResult(const FOnlineSessionSearchResult& SearchResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Joining session with Search Result."))
}

void UCGameInstance::PlayerJoined(const FUniqueNetIdRepl& UniqueId)
{
	if (WaitPlayerJoinTimeoutHandle.IsValid())
		GetWorld()->GetTimerManager().ClearTimer(WaitPlayerJoinTimeoutHandle);
	PlayerRecord.Add(UniqueId);
}

void UCGameInstance::PlayerLeft(const FUniqueNetIdRepl& UniqueId)
{
	PlayerRecord.Remove(UniqueId);
	if (PlayerRecord.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("All player left the session, terminating..."));
		TerminateSessionServer();
	}
}
