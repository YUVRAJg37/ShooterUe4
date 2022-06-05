#pragma once

UENUM(BlueprintType)
enum class EAmmoType : uint8
{
	EAT_9mm UMETA(DisplayName = "9mm"),
	EAT_Ar UMETA(DisplayName = "AssaultRidle"),

	EAT_MAX UMETA(DisplayName = "Max")
};