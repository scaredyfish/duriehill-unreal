#include "PJLinkClient.h"
#include "Networking.h"
#include "SocketSubsystem.h"

DEFINE_LOG_CATEGORY(LogPJLink);

PJLinkClient::PJLinkClient()
{
     Socket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, TEXT("PJLinkClient"), false);
}

PJLinkClient::~PJLinkClient()
{
    if (Socket != nullptr)
    {
        Socket->Close();
        ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(Socket);
    }
}

bool PJLinkClient::Connect(const FString& InIPAddress)
{
    if (Socket == nullptr)
    {
        return false;
    }

    FIPv4Address IPAddress;
    FIPv4Address::Parse(InIPAddress, IPAddress);

    TSharedRef<FInternetAddr> Addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
    Addr->SetIp(IPAddress.Value);
    Addr->SetPort(4352); // Default PJLink port

    return Socket->Connect(*Addr);
}

FString PJLinkClient::Authorize(const FString& InPassword)
{
    if (Socket == nullptr)
    {
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
    // append salt to password
    Salt.Append(InPassword);
    UE_LOG(LogPJLink, Log, TEXT("Salted password: %s"), *Salt);
    FString HashedPassword = FMD5::HashAnsiString(*Salt);
    HashedPassword.Append(TEXT("%1POWR ?\r"));

    UE_LOG(LogPJLink, Log, TEXT("Sending data: %s"), *HashedPassword);
    Socket->Send((uint8*)(TCHAR_TO_UTF8(*HashedPassword)), HashedPassword.Len(), BytesRead);

    Socket->Recv(RecvData, sizeof(RecvData), BytesRead);
    Response = FString(BytesRead, UTF8_TO_TCHAR(RecvData));

    UE_LOG(LogPJLink, Log, TEXT("Received data: %s"), *Response);

    return Response;
}

FString PJLinkClient::SendCommand(const FString& Command)
{
    if (Socket == nullptr)
    {
        return FString("ERRA");
    }

    uint8 RecvData[18];
    int32 BytesRead;
    TArray<uint8> SendData;
    FString FullCommand = FString::Printf(TEXT("%%1%s\r"), *Command);
    SendData.Append((uint8*)TCHAR_TO_UTF8(*FullCommand), FullCommand.Len());

    int32 BytesSent;
    Socket->Send(SendData.GetData(), SendData.Num(), BytesSent);

    Socket->Recv(RecvData, sizeof(RecvData), BytesRead);
    FString Response = FString(BytesRead, UTF8_TO_TCHAR(RecvData));

    UE_LOG(LogPJLink, Log, TEXT("Received data: %s"), *Response);

    return Response;
}