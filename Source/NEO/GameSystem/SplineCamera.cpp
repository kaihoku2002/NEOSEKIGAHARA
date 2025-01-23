// Fill out your copyright notice in the Description page of Project Settings.


#include "SplineCamera.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PlayerController.h"
#include "Components/SplineComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Math/UnrealMathUtility.h"
#include "NEO/CharacterSystem/PlayerSystem/PlayerCharacter.h"
#include "DrawDebugHelpers.h"
#include "Components/BoxComponent.h"							//ボックスコンポーネントを使うため
#include "NEO/GameSystem/NEOGameMode.h"

#define YAW_ROTATION -90.f
#define PITCH_ROTATION -25.f

#define CAMERA_MOVE_SPEED 0.08

// Sets default values
ASplineCamera::ASplineCamera()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpringArm"));
	CameraSpringArm->SetupAttachment(RootComponent);
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(CameraSpringArm);

	//CreateSplineComponent
	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));

	//Box
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->SetupAttachment(CameraComponent);

	bMoveFlag = false;
	CameraSpeed = 300.0f;
	CurrentRatio = 0;

}


void ASplineCamera::SetPlayerCamera()
{
	//SetViewTarget
	pGameMode = Cast<ANEOGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (pGameMode)
	{
		pGameMode->SetPlayerCamera(this);
		pGameMode->SetViewTargetWithBlend(this);
	}

	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
	//SetOwner
	if (PlayerCharacter) {
		PlayerCharacter->SetOwner(this);
	}
}

// Called when the game starts or when spawned
void ASplineCamera::BeginPlay()
{
	Super::BeginPlay();

	if (SplineComponent) {
		//Spline曲線上の開始地点にSplineカメラの位置をセット
		FVector CameraLocation = SplineComponent->GetLocationAtDistanceAlongSpline(0, ESplineCoordinateSpace::World);

		CameraComponent->SetWorldLocation(CameraLocation);
	}

	
}

// Called every frame
void ASplineCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//プレイヤーの位置に合わせてSplineカメラの位置をセット
	SetCamera();
}


/*
 * 関数名　　　　：SetCamera()
 * 処理内容　　　：Splineカメラをプレイヤーの位置にセット
 * 戻り値　　　　：なし
 */
void ASplineCamera::SetCamera()
{
	// プレイヤーを取得
	APlayerBase* PlayerCharacter = Cast<APlayerBase>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (PlayerCharacter && SplineComponent)
	{

		// プレイヤーの位置を取得
		FVector PlayerLocation = PlayerCharacter->GetActorLocation();

		// Spline曲線上でプレイヤーに最も近い点を取得
		FVector NearestPoint = SplineComponent->FindLocationClosestToWorldLocation(PlayerLocation, ESplineCoordinateSpace::World);


		//カメラの座標を取得
		FVector BeforeCameraLocation = CameraComponent->GetComponentLocation();		

		// カメラの座標をSpline曲線上でプレイヤーに最も近い点に移動させる
		FVector CameraLocation = FMath::Lerp(BeforeCameraLocation, NearestPoint, CAMERA_MOVE_SPEED);

		//カメラ位置を反映
		CameraComponent->SetWorldLocation(CameraLocation);


		//Spline曲線上でプレイヤーに最も近い点の回転を取得
		FRotator NewRotation = SplineComponent->FindRotationClosestToWorldLocation(CameraLocation, ESplineCoordinateSpace::World);

		//プレイヤーの方を向くように調整
		NewRotation.Yaw += YAW_ROTATION;
		NewRotation.Pitch = PITCH_ROTATION;

		//カメラ回転を反映
		CameraComponent->SetWorldRotation(NewRotation);
	}
}