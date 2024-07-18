#include "PJLinkClient.h"
#include "Networking.h"
#include "SocketSubsystem.h"

DEFINE_LOG_CATEGORY(LogPJLink);

PJLinkClient::PJLinkClient(const FString& InIPAddress, const FString& InPassword)
{
    Socket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, TEXT("PJLinkClient"), false);
    Address = InIPAddress;
    Password = InPassword;
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
    Socket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, TEXT("PJLinkClient"), false);

    FIPv4Address IPAddress;
    FIPv4Address::Parse(Address, IPAddress);

    TSharedRef<FInternetAddr> Addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
    Addr->SetIp(IPAddress.Value);
    Addr->SetPort(4352); // Default PJLink port

    return Socket->Connect(*Addr);
}

void PJLinkClient::SendCommandAsync(const FString& Command, FOnConnectDelegate Callback)
{
    Async(EAsyncExecution::Thread, [this, Command, Callback]()
	{
		bool result = Connect();
        if (result) {
            FString Status = Authorize(Command);
            Callback.Execute(Status);
        }
        else
        {
            UE_LOG(LogPJLink, Warning, TEXT("Failed to connect to %s"), *Address);
        } 
	});
}

bool PJLinkClient::Disconnect()
{
    if (Socket == nullptr)
    {
        return false;
    }

    Socket->Close();
    ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(Socket);
    Socket = nullptr;

    return true;
}

FString PJLinkClient::Authorize(const FString& Command)
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
    Salt.Append(Password);
    UE_LOG(LogPJLink, Log, TEXT("Salted password: %s"), *Salt);
    FString HashedPassword = FMD5::HashAnsiString(*Salt);

    FString FullCommand = FString::Printf(TEXT("%%1%s\r"), *Command);
    HashedPassword.Append(FullCommand);

    UE_LOG(LogPJLink, Log, TEXT("Sending data: %s"), *HashedPassword);
    Socket->Send((uint8*)(TCHAR_TO_UTF8(*HashedPassword)), HashedPassword.Len(), BytesRead);

    Socket->Recv(RecvData, sizeof(RecvData), BytesRead);
    Response = FString(BytesRead, UTF8_TO_TCHAR(RecvData));

    UE_LOG(LogPJLink, Log, TEXT("Received data: %s"), *Response);

    return Response;
}

FString PJLinkClient::SendCommand(const FString& Command)
{
    
    if (Socket == nullptr || !(Socket->GetConnectionState() == ESocketConnectionState::SCS_Connected)) 
    {
        if (Connect() == false) {
            return FString("ERRA");
        }
        Authorize();
    }

    uint8 RecvData[18];
    int32 BytesRead;

    FString FullCommand = FString::Printf(TEXT("%%1%s\r"), *Command);

    int32 BytesSent;
    UE_LOG(LogPJLink, Log, TEXT("Sending data: %s"), *FullCommand);
    bool result = Socket->Send((uint8*)(TCHAR_TO_UTF8(*FullCommand)), FullCommand.Len(), BytesSent);

    if (!result) {
        return FString("ERRA");
    }

    Socket->Recv(RecvData, sizeof(RecvData), BytesRead);
    FString Response = FString(BytesRead, UTF8_TO_TCHAR(RecvData));

    UE_LOG(LogPJLink, Log, TEXT("Received data: %s"), *Response);

    Socket->Close();
    ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(Socket);
    Socket = nullptr;

    return Response;
}