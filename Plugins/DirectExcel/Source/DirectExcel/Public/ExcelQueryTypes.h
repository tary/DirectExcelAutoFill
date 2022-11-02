#pragma once

#include "CoreMinimal.h"
#include "ExcelQueryTypes.generated.h"

USTRUCT(BlueprintType)
struct FExcelQueryInfo
{
	GENERATED_BODY()

	FExcelQueryInfo()
		: TableName(NAME_None) {}
	
	FExcelQueryInfo(FName InTableName)
		: TableName(InTableName) {}
	
	FExcelQueryInfo(FName InTableName, FName Key, const TMap<FName, FName>& InColumnQuest)
		: TableName(InTableName), KeyName(Key), ColumnQuest(InColumnQuest) {}

	FExcelQueryInfo& SetTableName(FName InTableName)
	{
		TableName = InTableName;
		return *this;
	}
	FExcelQueryInfo& SetKeyName(FName InKeyName)
	{
		KeyName = InKeyName;
		return *this;
	}
	FExcelQueryInfo& AddColumnQuest(FName InColumnName, FName InResultKey)
	{
		ColumnQuest.Add(InColumnName, InResultKey);
		return *this;
	}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName TableName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName KeyName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FName, FName> ColumnQuest;
};

USTRUCT(BlueprintType)
struct FExcelQueryRequest
{
	GENERATED_BODY()

	FExcelQueryRequest() : TaskID(NAME_None) {}
	
	FExcelQueryRequest(FName InTaskID) : TaskID(InTaskID) {}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName TaskID;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FExcelQueryInfo> QueryList;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FName, FString> Result;
};
