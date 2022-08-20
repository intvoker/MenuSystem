// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Menu.generated.h"

class UMultiplayerSessionsSubsystem;
class UButton;
/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void MenuSetup(int32 NumPublicConnectionsParam = 4, FString MatchTypeParam = FString("FreeForAll"),
	               FString PathToLobbyParam = FString("/Game/ThirdPerson/Maps/LobbyMap"));

protected:
	virtual bool Initialize() override;
	virtual void OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld) override;

	UFUNCTION()
	void MssOnCreateSessionComplete(bool bWasSuccessful);
	void MssOnFindSessionsComplete(const TArray<FOnlineSessionSearchResult>& SearchResults, bool bWasSuccessful);
	void MssOnJoinSessionComplete(EOnJoinSessionCompleteResult::Type Result);
	UFUNCTION()
	void MssOnStartSessionComplete(bool bWasSuccessful);
	UFUNCTION()
	void MssOnDestroySessionComplete(bool bWasSuccessful);

private:
	UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem;

	UPROPERTY(meta=(BindWidget))
	UButton* HostButton;

	UPROPERTY(meta=(BindWidget))
	UButton* JoinButton;

	UFUNCTION()
	void HostButtonClicked();

	UFUNCTION()
	void JoinButtonClicked();

	void MenuTearDown();

	int32 NumPublicConnections = 4;
	FString MatchType = FString("FreeForAll");
	FString PathToLobby = FString("/Game/ThirdPerson/Maps/LobbyMap");
};
