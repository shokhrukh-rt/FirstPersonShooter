// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "FirstPersonShooterHUD.generated.h"

UCLASS()
class AFirstPersonShooterHUD : public AHUD
{
	GENERATED_BODY()

public:
	AFirstPersonShooterHUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;

};

