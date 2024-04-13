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

    bool Connect();
    FString Authorize();
    FString SendCommand(const FString& Command);
    FString Password;
    FString Address;
private:
    FSocket* Socket;

};