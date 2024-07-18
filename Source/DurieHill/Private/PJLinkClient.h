#pragma once

#include "CoreMinimal.h"
#include "Sockets.h"
#include "SocketSubsystem.h"

DECLARE_LOG_CATEGORY_EXTERN(LogPJLink, Log, All);

DECLARE_DELEGATE_OneParam(FOnConnectDelegate, FString);

class PJLinkClient
{
public:
    PJLinkClient(const FString& InIPAddress, const FString& InPassword);
    ~PJLinkClient();

    bool Connect();
    void SendCommandAsync(const FString& Command, FOnConnectDelegate Callback);
    bool Disconnect();
    FString Authorize(const FString& Command = "POWR ?");
    FString SendCommand(const FString& Command);
    FString Password;
    FString Address;

    FOnConnectDelegate OnConnect;

private:
    FSocket* Socket;

};