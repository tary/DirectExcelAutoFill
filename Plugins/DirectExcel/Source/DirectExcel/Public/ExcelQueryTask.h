#pragma once

#include "CoreMinimal.h"
#include "ExcelQueryTypes.h"
#include "ExcelQueryTask.generated.h"


DECLARE_DYNAMIC_DELEGATE_RetVal_OneParam(FExcelQueryResultData, FExcelQueryDynamicDelegate, const FExcelQueryData&, InitData);

UCLASS()
class DIRECTEXCEL_API UExcelQueryTask : public UObject
{
	GENERATED_BODY()
public:
#if WITH_EDITOR
	//example
	//MetaData内的名字为关注的属性
	UFUNCTION(meta=(ExampleProperty))
	void ExcelAutoFillFunc(FName ChangedPropName);
#endif	
	void ProcessQueryFunc(UObject* InObject, UFunction* InFunc, FName PropName);
};
