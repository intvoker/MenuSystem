// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerSessionsSubsystem.h"

#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"

UMultiplayerSessionsSubsystem::UMultiplayerSessionsSubsystem()
{
	if (IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get())
	{
		OnlineSessionInterface = OnlineSubsystem->GetSessionInterface();
	}
}

void UMultiplayerSessionsSubsystem::CreateSession(int32 NumPublicConnections, FString MatchType)
{
	if (!OnlineSessionInterface.IsValid())
	{
		return;
	}

	if (OnlineSessionInterface->GetNamedSession(NAME_GameSession))
	{
		OnlineSessionInterface->DestroySession(NAME_GameSession);
	}

	OnCreateSessionCompleteDelegateHandle = OnlineSessionInterface->AddOnCreateSessionCompleteDelegate_Handle(
		OnCreateSessionCompleteDelegate);

	OnlineSessionSettings = MakeShareable(new FOnlineSessionSettings());
	OnlineSessionSettings->bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL";
	OnlineSessionSettings->NumPublicConnections = NumPublicConnections;
	OnlineSessionSettings->bAllowJoinInProgress = true;
	OnlineSessionSettings->bAllowJoinViaPresence = true;
	OnlineSessionSettings->bShouldAdvertise = true;
	OnlineSessionSettings->bUsesPresence = true;
	OnlineSessionSettings->bUseLobbiesIfAvailable = true;
	OnlineSessionSettings->Set(FName("MatchType"), MatchType, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!OnlineSessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession,
	                                           *OnlineSessionSettings))
	{
		OnlineSessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegateHandle);
		MssOnCreateSessionCompleteDelegate.Broadcast(false);
	}
}

void UMultiplayerSessionsSubsystem::FindSessions(int32 MaxSearchResults)
{
	if (!OnlineSessionInterface.IsValid())
	{
		return;
	}

	OnFindSessionsCompleteDelegateHandle = OnlineSessionInterface->AddOnFindSessionsCompleteDelegate_Handle(
		OnFindSessionsCompleteDelegate);

	OnlineSessionSearch = MakeShareable(new FOnlineSessionSearch());
	OnlineSessionSearch->MaxSearchResults = MaxSearchResults;
	OnlineSessionSearch->bIsLanQuery = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL";;
	OnlineSessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!OnlineSessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(),
	                                          OnlineSessionSearch.ToSharedRef()))
	{
		OnlineSessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegateHandle);
		MssOnFindSessionsCompleteDelegate.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
	}
}

void UMultiplayerSessionsSubsystem::JoinSession(const FOnlineSessionSearchResult& SearchResult)
{
	if (!OnlineSessionInterface.IsValid())
	{
		return;
	}

	OnJoinSessionCompleteDelegateHandle = OnlineSessionInterface->AddOnJoinSessionCompleteDelegate_Handle(
		OnJoinSessionCompleteDelegate);

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!OnlineSessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, SearchResult))
	{
		OnlineSessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegateHandle);
		MssOnJoinSessionCompleteDelegate.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
	}
}

void UMultiplayerSessionsSubsystem::StartSession()
{
}

void UMultiplayerSessionsSubsystem::DestroySession()
{
}

void UMultiplayerSessionsSubsystem::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (!OnlineSessionInterface.IsValid())
	{
		return;
	}

	OnlineSessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegateHandle);
	MssOnCreateSessionCompleteDelegate.Broadcast(bWasSuccessful);
}

void UMultiplayerSessionsSubsystem::OnFindSessionsComplete(bool bWasSuccessful)
{
	if (!OnlineSessionInterface.IsValid())
	{
		return;
	}

	OnlineSessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegateHandle);
	MssOnFindSessionsCompleteDelegate.Broadcast(OnlineSessionSearch->SearchResults, bWasSuccessful);
}

void UMultiplayerSessionsSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (!OnlineSessionInterface.IsValid())
	{
		return;
	}

	OnlineSessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegateHandle);
	MssOnJoinSessionCompleteDelegate.Broadcast(Result);
}

void UMultiplayerSessionsSubsystem::OnStartSessionComplete(FName SessionName, bool bWasSuccessful)
{
}

void UMultiplayerSessionsSubsystem::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
}
