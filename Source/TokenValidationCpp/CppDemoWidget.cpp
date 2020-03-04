// Fill out your copyright notice in the Description page of Project Settings.

#include "CppDemoWidget.h"

#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"

void UCppDemoWidget::TestTokenValidation(const FString& tokenToValidate)
{
	// Obtain UXsollaLoginSubsystem reference

	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this);
	UXsollaLoginSubsystem* XsollaLoginSubsystem = GameInstance->GetSubsystem<UXsollaLoginSubsystem>();

	// For this demo token is set manually by calling 'UXsollaLoginSubsystem::SetToken' method

	// Usually token is set automatically during UXsollaLoginSubsystem initialization (in case launcher passes it to UE app via xsolla-login-jwt command line parameter)
	// or during user authentication when calling 'UXsollaLoginSubsystem::AuthenticateUser'

	XsollaLoginSubsystem->SetToken(tokenToValidate);

	// Setting up callbacks

	FOnAuthUpdate successCallback;
	successCallback.BindDynamic(this, &UCppDemoWidget::OnTokenValidationSuccess);

	FOnAuthError errorCallback;
	errorCallback.BindDynamic(this, &UCppDemoWidget::OnTokenValidationError);

	// Call token validation method

	// In case user doesn't want to provide own validation URL in 'Project Settings -> Xsolla Login' default one will be used

	XsollaLoginSubsystem->ValidateToken(successCallback, errorCallback);
}

void UCppDemoWidget::OnTokenValidationSuccess(const FXsollaLoginData& LoginData)
{
	UE_LOG(LogTemp, Warning, TEXT("Yay! Token is valid"));
}

void UCppDemoWidget::OnTokenValidationError(const FString& Code, const FString& Description)
{
	UE_LOG(LogTemp, Error, TEXT("Error code: %s, Error description: %s"), *Code, *Description);
}
