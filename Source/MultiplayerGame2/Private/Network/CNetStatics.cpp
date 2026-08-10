// Fill out your copyright notice in the Description page of Project Settings.


#include "Network/CNetStatics.h"

uint8 UCNetStatics::GetPlayerCountPerTeam()
{
	return 5;
}

bool UCNetStatics::IsSessionServer(const UObject* WorldContextObject)
{
	return WorldContextObject->GetWorld()->GetNetMode() == NM_DedicatedServer;
}

FString UCNetStatics::GetSessionNameStr()
{
	return GetCommandLineArgsAsString(GetSessionNameKey());
}

FName UCNetStatics::GetSessionNameKey()
{
	return FName("SESSION_NAME");
}

FString UCNetStatics::GetSessionSearchIdStr()
{
	return GetCommandLineArgsAsString(GetSessionSearchIdKey());
}

FName UCNetStatics::GetSessionSearchIdKey()
{
	return FName("SESSION_SEARCH_ID");
}

int UCNetStatics::GetSessionPort()
{
	return GetCommandLineArgsAsInt(GetPortKey());
}

FName UCNetStatics::GetPortKey()
{
	return FName("PORT");
}

FString UCNetStatics::GetCommandLineArgsAsString(const FName& ParamName)
{
	FString OutVal = "";
	FString CommandLineArg = FString::Printf(TEXT("%s="), *(ParamName.ToString()));
	FParse::Value(FCommandLine::Get(), *CommandLineArg, OutVal);
	return OutVal;
}

int UCNetStatics::GetCommandLineArgsAsInt(const FName& ParamName)
{
	int OutVal = 0;
	FString CommandLineArg = FString::Printf(TEXT("%s="), *(ParamName.ToString()));
	FParse::Value(FCommandLine::Get(), *CommandLineArg, OutVal);
	return OutVal;
}

IOnlineSessionPtr UCNetStatics::GetSessionPtr()
{
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem)
		return OnlineSubsystem->GetSessionInterface();
	return nullptr;
}

IOnlineIdentityPtr UCNetStatics::GetIdentityPtr()
{
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem)
		return OnlineSubsystem->GetIdentityInterface();
	return nullptr;
}

FOnlineSessionSettings UCNetStatics::GenerateOnlineSessionSettings(const FName& SessionName, const FString& SessionSearchId, int Port)
{
	FOnlineSessionSettings OnlineSessionSettings{};
	OnlineSessionSettings.bIsLANMatch = false; //局域网匹配功能
	OnlineSessionSettings.NumPublicConnections = GetPlayerCountPerTeam() * 2; //最大连接数
	OnlineSessionSettings.bShouldAdvertise = true; //设置会话公开
	OnlineSessionSettings.bUsesPresence = false; //关闭用户登录
	OnlineSessionSettings.bAllowJoinViaPresence = false;
	OnlineSessionSettings.bAllowJoinViaPresenceFriendsOnly = false; //关闭好友登录
	OnlineSessionSettings.bAllowInvites = true; //允许邀请他人加入会话
	OnlineSessionSettings.bAllowJoinInProgress = false; //关闭中途加入游戏，如观战者可以中途加入游戏
	OnlineSessionSettings.bUseLobbiesIfAvailable = false; //关闭在线大厅
	OnlineSessionSettings.bUseLobbiesVoiceChatIfAvailable = false; //关闭语音聊天
	OnlineSessionSettings.bUsesStats = true; //可用于创建成就系统
	
	OnlineSessionSettings.Set(GetSessionNameKey(), SessionName.ToString(), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	OnlineSessionSettings.Set(GetSessionSearchIdKey(), SessionSearchId, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	OnlineSessionSettings.Set(GetPortKey(), Port, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	
	return OnlineSessionSettings;
}
