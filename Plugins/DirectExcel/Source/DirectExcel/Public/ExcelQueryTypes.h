#pragma once

#include "CoreMinimal.h"
#include "ExcelQueryTypes.generated.h"

USTRUCT(BlueprintType)
struct FExcelQueryKey
{
	GENERATED_BODY()

	explicit FExcelQueryKey(FName InKeyName, FName InTableName = NAME_None)
	 : TableName(InTableName), KeyName(InKeyName)
	{		
	}

	FExcelQueryKey() { }

	bool operator==(const FExcelQueryKey& OtherKey) const
	{
		return TableName == OtherKey.TableName && KeyName == OtherKey.KeyName;
	}

	bool operator!=(const FExcelQueryKey& OtherKey) const
	{
		return !(*this == OtherKey);
	}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName TableName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName KeyName;
};

FORCEINLINE uint32 GetTypeHash(const FExcelQueryKey& Key)
{
	return ::GetTypeHash(Key.KeyName) + ::GetTypeHash(Key.TableName);
}

USTRUCT(BlueprintType)
struct FExcelQueryInfo
{
	GENERATED_BODY()

	FExcelQueryInfo()
		: TableName(NAME_None) {}
	
	FExcelQueryInfo(FName InTableName)
		: TableName(InTableName) {}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName TableName;	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FExcelQueryKey Key;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> Column;
};

USTRUCT(BlueprintType)
struct FExcelQueryData
{
	GENERATED_BODY()

	void AddDefaultValue(const FString& InValue, FName InKeyName, FName InTableName = NAME_None)
	{
		FExcelQueryKey Key(InKeyName, InTableName);
		DefaultData.Add(MoveTemp(Key), InValue);
	}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FExcelQueryKey, FString> DefaultData;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FExcelQueryInfo> QueryList;	
};

USTRUCT(BlueprintType)
struct FExcelQueryResultData
{
	GENERATED_BODY()

	FString GetValue(FName InKeyName, FName InTableName = NAME_None) const
	{
		const FExcelQueryKey Key(InKeyName, InTableName);
		if (Data.Contains(Key))
		{
			return Data[Key];
		}
		return FString();
	}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FExcelQueryKey, FString> Data;
};