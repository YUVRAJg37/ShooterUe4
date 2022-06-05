// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterPlayerController.h"

#include "Blueprint/UserWidget.h"

AShooterPlayerController::AShooterPlayerController()
{
}

void AShooterPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if(UserWidgetClass)
	{
		UserWidget = CreateWidget<UUserWidget>(this, UserWidgetClass);

		if(UserWidget)
		{
			UserWidget->AddToViewport();
			UserWidget->SetVisibility(ESlateVisibility::Visible);
		}
	}
}
