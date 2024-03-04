// Fill out your copyright notice in the Description page of Project Settings.


#include "NetPlayerAnimInstance.h"

#include "NetTPSCDCharacter.h"

void UNetPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	// player�� ����ϰ�ʹ�.
	player = Cast<ANetTPSCDCharacter>(GetOwningActor());
}

void UNetPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (nullptr == player)
		return;

	// speed, direction���� ä���ʹ�.
	const FVector velocity = player->GetVelocity();

	speed = FVector::DotProduct(velocity, player->GetActorForwardVector());

	direction = FVector::DotProduct(velocity, player->GetActorRightVector());

	bHasPistol = player->bHasPistol;

	//player�� Pitch���� �����ͼ� PitchAngle�� �����ϰ� �ûQ.
	 ptichAngle = (player->GetBaseAimRotation().GetNormalized().Pitch) * -1;

	//pitchAngle���� -60~60�ȿ� ���־���
	 ptichAngle = FMath::Clamp(ptichAngle, -60, 60);
}

void UNetPlayerAnimInstance::PlayFireAnimation()
{
	//���� ��� �������� �ѽ�� �ִϸ��̼��� �ϰ� �ʹ�.
	if(bHasPistol && fireMontage)
	{
	Montage_Play(fireMontage);
	}
}

void UNetPlayerAnimInstance::PlayReloadAnimation()
{
	if (bHasPistol && fireMontage)
	{
		Montage_Play(reloadMontage);
	}
}

void UNetPlayerAnimInstance::AnimNotify_OnReloadFinished()
{
	//ĳ���Ϳ��� ������ �Ѿ��� ä���� ��� �ϰ�ʹ�.
	if (nullptr == player)
		return;

	player->InitAmmo();
}
