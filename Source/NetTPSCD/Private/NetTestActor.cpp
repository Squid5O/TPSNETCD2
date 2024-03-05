// Fill out your copyright notice in the Description page of Project Settings.


#include "NetTestActor.h"

// Sets default values
ANetTestActor::ANetTestActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	meshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("meshComp"));
	meshComp->SetRelativeScale3D(FVector(0.5f));

	bReplicates =true;

}

// Called when the game starts or when spawned
void ANetTestActor::BeginPlay()
{
	Super::BeginPlay();

	// 회전
	
}

// Called every frame
void ANetTestActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	PrintNetLog();

}

void ANetTestActor::PrintNetLog()
{
	//UE_LOG(LogTemp, Warning, TEXT("11"));
	//오너가 있는가?
	FString owner = GetOwner() ? GetOwner()->GetName() : TEXT("NoOwner");
	//NetConnection이 있는가?
	FString conn = GetNetConnection() ? TEXT("ValidCon") : TEXT("InvalidCon");
	//LocalRole
	FString localRole = UEnum::GetValueAsString<ENetRole>(GetLocalRole());
	//RemoteRole
	FString remoteRole = UEnum::GetValueAsString<ENetRole>(GetRemoteRole());

	//if(IsLocal)

	FString str = FString::Printf(TEXT("Owner : %s\nConnecetion : %s\nlocalRole : %s\nremoteRole : %s"
	), *owner, *conn, *localRole, *remoteRole);

	FVector loc = GetActorLocation() + FVector(0, 0, 50);
	DrawDebugString(GetWorld(), loc, str, nullptr, FColor::White, 0, true, 1.5f);

}