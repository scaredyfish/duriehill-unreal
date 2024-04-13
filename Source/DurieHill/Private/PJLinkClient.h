#pragma once

#include "CoreMinimal.h"
#include "Sockets.h"
#include "SocketSubsystem.h"

DECLARE_LOG_CATEGORY_EXTERN(LogPJLink, Log, All);

class PJLinkClient
{
public:
    PJLinkClient(const FString& InIPAddress, const FString& InPassword);
    ~PJLinkClient();

    
    FString SendCommand(const FString& Command);

private:
    bool Connect();
    FString Authorize(const FString& Command = "POWR ?");

    FSocket* Socket;
    FString Password;
    FString Address;
};