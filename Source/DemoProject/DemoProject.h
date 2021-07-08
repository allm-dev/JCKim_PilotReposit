// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EngineMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN (DemoProject, Log, All);
// caller 정보 FString
#define ABLOG_CALLINFO (FString(__FUNCTION__) + TEXT("(") + FString::FromInt(__LINE__) + TEXT(")"))
//caller info 로깅 매크로
#define ABLOG_S(Verbosity) UE_LOG(DemoProject, Verbosity, TEXT("%s"), *ABLOG_CALLINFO)
// caller info + 내맘대로 로깅
#define ABLOG(Verbosity, Format, ...) UE_LOG(DemoProject, Verbosity, TEXT("%s %s"), *ABLOG_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))

// check assertion define macro func
#define MYCHECK(Expr, ...) {if(!(Expr)) {ABLOG(Error, TEXT("ASSERTION : %s"), TEXT("'"#Expr"'")); return __VA_ARGS__; } }