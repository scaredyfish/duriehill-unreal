#include "PJLinkClient.h"
#include "Networking.h"
#include "SocketSubsystem.h"

DEFINE_LOG_CATEGORY(LogPJLink);

PJLinkClient::PJLinkClient(const FString& InIPAddress, const FString& InPassword)
{
    Password = InPassword;
    Address = InIPAddress;
    Socket = nullptr;
}

PJLinkClient::~PJLinkClient()
{
    if (Socket != nullptr)
    {
        Socket->Close();
        ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(Socket);
    }
}

bool PJLinkClient::Connect()
{
    if (Socket != nullptr)
    {
        Socket->Close();
        ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(Socket);
    }
    Socket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, TEXT("PJLinkClient"), false);
    
    FIPv4Address IPAddress;
    FIPv4Address::Parse(Address, IPAddress);

    TSharedRef<FInternetAddr> Addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
    Addr->SetIp(IPAddress.Value);
    Addr->SetPort(4352); // Default PJLink port

    if (Socket->Connect(*Addr))
    {
        return true;
    }

    return false;
}

FString PJLinkClient::Authorize(const FString& Command)
{
    if (Socket == nullptr || !(Socket->GetConnectionState() == ESocketConnectionState::SCS_Connected))
    {
        UE_LOG(LogPJLink, Log, TEXT("Cannot authorize - socket is not connected"));
        return FString("ERRA");
    }

    uint8 RecvData[18];
    int32 BytesRead;
    FString Response;

    Socket->Recv(RecvData, sizeof(RecvData), BytesRead);
    Response = FString(BytesRead, UTF8_TO_TCHAR(RecvData));

    // log the received data
    UE_LOG(LogPJLink, Log, TEXT("Received data: %s"), *Response);

    // read the last 8 bytes of the received data
    FString Salt = Response;
    Salt.MidInline(9, 8);
    UE_LOG(LogPJLink, Log, TEXT("Salt: %s"), *Salt);
    // append salt to password
    Salt.Append(Password);
    
    UE_LOG(LogPJLink, Log, TEXT("Salted password: %s"), *Salt);
    FString HashedPassword = FMD5::HashAnsiString(*Salt);
    HashedPassword.Append(Command);

    UE_LOG(LogPJLink, Log, TEXT("Sending data: %s"), *HashedPassword);
    Socket->Send((uint8*)TCHAR_TO_UTF8(*HashedPassword), HashedPassword.Len(), BytesRead);

    if (Socket->Wait(ESocketWaitConditions::WaitForRead, FTimespan(0, 0, 10)))
    {
        Socket->Recv(RecvData, 1, BytesRead);
    }
    else
    {
        // Handle timeout, for example log a message and return
        UE_LOG(LogTemp, Warning, TEXT("Timeout waiting for data from socket"));
        return FString("ERRA");
    }

    Response = FString(BytesRead, UTF8_TO_TCHAR(RecvData));

    UE_LOG(LogPJLink, Log, TEXT("Received data: %s"), *Response);

    return Response;
}

FString PJLinkClient::SendCommand(const FString& Command)
{
    if (Socket == nullptr || !(Socket->GetConnectionState() == ESocketConnectionState::SCS_Connected))
    {
        UE_LOG(LogPJLink, Log, TEXT("Socket is not connected"));
        if (Connect()) {
            return Authorize(Command);
        } else {
            return FString("ERRA");
        }
    }

    uint8 RecvData[18];
    int32 BytesRead;
    TArray<uint8> SendData;
    FString FullCommand = FString::Printf(TEXT("%%1%s\r"), *Command);
    SendData.Append((uint8*)TCHAR_TO_UTF8(*FullCommand), FullCommand.Len());

    int32 BytesSent;
    UE_LOG(LogPJLink, Log, TEXT("Sending data: %s"), *FullCommand);
    Socket->Send(SendData.GetData(), SendData.Num(), BytesSent);

    Socket->Recv(RecvData, sizeof(RecvData), BytesRead);
    FString Response = FString(BytesRead, UTF8_TO_TCHAR(RecvData));

    UE_LOG(LogPJLink, Log, TEXT("Received data: %s"), *Response);

    return Response;
}