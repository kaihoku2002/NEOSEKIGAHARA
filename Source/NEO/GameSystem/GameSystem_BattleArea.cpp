#include "GameSystem_BattleArea.h"
#include "Components/SceneComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "DrawDebugHelpers.h"
#include "NEO/CharacterSystem/Enemy/EnamyBase.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "Camera/CameraComponent.h"
#include "NEO/CharacterSystem/PlayerSystem/PlayerCharacter.h"

#include "ProceduralMeshComponent.h"
#include "SpawnPoint.h"
#include "NEO/GameSystem/NEOGameMode.h"

// Sets default values
AGameSystem_BattleArea::AGameSystem_BattleArea()
	: MaxCameraDistance(1000.0f)
	, NearPlane(10.0f)
	, FarPlane(500.0f)
	, EnterActorTag("Player")
	, lifeTime(100.0f)
	, thickness(1.0f)
	, SpawnPointTag("SpawnPoint")
	, EnemyCount(0)
	, bIsInBattleArea(false)
	, isEvent(false)
	, EventLangth(0.f)

	//:NowBattleArea(false)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	RootComponent = SceneComponent;
	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(RootComponent);

	//StaticMesh インスタンス
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshComponent->SetupAttachment(RootComponent);
	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	StaticMeshComponent->SetCollisionResponseToAllChannels(ECR_Overlap);
	StaticMeshComponent->SetVisibility(false);

	//Meshの生成
	LeftMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("LeftProceduralMesh"));
	LeftMesh->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);

	RightMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("RightProceduralMesh"));
	RightMesh->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	
	NearMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("NearProceduralMesh"));
	NearMesh->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);

	FarMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("FarProceduralMesh"));
	FarMesh->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);

}


// Called when the game starts or when spawned
void AGameSystem_BattleArea::BeginPlay()
{
	Super::BeginPlay();

	StaticMeshComponent->OnComponentBeginOverlap.AddDynamic(this, &AGameSystem_BattleArea::BeginOverlap);

	GetSpawnPoints();
}

// Called every frame
void AGameSystem_BattleArea::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void AGameSystem_BattleArea::CreateBattleArea()
{
	FMinimalViewInfo CameraViewInfo = GetCameraViewInfo();	//カメラビューの情報
	SFrustumVertices FrustumVertices = GetFrustumVertices(CameraViewInfo);	//カメラビューの頂点情報
	CreateAreaMesh(FrustumVertices);	//メッシュの生成
	
	//デバッグ用
	AreaDebugDraw(FrustumVertices);
}

// 
FMinimalViewInfo AGameSystem_BattleArea::GetCameraViewInfo()
{
	FMinimalViewInfo CameraView;	//カメラビューの情報

	FMatrix ViewMatrix;				//ビュー行列
	FMatrix ProjectionMatrix;		//投影行列
	FMatrix ViewProjectionMatrix;	//ビュー投影行列

	CameraComponent->GetCameraView(0, CameraView);	//カメラビューの情報を取得

	//ビュー投影行列を取得
	UGameplayStatics::GetViewProjectionMatrix(CameraView, ViewMatrix, ProjectionMatrix, ViewProjectionMatrix);

	return CameraView;
}


 /*
  * 関数名　　　　：GetFrustumVertices()
  * 処理内容　　　：カメラビューの情報から視錐台の頂点座標を計算し戻り値として返す
  * 引数１　　　　：FMinimalViewInfo CameraView・・・・・・・・・・・・・カメラビューの情報
  * 戻り値　　　　：視錐台の頂点情報
  */
SFrustumVertices AGameSystem_BattleArea::GetFrustumVertices(FMinimalViewInfo CameraView)
{
	SFrustumVertices FrustumVertices;

	//ビューフラスタムの近い平面の半分の幅を計算する
	float HalfWidth = FMath::Tan(FMath::DegreesToRadians(CameraView.FOV) / 2.0f) * NearPlane;
	//アスペクト比に合わせる
	float HalfHeight = HalfWidth / CameraView.AspectRatio;

	//カメラの位置と向きを取得する
	FVector CameraLocation = CameraView.Location;
	FVector CameraForward = CameraComponent->GetForwardVector();
	FVector CameraRight = CameraComponent->GetRightVector();
	FVector CameraUp = CameraComponent->GetUpVector();

	//視錐台の近い平面の頂点を計算する
	FVector BottomLeftNear = CameraLocation + CameraForward * NearPlane - CameraRight * HalfWidth - CameraUp * HalfHeight;
	FVector TopRightNear = CameraLocation + CameraForward * NearPlane + CameraRight * HalfWidth + CameraUp * HalfHeight;
	FVector BottomRightNear = CameraLocation + CameraForward * NearPlane + CameraRight * HalfWidth - CameraUp * HalfHeight;
	FVector TopLeftNear = CameraLocation + CameraForward * NearPlane - CameraRight * HalfWidth + CameraUp * HalfHeight;

	//ビューフラスタムの遠い平面の半分の幅を計算する
	HalfWidth = FMath::Tan(FMath::DegreesToRadians(CameraView.FOV) / 2.0f) * FarPlane;
	//アスペクト比に合わせる
	HalfHeight = HalfWidth / CameraView.AspectRatio;

	//視錐台の遠い平面の頂点を計算する
	FVector BottomLeftFar = CameraLocation + CameraForward * FarPlane - CameraRight * HalfWidth - CameraUp * HalfHeight;
	FVector TopRightFar = CameraLocation + CameraForward * FarPlane + CameraRight * HalfWidth + CameraUp * HalfHeight;
	FVector BottomRightFar = CameraLocation + CameraForward * FarPlane + CameraRight * HalfWidth - CameraUp * HalfHeight;
	FVector TopLeftFar = CameraLocation + CameraForward * FarPlane - CameraRight * HalfWidth + CameraUp * HalfHeight;
	
	//構造体の変数に代入
	FrustumVertices.BottomLeftNear = BottomLeftNear;
	FrustumVertices.TopRightNear = TopRightNear;
	FrustumVertices.BottomRightNear = BottomRightNear;
	FrustumVertices.TopLeftNear = TopLeftNear;
	FrustumVertices.BottomLeftFar = BottomLeftFar;
	FrustumVertices.TopRightFar = TopRightFar;
	FrustumVertices.BottomRightFar = BottomRightFar;
	FrustumVertices.TopLeftFar = TopLeftFar;


	return FrustumVertices;
}

 /*
  * 関数名　　　　：CreateAreaMesh()
  * 処理内容　　　：ビューフラスタムを使って2つのメッシュを作成し、それぞれ左辺と右辺に配置する処理
  * 引数１　　　　：SFrustumVertices FrustumVertices ・・・・・・・視錐台の頂点。メッシュ作成に使用される
  * 戻り値　　　　：なし
  */
void AGameSystem_BattleArea::CreateAreaMesh(SFrustumVertices FrustumVertices)
{
	//　左側の平面の頂点を追加
	TArray<FVector> Vertices;
	Vertices.Add(FrustumVertices.BottomLeftNear);
	Vertices.Add(FrustumVertices.TopLeftNear);
	Vertices.Add(FrustumVertices.TopLeftFar);
	Vertices.Add(FrustumVertices.BottomLeftFar);

	//　Trianglesの配列を作成
	TArray<int32> Triangles;
	Triangles.Add(0);
	Triangles.Add(1);
	Triangles.Add(2);
	Triangles.Add(2);
	Triangles.Add(3);
	Triangles.Add(0);

	// メッシュの作成
	LeftMesh->CreateMeshSection_LinearColor(0, Vertices, Triangles, TArray<FVector>(), TArray<FVector2D>(), TArray<FLinearColor>(), TArray<FProcMeshTangent>(), true);
	//　衝突を無効に
	LeftMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);


	//　右側の平面の頂点を追加
	Vertices.Reset();
	Vertices.Add(FrustumVertices.BottomRightNear);
	Vertices.Add(FrustumVertices.TopRightNear);
	Vertices.Add(FrustumVertices.TopRightFar);
	Vertices.Add(FrustumVertices.BottomRightFar);

	// メッシュの作成
	RightMesh->CreateMeshSection_LinearColor(0, Vertices, Triangles, TArray<FVector>(), TArray<FVector2D>(), TArray<FLinearColor>(), TArray<FProcMeshTangent>(), true);
	//　衝突を無効に
	RightMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);


	//近い平面の頂点を追加
	Vertices.Reset();
	Vertices.Add(FrustumVertices.BottomLeftNear);
	Vertices.Add(FrustumVertices.TopLeftNear);
	Vertices.Add(FrustumVertices.TopRightNear);
	Vertices.Add(FrustumVertices.BottomRightNear);

	// メッシュの作成
	NearMesh->CreateMeshSection_LinearColor(0, Vertices, Triangles, TArray<FVector>(), TArray<FVector2D>(), TArray<FLinearColor>(), TArray<FProcMeshTangent>(), true);
	//　衝突を無効に
	NearMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);


	//遠い平面の頂点を追加
	Vertices.Reset();
	Vertices.Add(FrustumVertices.BottomLeftFar);
	Vertices.Add(FrustumVertices.TopLeftFar);
	Vertices.Add(FrustumVertices.TopRightFar);
	Vertices.Add(FrustumVertices.BottomRightFar);

	// メッシュの作成
	FarMesh->CreateMeshSection_LinearColor(0, Vertices, Triangles, TArray<FVector>(), TArray<FVector2D>(), TArray<FLinearColor>(), TArray<FProcMeshTangent>(), true);
	//　衝突を無効に
	FarMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);


	LeftMesh->SetVisibility(false);
	RightMesh->SetVisibility(false);
	NearMesh->SetVisibility(false);
	FarMesh->SetVisibility(false);
}


void AGameSystem_BattleArea::AreaDebugDraw(SFrustumVertices FrustumVertices)
{
	//視錐台の頂点を描画する
	
}


void AGameSystem_BattleArea::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ACharacter* Character = Cast<ACharacter>(OtherActor);
	if (Character)	{
		if (Character->ActorHasTag(EnterActorTag))		{
			StaticMeshComponent->DestroyComponent();	// コンポーネントを破壊

			EnterBattleArea(isEvent);
		}
	}
}


int32 AGameSystem_BattleArea::CheckEnemyCount()
{
	return 0;
}


/*
 * 関数名　　　　：EnterBattleArea()
 * 処理内容　　　：バトルエリアに侵入した時の処理
 * 引数１　　　　：bool _isEvent ・・・・・・・イベントの有無
 * 戻り値　　　　：なし
 */
void AGameSystem_BattleArea::EnterBattleArea(bool _isEvent)
{
	//カメラをコンポーネントを別のカメラに設定
	ANEOGameMode* GameMode = GetWorld()->GetAuthGameMode<ANEOGameMode>();

	//Transformを設定
	UCameraComponent* NowCamera = GameMode->GetNowPlayerCamera()->FindComponentByClass<UCameraComponent>();
	CameraComponent->SetWorldTransform(NowCamera->GetComponentTransform());
	CameraComponent->FieldOfView = NowCamera->FieldOfView;
	CameraComponent->AspectRatio = NowCamera->AspectRatio;


	//Meshの生成
	auto Transform = GetActorTransform();

	// RootのTransformを初期化
	GetRootComponent()->SetRelativeTransform(FTransform::Identity);

	CreateBattleArea();

	GetRootComponent()->SetRelativeTransform(Transform);

	//GameModeに入る
	if (GameMode) {
		bIsInBattleArea = true;
		if (SpawnPoints.Num() == 0) {
			UE_LOG(LogTemp, Warning, TEXT("SpawnPoints is empty"));
			return;
		}

		//イベントがあったらその時間を代入(なければ0を入れる)
		if (_isEvent)
		{
			GameMode->SetIsOnBattleArea(bIsInBattleArea,SpawnPoints, this, LeftMesh, RightMesh, NearMesh, FarMesh, EventLangth);
		}
		else
		{
			GameMode->SetIsOnBattleArea(bIsInBattleArea, SpawnPoints, this, LeftMesh, RightMesh, NearMesh, FarMesh, 0.f);
		}

	}
}

void AGameSystem_BattleArea::ExitBattleArea()
{

}


/*
 * 関数名　　　　：GetSpawnPoints()
 * 処理内容　　　：スポーンポイント情報の取得
 * 戻り値　　　　：なし
 */
void AGameSystem_BattleArea::GetSpawnPoints()
{
	SpawnPoints.Reset();
	//SpawnPointsの取得
	TArray<AActor*> ASpawnPoints;

	//tagを使ってSpawnPointsの取得
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), SpawnPointTag, ASpawnPoints);

	//SpawnPointsをSpawnPointのデータ型に変換
	for (AActor* SpawnPoint : ASpawnPoints)	{
		SpawnPoints.Add(Cast<ASpawnPoint>(SpawnPoint));
	}

}

void AGameSystem_BattleArea::IgnoreCollision()
{
	UE_LOG(LogTemp, Warning, TEXT("IgnoreCollision called"));
	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	NearMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FarMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);


}

void AGameSystem_BattleArea::ResetCollision()
{
	UE_LOG(LogTemp, Warning, TEXT("ResetCollision called"));
	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	LeftMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	RightMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	NearMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	FarMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

}

void AGameSystem_BattleArea::BPFunction_Implementation()
{
}

void AGameSystem_BattleArea::ExitAreaEvent_Implementation()
{
}
