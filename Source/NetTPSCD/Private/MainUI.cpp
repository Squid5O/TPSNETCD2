// Fill out your copyright notice in the Description page of Project Settings.


#include "MainUI.h"

#include "NetTPSCDCharacter.h"
#include "Components/Image.h"
#include "Components/UniformGridPanel.h"

void UMainUI::SetActiveCrosshair(bool bActive)
{
	// ���� ũ�ν���� �̹����� bActive�� true ��� ���̰� false ��� �Ⱥ��̰� �ϰ� �ûQ.
	if (bActive) {
		ImageCrosshair->SetVisibility(ESlateVisibility::Visible);
		grid_bullet->SetVisibility(ESlateVisibility::Visible);
	} else{
		ImageCrosshair->SetVisibility(ESlateVisibility::Hidden);
		grid_bullet->SetVisibility(ESlateVisibility::Hidden);
	}

	//�÷��̾ �¾�� �Ⱥ��̰�
	//�� ������ ���̰�
	//�� ������ �Ⱥ��̰� 
}

void UMainUI::ReloadBulletUI(int32 MaxBulletCount)
{
		//������ grid_bullet�� �ڽĵ��� ��� �����ϰ�
	grid_bullet->ClearChildren();
	//�ٽ� maxBulletCount��ŭ �����ϰ� �ʹ�.
	for(int32 i = 0; i < MaxBulletCount; i++)
	{
		AddBulletUI();
	}
}

void UMainUI::AddBulletUI()
{
	//�Ѿ� ������ �����
//	CreateWidget(GetWorld(), ) this�� ����
	auto bulletUI = CreateWidget(this, bulletUIFactory);
	//grid�� �ڽ����� ���δ�.
	grid_bullet->AddChildToUniformGrid(bulletUI, 0, grid_bullet->GetChildrenCount());
}

void UMainUI::RemoveBulletUI(int32 index)
{
	//gird�� index��ġ�� �ڽ��� �����Ѵ�.
	grid_bullet->RemoveChildAt(index);
}
