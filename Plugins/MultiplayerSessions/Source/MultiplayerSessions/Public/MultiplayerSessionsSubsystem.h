// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MultiplayerSessionsSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMssOnCreateSessionCompleteDelegate, bool, bWasSuccessful);

/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UMultiplayerSessionsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	UMultiplayerSessionsSubsystem();

	void CreateSession(int32 NumPublicConnections, FString MatchType);
	void FindSessions(int32 MaxSearchResults);
	void JoinSession(const FOnlineSessionSearchResult& SearchResult);
	void StartSession();
	void DestroySession();

	FMssOnCreateSessionCompleteDelegate MssOnCreateSessionCompleteDelegate;

protected:
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnFindSessionsComplete(bool bWasSuccessful);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	void OnStartSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);

private:
	IOnlineSessionPtr OnlineSessionInterface;
	TSharedPtr<FOnlineSessionSettings> SessionSettings;

	FOnCreateSessionCompleteDelegate OnCreateSessionCompleteDelegate = FOnCreateSessionCompleteDelegate::CreateUObject(
		this, &ThisClass::OnCreateSessionComplete);
	FDelegateHandle OnCreateSessionCompleteDelegateHandle;

	FOnFindSessionsCompleteDelegate OnFindSessionsCompleteDelegate = FOnFindSessionsCompleteDelegate::CreateUObject(
		this, &ThisClass::OnFindSessionsComplete);
	FDelegateHandle OnFindSessionsCompleteDelegateHandle;

	FOnJoinSessionCompleteDelegate OnJoinSessionCompleteDelegate = FOnJoinSessionCompleteDelegate::CreateUObject(
		this, &ThisClass::OnJoinSessionComplete);
	FDelegateHandle OnJoinSessionCompleteDelegateHandle;

	FOnStartSessionCompleteDelegate OnStartSessionCompleteDelegate = FOnStartSessionCompleteDelegate::CreateUObject(
		this, &ThisClass::OnStartSessionComplete);
	FDelegateHandle OnStartSessionCompleteDelegateHandle;

	FOnDestroySessionCompleteDelegate OnDestroySessionCompleteDelegate = FOnDestroySessionCompleteDelegate::CreateUObject(
		this, &ThisClass::OnDestroySessionComplete);
	FDelegateHandle OnDestroySessionCompleteDelegateHandle;
};
