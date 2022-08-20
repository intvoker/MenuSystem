// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu.h"

#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "Components/Button.h"

void UMenu::MenuSetup(int32 NumPublicConnectionsParam, FString MatchTypeParam, FString PathToLobbyParam)
{
	NumPublicConnections = NumPublicConnectionsParam;
	MatchType = MatchTypeParam;
	PathToLobby = PathToLobbyParam;

	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	bIsFocusable = true;

	if (const UWorld* World = GetWorld())
	{
		if (APlayerController* PlayerController = World->GetFirstPlayerController())
		{
			FInputModeUIOnly InputModeData;
			InputModeData.SetWidgetToFocus(TakeWidget());
			InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(true);
		}
	}

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
	}

	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->MssOnCreateSessionCompleteDelegate.AddDynamic(
			this, &ThisClass::MssOnCreateSessionComplete);
		MultiplayerSessionsSubsystem->MssOnFindSessionsCompleteDelegate.AddUObject(
			this, &ThisClass::MssOnFindSessionsComplete);
		MultiplayerSessionsSubsystem->MssOnJoinSessionCompleteDelegate.AddUObject(
			this, &ThisClass::MssOnJoinSessionComplete);
		MultiplayerSessionsSubsystem->MssOnStartSessionCompleteDelegate.AddDynamic(
			this, &ThisClass::MssOnStartSessionComplete);
		MultiplayerSessionsSubsystem->MssOnDestroySessionCompleteDelegate.AddDynamic(
			this, &ThisClass::MssOnDestroySessionComplete);
	}
}

bool UMenu::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}

	if (HostButton)
	{
		HostButton->OnClicked.AddDynamic(this, &ThisClass::HostButtonClicked);
	}

	if (JoinButton)
	{
		JoinButton->OnClicked.AddDynamic(this, &ThisClass::JoinButtonClicked);
	}

	return true;
}

void UMenu::OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld)
{
	MenuTearDown();

	Super::OnLevelRemovedFromWorld(InLevel, InWorld);
}

void UMenu::MssOnCreateSessionComplete(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Yellow,
			                                 FString(TEXT("Create Session Successful")), false);
		}

		if (UWorld* World = GetWorld())
		{
			World->ServerTravel(FString::Printf(TEXT("%s?listen"), *PathToLobby));
		}
	}
	else
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Red,
			                                 FString(TEXT("Create Session Failed")), false);
		}
	}
}

void UMenu::MssOnFindSessionsComplete(const TArray<FOnlineSessionSearchResult>& SearchResults, bool bWasSuccessful)
{
	for (auto SearchResult : SearchResults)
	{
		FString MatchTypeParam;
		SearchResult.Session.SessionSettings.Get(FName("MatchType"), MatchTypeParam);

		if (MatchTypeParam == MatchType)
		{
			if (MultiplayerSessionsSubsystem)
			{
				MultiplayerSessionsSubsystem->JoinSession(SearchResult);
			}

			return;
		}
	}
}

void UMenu::MssOnJoinSessionComplete(EOnJoinSessionCompleteResult::Type Result)
{
	if (IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get())
	{
		if (IOnlineSessionPtr OnlineSessionInterface = OnlineSubsystem->GetSessionInterface())
		{
			if (FString ConnectInfo; OnlineSessionInterface->GetResolvedConnectString(NAME_GameSession, ConnectInfo))
			{
				if (APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController())
				{
					PlayerController->ClientTravel(ConnectInfo, ETravelType::TRAVEL_Absolute);
				}
			}
		}
	}
}

void UMenu::MssOnStartSessionComplete(bool bWasSuccessful)
{
}

void UMenu::MssOnDestroySessionComplete(bool bWasSuccessful)
{
}

void UMenu::HostButtonClicked()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Yellow,
		                                 FString(TEXT("Host Button Clicked")), false);
	}

	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->CreateSession(NumPublicConnections, MatchType);
	}
}

void UMenu::JoinButtonClicked()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Yellow,
		                                 FString(TEXT("Join Button Clicked")), false);
	}

	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->FindSessions(10000);
	}
}

void UMenu::MenuTearDown()
{
	RemoveFromParent();

	if (const UWorld* World = GetWorld())
	{
		if (APlayerController* PlayerController = World->GetFirstPlayerController())
		{
			FInputModeGameOnly InputModeData;
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(false);
		}
	}
}
