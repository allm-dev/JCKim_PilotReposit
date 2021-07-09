// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameInstance.h"

UMyGameInstance::UMyGameInstance()
{
	FString CharacterDataPath = TEXT("/Game/GameData/ABCharacterData.ABCharacterData");
	static ConstructorHelpers::FObjectFinder<UDataTable> DT_MYCHARACTER(*CharacterDataPath);

	//character data load check
	MYCHECK(DT_MYCHARACTER.Succeeded());
	MyCharacterTable = DT_MYCHARACTER.Object;
	//loaded data validation
	MYCHECK(MyCharacterTable->GetRowMap().Num() >0);
}

void UMyGameInstance::Init()
{
	Super::Init();
}

FMyCharacterData* UMyGameInstance::GetMyCharacterData(int32 Level)
{
	return MyCharacterTable->FindRow<FMyCharacterData>(*FString::FromInt(Level), TEXT(""));
}
