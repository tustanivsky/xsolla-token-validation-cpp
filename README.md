Check out https://github.com/tustanivsky/xsolla-token-validation-cpp/blob/master/Source/TokenValidationCpp/CppDemoWidget.cpp

To use Xsolla Login SDK and validate JWT token in UE4 C++ projects following steps should be completed:

1. Add `XsollaLogin` module dependency to your game's *Build.cs file

```cpp
PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "XsollaLogin" });
```

2. Add required include directives to your source code files

```cpp
#include "XsollaLogin/Public/XsollaLoginSubsystem.h"
```

3. Obtain `UXsollaLoginSubsystem` reference

```cpp
UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this);
UXsollaLoginSubsystem* XsollaLoginSubsystem = GameInstance->GetSubsystem<UXsollaLoginSubsystem>();
```

4. Prepare `UXsollaLoginSubsystem` for token validation. Usually token is set automatically during `UXsollaLoginSubsystem` initialization (in case launcher passes it to UE app via `xsolla-login-jwt` command line parameter) or during user authentication when calling `UXsollaLoginSubsystem::AuthenticateUser`. Note that for this demo token is set manually by calling `UXsollaLoginSubsystem::SetToken` method.

5. Set up callbacks for token validation method

```cpp
FOnAuthUpdate successCallback;
successCallback.BindDynamic(this, &UCppDemoWidget::OnTokenValidationSuccess);

FOnAuthError errorCallback;
errorCallback.BindDynamic(this, &UCppDemoWidget::OnTokenValidationError);
```

6. Call token validation method

```cpp
XsollaLoginSubsystem->ValidateToken(successCallback, errorCallback);
```
