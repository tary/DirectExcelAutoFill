#pragma once
#include "CoreMinimal.h"
#include "ExcelQueryProvider.h"


class FExcelAutoFillTool : public IExcelQueryExecutor, public FGCObject, public TSharedFromThis<FExcelAutoFillTool>
{
public:
	void OnRegister();
	void OnUnRegister();

	virtual FString GetReferencerName() const override
	{
		return TEXT("FExcelAutoFillTool");
	}

	virtual void AddReferencedObjects( FReferenceCollector& Collector ) override;
	void LoadTables();
	void RefreshOnObject(UObject* Object, FProperty* InProperty);
	static bool IsSupportedActor(AActor* Object);
	virtual FExcelQueryResultData ProcedureQuery(const FExcelQueryData& QueryData) override;
protected:
	void OnAutoRefreshStatusChanged(class IConsoleVariable* Var);
	void OnSettingChanged(FName PropertyChangedEvent);
	void OnPostEngineInit();
	void OnObjectPropertyChanged(UObject* Object, FPropertyChangedEvent& Event);

	static FName ExcelAutoWatchName;
	static FName ExcelAutoRefreshName;
	
	TMap<FName, class UExcelWorksheet*> AllLoadedSheet;
	TMap<FName, class UExcelWorkbook*> AllLoadedWorkbook;


	FDelegateHandle ObjectPropChangedHandle;
	FDelegateHandle SettingChangedHandle;
	FDelegateHandle PostEngineInitHandle;
	bool bHasEngineInit = false;

	TObjectPtr<class UExcelQueryTask> QueryTask;
};

