// Copyright 2019 Xsolla Inc. All Rights Reserved.

#include "XsollaPayStationSubsystem.h"

#include "XsollaPayStation.h"
#include "XsollaPayStationDefines.h"
#include "XsollaPayStationSettings.h"

#include "Engine/Engine.h"
#include "Modules/ModuleManager.h"
#include "Runtime/Launch/Resources/Version.h"
#include "UObject/ConstructorHelpers.h"
#include "UObject/Package.h"

#define LOCTEXT_NAMESPACE "FXsollaPayStationModule"

const FString UXsollaPayStationSubsystem::PaymentEndpoint(TEXT("https://secure.xsolla.com/paystation3"));
const FString UXsollaPayStationSubsystem::SandboxPaymentEndpoint(TEXT("https://sandbox-secure.xsolla.com/paystation3"));

UXsollaPayStationSubsystem::UXsollaPayStationSubsystem()
	: UGameInstanceSubsystem()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> BrowserWidgetFinder(TEXT("/Xsolla/Browser/W_PayStationBrowser.W_PayStationBrowser_C"));
	DefaultBrowserWidgetClass = BrowserWidgetFinder.Class;
}

void UXsollaPayStationSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UE_LOG(LogXsollaPayStation, Log, TEXT("%s: XsollaPayStation subsystem initialized"), *VA_FUNC_LINE);
}

void UXsollaPayStationSubsystem::Deinitialize()
{
	// Do nothing for now
	Super::Deinitialize();
}

void UXsollaPayStationSubsystem::FetchPaymentToken(const FOnFetchPaymentTokenSuccess& SuccessCallback, const FOnPayStationError& ErrorCallback)
{
	const UXsollaPayStationSettings* Settings = FXsollaPayStationModule::Get().GetSettings();
	TSharedRef<IHttpRequest> HttpRequest = CreateHttpRequest(Settings->TokenRequestURL);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UXsollaPayStationSubsystem::FetchPaymentToken_HttpRequestComplete, SuccessCallback, ErrorCallback);
	HttpRequest->ProcessRequest();
}

void UXsollaPayStationSubsystem::LaunchPaymentConsole(const FString& PaymentToken, UUserWidget*& BrowserWidget)
{
	const FString Endpoint = IsSandboxEnabled() ? SandboxPaymentEndpoint : PaymentEndpoint;
	const FString PayStationUrl = FString::Printf(TEXT("%s?access_token=%s"), *Endpoint, *PaymentToken);

	UE_LOG(LogXsollaPayStation, Log, TEXT("%s: Loading PayStation: %s"), *VA_FUNC_LINE, *PayStationUrl);

	const UXsollaPayStationSettings* Settings = FXsollaPayStationModule::Get().GetSettings();

	// Check for user browser widget override
	auto BrowserWidgetClass = (Settings->OverrideBrowserWidgetClass) ? Settings->OverrideBrowserWidgetClass : DefaultBrowserWidgetClass;

	PengindPayStationUrl = PayStationUrl;
	auto MyBrowser = CreateWidget<UUserWidget>(GEngine->GameViewport->GetWorld(), BrowserWidgetClass);
	MyBrowser->AddToViewport(MAX_int32);

	BrowserWidget = MyBrowser;
}

FString UXsollaPayStationSubsystem::GetPendingPayStationUrl() const
{
	return PengindPayStationUrl;
}

void UXsollaPayStationSubsystem::FetchPaymentToken_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded, FOnFetchPaymentTokenSuccess SuccessCallback, FOnPayStationError ErrorCallback)
{
	if (bSucceeded && HttpResponse.IsValid())
	{
		FString PaymentToken = HttpResponse->GetContentAsString();
		UE_LOG(LogXsollaPayStation, Verbose, TEXT("%s: Response: %s"), *VA_FUNC_LINE, *PaymentToken);

		SuccessCallback.ExecuteIfBound(PaymentToken);
	}
	else
	{
		UE_LOG(LogXsollaPayStation, Warning, TEXT("%s: request failed"), *VA_FUNC_LINE);
		ErrorCallback.ExecuteIfBound(204, 0, TEXT("No response"));
	}
}

bool UXsollaPayStationSubsystem::IsSandboxEnabled() const
{
	const UXsollaPayStationSettings* Settings = FXsollaPayStationModule::Get().GetSettings();
	bool bIsSandboxEnabled = Settings->bSandbox;

#if UE_BUILD_SHIPPING
	bIsSandboxEnabled = Settings->bSandbox && Settings->bEnableSandboxInShipping;
	if (bIsSandboxEnabled)
	{
		UE_LOG(LogXsollaPayStation, Warning, TEXT("%s: Sandbox should be disabled in Shipping build"), *VA_FUNC_LINE);
	}
#endif // UE_BUILD_SHIPPING

	return bIsSandboxEnabled;
}

TSharedRef<IHttpRequest> UXsollaPayStationSubsystem::CreateHttpRequest(const FString& Url)
{
	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();

	const FString MetaUrl = FString::Printf(TEXT("%sengine=ue4&engine_v=%s&sdk=paystation&sdk_v=%s"),
		Url.Contains(TEXT("?")) ? TEXT("&") : TEXT("?"),
		ENGINE_VERSION_STRING,
		XSOLLA_PAYSTATION_VERSION);
	HttpRequest->SetURL(Url + MetaUrl);

	HttpRequest->SetVerb(TEXT("POST"));

	// Xsolla meta
	HttpRequest->SetHeader(TEXT("X-ENGINE"), TEXT("UE4"));
	HttpRequest->SetHeader(TEXT("X-ENGINE-V"), ENGINE_VERSION_STRING);
	HttpRequest->SetHeader(TEXT("X-SDK"), TEXT("PAYSTATION"));
	HttpRequest->SetHeader(TEXT("X-SDK-V"), XSOLLA_PAYSTATION_VERSION);

	return HttpRequest;
}

#undef LOCTEXT_NAMESPACE