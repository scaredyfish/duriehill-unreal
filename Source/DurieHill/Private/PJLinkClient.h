#pragma once

#include "CoreMinimal.h"
#include "Sockets.h"
#include "SocketSubsystem.h"

DECLARE_LOG_CATEGORY_EXTERN(LogPJLink, Log, All);

class PJLinkClient
{
public:
    PJLinkClient();
    ~PJLinkClient();

    bool Connect(const FString& InIPAddress);
    FString Authorize(const FString& InPassword);
    FString SendCommand(const FString& Command);

private:
    FSocket* Socket;
};