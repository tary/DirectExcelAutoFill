#pragma once
#include "CoreMinimal.h"


class FExcelAutoFillTool : public FGCObject
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
	void RefreshOnObject(UObject* Object, FName ChangedByProp);
	static bool IsSupportedActor(UObject* Object);
protected:
	void OnSettingChanged(FName PropertyChangedEvent);
	void OnPostEngineInit();
	void OnObjectPropertyChanged(UObject* Object, FPropertyChangedEvent& Event);

	TMap<FName, class UExcelWorksheet*> AllLoadedSheet;
	TMap<FName, class UExcelWorkbook*> AllLoadedWorkbook;


	FDelegateHandle ObjectPropChangedHandle;
	FDelegateHandle SettingChangedHandle;
	FDelegateHandle PostEngineInitHandle;
	bool bHasEngineInit = false;
};

