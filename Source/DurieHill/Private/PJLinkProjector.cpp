// Fill out your copyright notice in the Description page of Project Settings.

#include "PJLinkProjector.h"
#include "CoreMinimal.h"
#include "Sockets.h"
#include "SocketSubsystem.h"

APJLinkProjector::APJLinkProjector()
{
    
}

void APJLinkProjector::BeginPlay()
{
    Super::BeginPlay();
    
    Client = new PJLinkClient(IPAddress, Password);

    CurrentStatus = EPJLinkStatus::Unknown;

    Client->SendCommandAsync("POWR ?", FOnConnectDelegate::CreateLambda([this](const FString& Status)
        {
        if (Status.Contains("ERRA")) {
            CurrentStatus = EPJLinkStatus::Unknown;
        }
        else if (Status.Contains("POWR=1")) {
            CurrentStatus = EPJLinkStatus::On;
        }
        else if (Status.Contains("POWR=0")) {
            CurrentStatus = EPJLinkStatus::Off;
        }

        Client->Disconnect();
    }));
    
}

void APJLinkProjector::On()
{
    if (Client == nullptr) {
        UE_LOG(LogPJLink, Warning, TEXT("Client not initialised"));
        Client = new PJLinkClient(IPAddress, Password);
    }

    Client->SendCommandAsync("POWR 1", FOnConnectDelegate::CreateLambda([this](const FString & Status)
        {  
            if (!Status.Contains("ERRA")) {
		        CurrentStatus = EPJLinkStatus::On;
	        } else {
		        CurrentStatus = EPJLinkStatus::Off;
	        }

            Client->Disconnect();
    	}));
}

void APJLinkProjector::Off()
{
    if (Client == nullptr) {
        UE_LOG(LogPJLink, Warning, TEXT("Client not initialised"));
        Client = new PJLinkClient(IPAddress, Password);
    }

    Client->SendCommandAsync("POWR 0", FOnConnectDelegate::CreateLambda([this](const FString& Status)
        {
            if (!Status.Contains("ERRA")) {
                CurrentStatus = EPJLinkStatus::Off;
            }
            else {
                CurrentStatus = EPJLinkStatus::On;
            }

            Client->Disconnect();
        }));
}

void APJLinkProjector::Status(FString& Status)
{

    if (Client == nullptr) {
		UE_LOG(LogPJLink, Warning, TEXT("Client not initialised"));
		Client = new PJLinkClient(IPAddress, Password);
	}

    Client->SendCommandAsync("POWR ?", FOnConnectDelegate::CreateLambda([this](const FString& Status)
        {
            if (Status.Contains("1")) {
                CurrentStatus = EPJLinkStatus::On;
            }
            else {
                CurrentStatus = EPJLinkStatus::Off;
            }

            Client->Disconnect();
        }));
}

