// Copyright Epic Games, Inc. All Rights Reserved.

#include "NetTPSCDCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "HPbarWidget.h"
#include "InputActionValue.h"
#include "MainUI.h"
#include "NetPlayerAnimInstance.h"
#include "Blueprint/UserWidget.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ANetTPSCDCharacter

ANetTPSCDCharacter::ANetTPSCDCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bUseControllerDesiredRotation= true;
	GetCharacterMovement()->bOrientRotationToMovement = false; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 130.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	CameraBoom->SetRelativeLocation(FVector(0, 40, 60));

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// �� ������Ʈ�� �����ؼ� GetMesh�� GunPoint�� ���̰�ʹ�.
	handComp = CreateDefaultSubobject<USceneComponent>(TEXT("handComp"));
	handComp->SetupAttachment(GetMesh(), TEXT("GunPoint"));
	handComp->SetRelativeLocationAndRotation(
		FVector(-16.117320f, 2.606926f, 3.561379f),
		FRotator(17.690681f, 83.344357f,  9.577745));

	//������ hpUIComp ������Ʈ�� �߰��ϰ�ʹ�.
	hpUIComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("hpUIComp"));
	hpUIComp->SetupAttachment(RootComponent);
}

void ANetTPSCDCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//�¾ �� hpUI�� ��������ʹ�.
	hpUI = Cast<UHPbarWidget>(hpUIComp->GetWidget());

	initUI();
	
	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void ANetTPSCDCharacter::PickupPistol(const FInputActionValue& Value)
{
	if (bHasPistol || isReload)
		return;

	// ����� ���� �˻��ؼ� 
	TArray<struct FOverlapResult> OutOverlaps;
	FCollisionObjectQueryParams ObjectQueryParams(FCollisionObjectQueryParams::InitType::AllObjects);

	bool bHits = GetWorld()->OverlapMultiByObjectType(
		OutOverlaps,
		GetActorLocation(),
		FQuat::Identity,
		ObjectQueryParams,
		FCollisionShape::MakeSphere(findPistolRadius));

	// ���� �˻��� ��� �ִٸ�
	if (bHits)
	{
		// ��ü �˻��ؼ�
		for (auto result : OutOverlaps)
		{
			// ���� ������ �̸��� BP_Pistol�� ���ԵǾ��ִٸ�
			if (result.GetActor()->GetActorNameOrLabel().Contains(TEXT("BP_Pistol")))
			{
				// �װ��� grabPistol�� �ϰ�ʹ�.
				grabPistol = result.GetActor();
				// �ݺ��� �׸��ϰ�ʹ�.
				break;
			}
		}
	}

	// grabPistol�� nullptr�� �ƴ϶�� �տ� ���̰�ʹ�.
	if (grabPistol)
	{
		AttachPistol(grabPistol);
		grabPistol->SetOwner(this);
		bHasPistol = true;
		isReload = false;
		mainUI->SetActiveCrosshair(true);
	}
}

void ANetTPSCDCharacter::DropPistol(const FInputActionValue& Value)
{
	if (false == bHasPistol || isReload)
		return;

	bHasPistol = false;
	DetachPistol(grabPistol);
	mainUI->SetActiveCrosshair(false);
}

void ANetTPSCDCharacter::AttachPistol(const AActor* pistol)
{
	// pistol�� staticmeshcomponent�� ��������ʹ�.
	auto mesh = pistol->GetComponentByClass<UStaticMeshComponent>();
	// pistol ������ ����ʹ�.
	mesh->SetSimulatePhysics(false);
	// hand�� ���̰�ʹ�.
	mesh->AttachToComponent(handComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
}

void ANetTPSCDCharacter::DetachPistol(const AActor* pistol)
{
	if (nullptr == grabPistol)
		return;

	// pistol�� staticmeshcomponent�� ��������ʹ�.
	auto mesh = pistol->GetComponentByClass<UStaticMeshComponent>();
	// pistol ������ �Ѱ�ʹ�.
	mesh->SetSimulatePhysics(true);
	// hand���� ����ʹ�.
	mesh->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);

	grabPistol->SetOwner(nullptr);
	grabPistol = nullptr;
}

void ANetTPSCDCharacter::Fire(const FInputActionValue& Value)
{
	if (false == bHasPistol || nullptr == grabPistol)
		return;

	// bulletCount�� 0�� ���϶�� �ٷ� �Լ� ����
	if (bulletCount <= 0)
	{
		return;
	}

	if (isReload)
		return;

	//1�� �����ϰ�
	bulletCount--;
	//�Ѿ� UI�� �����ϰ� �ʹ�.
	if(mainUI)
	{
		mainUI->RemoveBulletUI(bulletCount);
	}

	//UNetPlayerAnimInstance::PlayerFireAnimtation ȣ���ϰ� �ʹ�.
	//1.UNetPlayerAnimInstance ������
	auto anim = Cast<UNetPlayerAnimInstance>(GetMesh()->GetAnimInstance());
	//2.PlayerFireAnimtation ������
		anim->PlayFireAnimation();

	// - ī�޶� ��ġ���� ī�޶� �չ�������
	FHitResult OutHit;
	FVector Start = FollowCamera->GetComponentLocation();
	FVector End = Start + FollowCamera->GetForwardVector() * 100000;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECollisionChannel::ECC_Visibility, Params);

	//���� �ε������� �ִٸ� �װ��� ���� VFX�� ��ġ�ϰ� �ʹ�.
	if(bHit)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionExplosionVFXFactory, OutHit.ImpactPoint);
		//�װ��� ����VFX ��ġ�ϰ� �ʹ�.

		//���� �ε��� ������ ANetTPCcharecter ���
		//takeDamage�� �������� 1�� �ְ� �ʹٴ�
		auto otherPlayer = Cast<ANetTPSCDCharacter>(OutHit.GetActor());

		if (otherPlayer)
		{
			otherPlayer->TakeDamage(1);
		}
	
	}

}

void ANetTPSCDCharacter::Reload(const FInputActionValue& Value)
{
	if (isReload)
		return;

	isReload = true;

	//���ε� �ִϸ��̼��� ���
	auto anim = Cast<UNetPlayerAnimInstance>(GetMesh()->GetAnimInstance());
	anim->PlayReloadAnimation();
}

void ANetTPSCDCharacter::InitAmmo()
{
	bulletCount = maxBulletCount;
	if (mainUI)
	{
		mainUI->ReloadBulletUI(maxBulletCount);
	}
	isReload = false;
}

int32 ANetTPSCDCharacter::GetHP()
{
	return hp;
}

void ANetTPSCDCharacter::SetHP(int32 value)
{
	hp = value;
	if(mainUI) // ����
	{
		mainUI->hp = static_cast<float>(hp) / maxHP;   //(float)hp
	}else // �ϲ�
	{
		hpUI->hp = static_cast<float>(hp) / maxHP;   //(float)hp
	}
}

void ANetTPSCDCharacter::TakeDamage(int32 damage)
{
	UE_LOG(LogTemp, Warning, TEXT("ANetTPSCDCharacter::TakeDamage"));

	//������ ��ŭ ü���� �����ϰ� �ʹ�.
	int32 newHP = GetHP() - damage;
	newHP = FMath::Clamp(newHP, 0, maxHP);
	//int newHP = FMath::Clamp<int32>(GetHP() - damage, 0 , maxHP);  ���ΰ�
	SetHP(newHP);
}

void ANetTPSCDCharacter::initUI()
{
	//��Ʈ�ѷ��� playerController��� ����
	//��, main UI�� �������� �ʰڴ�.
	auto pc = Cast<APlayerController>(GetController());
	if (nullptr == pc)
		return;

	UE_LOG(LogTemp, Warning, TEXT("ANetTPSCDCharacter::BeginPlay"));

	//���� ��Ʈ�ѷ��� PlayerController��� ����.

	//MainUI�� ȭ�鿡 ���̰� �ϰ� �ʹ�.

	mainUI = Cast<UMainUI>(CreateWidget(GetWorld(), mainUIFactory));
	//mainUI = CreateWidget<UMainUI>(GetWorld(), mainUIFactory);
	mainUI->AddToViewport();
	//beginPlay���� MainUI�� �����ؼ� ����ϰ� �ʹ�.
	mainUI->SetActiveCrosshair(false);
	// �켱 ó���� �Ⱥ��̰� 

	//�Ѿ� UI�� �ִ� �Ѿ� ������ŭ �������ְ� �ʹ�.
	for (int32 i = 0; i < maxBulletCount; i++)
	{
		mainUI->AddBulletUI();
	}
}


//////////////////////////////////////////////////////////////////////////
// Input

void ANetTPSCDCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ANetTPSCDCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ANetTPSCDCharacter::Look);

		EnhancedInputComponent->BindAction(PickupPistolAction, ETriggerEvent::Started, this, &ANetTPSCDCharacter::PickupPistol);

		EnhancedInputComponent->BindAction(DropPistolAction, ETriggerEvent::Started, this, &ANetTPSCDCharacter::DropPistol);

		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &ANetTPSCDCharacter::Fire);

		EnhancedInputComponent->BindAction(RelodadAction, ETriggerEvent::Started, this, &ANetTPSCDCharacter::Reload);

	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ANetTPSCDCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ANetTPSCDCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}