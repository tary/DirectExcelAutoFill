#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ExcelQueryTypes.h"
#include "ExcelAutoConfigInterface.generated.h"


UINTERFACE(MinimalAPI)
class UExcelAutoConfigInterface : public UInterface
{
	GENERATED_BODY()
};


class DIRECTEXCEL_API IExcelAutoConfigInterface
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintNativeEvent, CallInEditor)
	bool GenerateExcelQueryData(FName ChangedPropName, FExcelQueryRequest& Data);
	UFUNCTION(BlueprintNativeEvent, CallInEditor)
	void SetExcelAutoCompleteData(FName ChangedPropName, const FExcelQueryRequest& Value);

};
