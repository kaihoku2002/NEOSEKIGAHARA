#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameSystem_BattleArea.generated.h"

class ATGS_GameMode;

struct SFrustumVertices
{
	FVector TopLeftNear;
	FVector TopRightNear;
	FVector BottomLeftNear;
	FVector BottomRightNear;
	FVector TopLeftFar;
	FVector TopRightFar;
	FVector BottomLeftFar;
	FVector BottomRightFar;
};

UCLASS()
class NEO_API AGameSystem_BattleArea : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGameSystem_BattleArea();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BattleArea")
		class USceneComponent* SceneComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BattleArea")
		class UCameraComponent* CameraComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BattleArea")
		class UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BattleArea")
		float MaxCameraDistance;

	//�J�����̋ߕ��ʂƉ����ʂ̋���
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BattleArea")
		float NearPlane;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BattleArea")
		float FarPlane;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "BattleArea")
		FName EnterActorTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BattleAreaDebug")
		float lifeTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BattleAreaDebug")
	    float thickness;


	//�o�g���G���A�̃��b�V��
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BattleArea")
		class UProceduralMeshComponent* LeftMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BattleArea")
		class UProceduralMeshComponent* RightMesh;

	//�߂����̃��b�V��
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BattleArea")
		class UProceduralMeshComponent* NearMesh;

	//�������̃��b�V��
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BattleArea")
		class UProceduralMeshComponent* FarMesh;

		//SpawnPoint�̃^�O��
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BattleArea")
		FName SpawnPointTag;

	//�G�̃J�E���g
	int32 EnemyCount;

	//flag
	bool bIsInBattleArea;

	TArray<class ASpawnPoint*> SpawnPoints;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void CreateBattleArea(); //�o�g���G���A�𐶐�����

	inline FMinimalViewInfo GetCameraViewInfo(); //�J�����̎�����擾����

	SFrustumVertices GetFrustumVertices(FMinimalViewInfo CameraView); //�o�g���G���A�̃��b�V���𐶐�����

	void CreateAreaMesh(SFrustumVertices FrustumVertices); //���b�V���𐶐�����

	void AreaDebugDraw(SFrustumVertices FrustumVertices); //�f�o�b�O�p�̕`��

	//�I�[�o�[���b�v�C�x���g
	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	//�G���A���̓G�̐����`�F�b�N
	int32 CheckEnemyCount();

	//�o�g���G���A�ɓ���
	void EnterBattleArea(bool _isEvent);

	//���̃J�������C�x���g�t�����ǂ���
	UPROPERTY(EditAnywhere)
	bool isEvent;

	//�C�x���g�����Ă����玞�Ԃ������ɓ����
	UPROPERTY(EditAnywhere)
	float EventLangth;

	//�o�g���G���A����o��
	void ExitBattleArea();

	//SpawnActor���擾����
	void GetSpawnPoints();

	// �R���W�����𖳎������
	void IgnoreCollision();
	// �R���W���������ɖ߂��֐�
	void ResetCollision();

	virtual void BPFunction_Implementation();
	
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable, Category = "BattleArea")
		void ExitAreaEvent();

	virtual void ExitAreaEvent_Implementation();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BattleArea")
		int32 LSNum = 0;	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BattleArea")
		int32 ExitEventNum = 0;


};
