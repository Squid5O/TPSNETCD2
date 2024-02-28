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
}
