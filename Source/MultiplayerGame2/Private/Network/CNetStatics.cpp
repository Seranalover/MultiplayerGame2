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
