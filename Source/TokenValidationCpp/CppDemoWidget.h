// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "XsollaLogin/Public/XsollaLoginSubsystem.h"
#include "CppDemoWidget.generated.h"

UCLASS()
class TOKENVALIDATIONCPP_API UCppDemoWidget : public UUserWidget
{
	GENERATED_BODY()

protected:

	UFUNCTION(BlueprintCallable)
	void TestTokenValidation(const FString& tokenToValidate);

	UFUNCTION(BlueprintCallable)
	FString GetUsernameFromToken(const FString& token);

private:

	UFUNCTION()
	void OnTokenValidationSuccess(const FXsollaLoginData& LoginData);

	UFUNCTION()
	void OnTokenValidationError(const FString& Code, const FString& Description);

	UFUNCTION()
	void OnAttributesReceived();
};
