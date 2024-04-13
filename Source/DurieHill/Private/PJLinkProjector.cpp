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
    Client->Connect();
    FString Status = Client->Authorize();
   
    if (Status.Contains("ERRA")) {
        CurrentStatus = EPJLinkStatus::Unknown;
    } else if (Status.Contains("POWR=1")) {
		CurrentStatus = EPJLinkStatus::On;
	} else if (Status.Contains("POWR=0")) {
		CurrentStatus = EPJLinkStatus::Off;
	}
}

void APJLinkProjector::On()
{
    // Turn on the projector
    FString Status = Client->SendCommand("POWR 1");

    if (!Status.Contains("ERRA")) {
		CurrentStatus = EPJLinkStatus::On;
	} else {
		CurrentStatus = EPJLinkStatus::Off;
	}
}

void APJLinkProjector::Off()
{
    // Turn off the projector
    FString Status = Client->SendCommand("POWR 0");

    if (!Status.Contains("ERRA")) {
        CurrentStatus =  EPJLinkStatus::Off;
    } else {
        CurrentStatus = EPJLinkStatus::On;
    }
}

void APJLinkProjector::Status(FString& Status)
{
    // Get the projector status
    Status = Client->SendCommand("POWR ?");

    if (Status.Contains("1")) {
		CurrentStatus = EPJLinkStatus::On;
	}
    else {
		CurrentStatus = EPJLinkStatus::Off;
	}   
}

