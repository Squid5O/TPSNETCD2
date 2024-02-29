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
};
