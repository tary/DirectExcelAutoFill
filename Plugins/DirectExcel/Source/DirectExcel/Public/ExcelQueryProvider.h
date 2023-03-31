// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once
#include "CoreTypes.h"
#include "ExcelQueryTypes.h"
#include "Features/IModularFeatures.h"

class DIRECTEXCEL_API IExcelQueryExecutor : public IModularFeature
{
public:
	static FName FeatureName;
	virtual FExcelQueryResultData ProcedureQuery(const FExcelQueryData& QueryData) = 0;
};
