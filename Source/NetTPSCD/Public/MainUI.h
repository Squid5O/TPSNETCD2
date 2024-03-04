// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainUI.generated.h"

/**
 * 
 */
UCLASS()
class NETTPSCD_API UMainUI : public UUserWidget
{
	GENERATED_BODY()

public:
	//ImageCrosshair�� BindWidget �غ�����
	UPROPERTY(EditDefaultsOnly, meta=(BindWidget))
	class UImage* ImageCrosshair;

	void SetActiveCrosshair(bool bActive);

	//�¾ �� �÷��̾ ���� �Ѿ��� �ִ� ������ŭ ǥ���ϰ� �ʹ�.
	//���� ��� �ϳ��� �����ϰ� �ʹ�.
	// ������ �ϸ� �ٽ� �� ä��� �ʹ�.
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UUniformGridPanel* grid_bullet;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UUserWidget> bulletUIFactory;

	void ReloadBulletUI(int32 MaxBulletCount);
	void AddBulletUI();
	void RemoveBulletUI(int32 index);

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	class UProgressBar* bar_hp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float hp = 1.0f;
};
