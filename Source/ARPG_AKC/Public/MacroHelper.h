// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#define DEBUG_PRINT(Message) \
{ \
if (GEngine) \
{ \
GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::White, Message); \
} \
}

#define TICK_DEBUG_PRINT(Message) \
{ \
if (GEngine) \
{ \
GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::White, Message); \
} \
}