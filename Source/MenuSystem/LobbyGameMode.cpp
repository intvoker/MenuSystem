// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"

#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	PrintPlayers(NewPlayer, true);
}

void ALobbyGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	PrintPlayers(Exiting, false);
}

void ALobbyGameMode::PrintPlayers(const AController* Controller, const bool bIsLoggedIn) const
{
	if (!GameState)
	{
		return;
	}

	const APlayerState* PlayerState = Controller->GetPlayerState<APlayerState>();
	if (!PlayerState)
	{
		return;
	}

	int NumberOfPlayers = GameState.Get()->PlayerArray.Num();
	NumberOfPlayers = bIsLoggedIn ? NumberOfPlayers : NumberOfPlayers - 1;

	const FString PlayerName = PlayerState->GetPlayerName();
	const FString LoggedMessage = bIsLoggedIn ? TEXT("Has Logged In") : TEXT("Has Logged Out");

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 60.f, FColor::Yellow,
		                                 FString::Printf(TEXT("Number of Players: %d"), NumberOfPlayers),
		                                 false);

		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 60.f, FColor::Cyan,
		                                 FString::Printf(TEXT("Player Name: %s %s"), *PlayerName, *LoggedMessage),
		                                 false);
	}
}
