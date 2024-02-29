// Fill out your copyright notice in the Description page of Project Settings.


#include "MainUI.h"

#include "NetTPSCDCharacter.h"
#include "Components/Image.h"

void UMainUI::SetActiveCrosshair(bool bActive)
{
	// ���� ũ�ν���� �̹����� bActive�� true ��� ���̰� false ��� �Ⱥ��̰� �ϰ� �ûQ.
	if (bActive) {
		ImageCrosshair->SetVisibility(ESlateVisibility::Visible);
	} else{
		ImageCrosshair->SetVisibility(ESlateVisibility::Hidden);
	}

	//�÷��̾ �¾�� �Ⱥ��̰�
	//�� ������ ���̰�
	//�� ������ �Ⱥ��̰� 
}
