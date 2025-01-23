// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"

#include "DebugComponent.generated.h"

class UInputMappingContext;
class UInputAction;

// 入力で処理する関数をバインド
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDebugKeyInputDelegate);

//-----------------DebugAction------------------------------------------------------------
USTRUCT(BlueprintType)
struct FDebugAction
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UInputMappingContext* DebugKeyMappingContext = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UInputAction* Debug_ESC = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UInputAction* Debug_F1 = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UInputAction* Debug_F2 = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UInputAction* Debug_F3 = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UInputAction* Debug_F4 = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UInputAction* Debug_F5 = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UInputAction* Debug_F6 = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UInputAction* Debug_F7 = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UInputAction* Debug_F8 = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UInputAction* Debug_F9 = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UInputAction* Debug_F10 = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UInputAction* Debug_F11 = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UInputAction* Debug_F12 = nullptr;
};
//----------------------------------------------------------------------------------------

class IDebugModule : public IModuleInterface
{
public:
	/**
	 * Singleton-like access to this module's interface.  This is just for convenience
	 * Beware of calling this during the shutdown phase, though.  Your module might have been unloaded already.
	 *
	 * @return Returns singleton instance, loading the module on demand if needed
	 */
	static inline IDebugModule& Get()
	{
		return FModuleManager::LoadModuleChecked< IDebugModule >("DebugModule");
	}
	/**
	 * Checks to see if this module is loaded and ready.  It is only valid to call Get() if IsAvailable() returns true.
	 *
	 * @return True if the module is loaded and ready to use
	 */
	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("DebugModule");
	}
};

UCLASS( ClassGroup=(Debug), meta=(BlueprintSpawnableComponent) )
class DEBUGMODULE_API UDebugComponent : public UActorComponent,public IDebugModule
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDebugComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;


public:	

	// 入力のセットアップ
	void SetupDebugInputComponent(class UInputComponent* DebugInputComponent);

	// デバッグ用キー取得
	FDebugAction GetDebugKeyMapping()const { return DebugActions; }
		
private:

	// オーナーからバインドされた関数を呼び出す
	void TriggerDebugEvent_ESC() { DebugInputReceived_ESC.Broadcast(); }
	void TriggerDebugEvent_F1() { DebugInputReceived_F1.Broadcast(); }
	void TriggerDebugEvent_F2() { DebugInputReceived_F2.Broadcast(); }
	void TriggerDebugEvent_F3() { DebugInputReceived_F3.Broadcast(); }
	void TriggerDebugEvent_F4() { DebugInputReceived_F4.Broadcast(); }
	void TriggerDebugEvent_F5() { DebugInputReceived_F5.Broadcast(); }
	void TriggerDebugEvent_F6() { DebugInputReceived_F6.Broadcast(); }
	void TriggerDebugEvent_F7() { DebugInputReceived_F7.Broadcast(); }
	void TriggerDebugEvent_F8() { DebugInputReceived_F8.Broadcast(); }
	void TriggerDebugEvent_F9() { DebugInputReceived_F9.Broadcast(); }
	void TriggerDebugEvent_F10(){ DebugInputReceived_F10.Broadcast();}
	void TriggerDebugEvent_F11(){ DebugInputReceived_F11.Broadcast();}
	void TriggerDebugEvent_F12(){ DebugInputReceived_F12.Broadcast();}

public:

	// 入力のデリゲート
	FDebugKeyInputDelegate DebugInputReceived_ESC;
	FDebugKeyInputDelegate DebugInputReceived_F1;
	FDebugKeyInputDelegate DebugInputReceived_F2;
	FDebugKeyInputDelegate DebugInputReceived_F3;
	FDebugKeyInputDelegate DebugInputReceived_F4;
	FDebugKeyInputDelegate DebugInputReceived_F5;
	FDebugKeyInputDelegate DebugInputReceived_F6;
	FDebugKeyInputDelegate DebugInputReceived_F7;
	FDebugKeyInputDelegate DebugInputReceived_F8;
	FDebugKeyInputDelegate DebugInputReceived_F9;
	FDebugKeyInputDelegate DebugInputReceived_F10;
	FDebugKeyInputDelegate DebugInputReceived_F11;
	FDebugKeyInputDelegate DebugInputReceived_F12;

protected:

	// デバッグ用のマッピング
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Debug")
		FDebugAction DebugActions;
};
