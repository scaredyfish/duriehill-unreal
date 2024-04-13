// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PJLinkClient.h"
#include "PJLinkProjector.generated.h"

// enum of pjlink statuses
UENUM(BlueprintType)
enum class EPJLinkStatus : uint8
{
	Unknown = 0,
	Off = 1,
	On = 2,
	Cooling =3,
	Warming = 4,
	Unavailable = 5,
	Error = 6
};

/** Please add a class description */
UCLASS(Blueprintable, BlueprintType)
class APJLinkProjector : public AActor
{
	GENERATED_BODY()
public:
	APJLinkProjector();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	/** Please add a function description */
	UFUNCTION(BlueprintCallable)
	void On();

	/** Please add a function description */
	UFUNCTION(BlueprintCallable)
	void Off();

	/** Please add a function description */
	UFUNCTION(BlueprintCallable)
	void Status(FString& Status);
public:
	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere , Category = "Default")
	FString IPAddress;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default", meta = (MultiLine = "true"))
	FString Password;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Default")
	EPJLinkStatus CurrentStatus;
	
private:
	PJLinkClient* Client;
};
