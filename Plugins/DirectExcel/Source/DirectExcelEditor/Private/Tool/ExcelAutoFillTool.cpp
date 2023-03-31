#include "ExcelAutoFillTool.h"

#include "DirectExcelLibrary.h"
#include "ExcelQueryTask.h"
#include "ExcelQueryTypes.h"
#include "ExcelToolSettings.h"
#include "ExcelWorkbook.h"

PRAGMA_DISABLE_OPTIMIZATION

bool bUseUseExcelAutoFill = false;
static FAutoConsoleVariableRef CVarUseExcelAutoFill(
	TEXT("d.ExcelAutoFill.AutoRefresh"),
	bUseUseExcelAutoFill,
	TEXT("打开或关闭属性修改时自动根据属性刷新Excel最新数据, 可能会影响编辑器性能")
);

void FExcelAutoFillTool::OnRegister()
{
	CVarUseExcelAutoFill->SetOnChangedCallback(FConsoleVariableDelegate::CreateSP(this, &FExcelAutoFillTool::OnAutoRefreshStatusChanged));
	IModularFeatures::Get().RegisterModularFeature(FeatureName, this);
	if (bUseUseExcelAutoFill)
	{		
		ObjectPropChangedHandle = FCoreUObjectDelegates::OnObjectPropertyChanged.AddRaw(this, &FExcelAutoFillTool::OnObjectPropertyChanged);
	}
	
	SettingChangedHandle = UExcelToolSettings::OnExcelToolSettingsModify.AddRaw(this, &FExcelAutoFillTool::OnSettingChanged);

	PostEngineInitHandle = FCoreDelegates::OnPostEngineInit.AddRaw(this, &FExcelAutoFillTool::OnPostEngineInit);
	
	QueryTask = NewObject<UExcelQueryTask>();
	QueryTask->AddToRoot();
}

void FExcelAutoFillTool::OnUnRegister()
{
	CVarUseExcelAutoFill->OnChangedDelegate().RemoveAll(this);
	IModularFeatures::Get().UnregisterModularFeature(FeatureName, this);
	if (IsValid(QueryTask))
	{
		QueryTask->RemoveFromRoot();
		QueryTask->MarkAsGarbage();
		QueryTask = nullptr;
	}
	AllLoadedSheet.Reset();
	AllLoadedWorkbook.Reset();
	FCoreUObjectDelegates::OnObjectPropertyChanged.Remove(ObjectPropChangedHandle);
	ObjectPropChangedHandle.Reset();
	UExcelToolSettings::OnExcelToolSettingsModify.Remove(SettingChangedHandle);
	SettingChangedHandle.Reset();
	FCoreDelegates::OnPostEngineInit.Remove(PostEngineInitHandle);
	bHasEngineInit = false;
}

void FExcelAutoFillTool::AddReferencedObjects(FReferenceCollector& Collector)
{
	TArray<UExcelWorksheet*> AllRefSheet;
	AllLoadedSheet.GenerateValueArray(AllRefSheet);		
	TArray<UExcelWorkbook*> AllRefWorkBook;
	AllLoadedWorkbook.GenerateValueArray(AllRefWorkBook);

	if (!AllRefSheet.IsEmpty())
	{
		Collector.AddReferencedObjects(AllRefSheet);
	}
	if (!AllRefWorkBook.IsEmpty())
	{
		Collector.AddReferencedObjects(AllRefWorkBook);
	}
	
	if (QueryTask)
	{
	    Collector.AddReferencedObject(QueryTask);
	}
}

void FExcelAutoFillTool::OnAutoRefreshStatusChanged(IConsoleVariable* Var)
{
	if (bUseUseExcelAutoFill)
	{		
		ObjectPropChangedHandle = FCoreUObjectDelegates::OnObjectPropertyChanged.AddRaw(this, &FExcelAutoFillTool::OnObjectPropertyChanged);
	}
	else if (ObjectPropChangedHandle.IsValid())
	{
		FCoreUObjectDelegates::OnObjectPropertyChanged.Remove(ObjectPropChangedHandle);
		ObjectPropChangedHandle.Reset();
	}
}

void FExcelAutoFillTool::OnSettingChanged(FName PropertyChangedEvent)
{
	LoadTables();
}

void FExcelAutoFillTool::OnPostEngineInit()
{
	bHasEngineInit = true;
	LoadTables();
}

void FExcelAutoFillTool::LoadTables()
{
	if (!bHasEngineInit)
	{
		return;
	}
	const UExcelToolUserSettings* UserSetting = UExcelToolUserSettings::GetSettings();
	const UExcelToolSettings* Setting = UExcelToolSettings::GetSettings();
	if (UserSetting && !UserSetting->ExcelSourcePath.Path.IsEmpty() && FPaths::DirectoryExists(UserSetting->ExcelSourcePath.Path) &&
		Setting && !Setting->DataTable.IsEmpty())
	{
		const FString PostFix = TEXT(".xlsx");
		for (const FExcelTableConfig& TableConfig : Setting->DataTable)
		{
			const FString TableFileName = TableConfig.TableName.ToString();
			const FString ExcelFilePath = FPaths::Combine(UserSetting->ExcelSourcePath.Path, TableFileName) + PostFix;

			UExcelWorkbook* SourceWorkbook = nullptr;
			if (AllLoadedWorkbook.Contains(TableConfig.TableName))
			{
				SourceWorkbook = AllLoadedWorkbook[TableConfig.TableName];
				if (SourceWorkbook)
				{
					SourceWorkbook->Reload();
				}
			}
			else
			{
				SourceWorkbook = UDirectExcelLibrary::LoadExcel(ExcelFilePath);
				if (SourceWorkbook)
				{
					AllLoadedWorkbook.Add(TableConfig.TableName, SourceWorkbook);
				}
			}
			
			if (!SourceWorkbook)
			{
				UE_LOG(LogTemp, Error, TEXT("加载Excel表格失败:%s"), *ExcelFilePath);
				continue;
			}

			const TArray<UExcelWorksheet*>& AllSheets = SourceWorkbook->AllSheets();
			for (UExcelWorksheet* Sheet : AllSheets)
			{
				FString TitleName = Sheet->Title();
				
				if (TitleName.Equals(TEXT("data")))
				{
					TitleName = TableFileName;
				}
				
				FName TitleFName = *TitleName;
				if (Setting->IgnoreSheetName.Contains(TitleFName))
				{
					continue;
				}
				
				FExcelTableHeader Header;
				Header.DataRowStartIndex = TableConfig.RowDataStartFrom;
				Header.ColumnHeaderStartRowIndex = TableConfig.HeaderStartRowIndex;
				Sheet->SetTableHeader(Header);
				AllLoadedSheet.Add(TitleFName, Sheet);
			}
		}
	}
}

void FExcelAutoFillTool::RefreshOnObject(UObject* Object, FProperty* InProperty)
{
	if (!Object || !QueryTask)
	{
		return;
	}
	
	UFunction* Function = nullptr;
	if (UClass* Cls = Object->GetClass())
	{
		const FName FuncName = GET_FUNCTION_NAME_CHECKED(UExcelQueryTask, ExcelAutoFillFunc);
		Function = Cls->FindFunctionByName(FuncName);
	}

	if (!Function)
	{
		//todo log
		return;
	}

	if (InProperty && !Function->HasMetaData(InProperty->GetFName()))
	{
		return;
	}

	//const auto QueryFunc = FExcelQueryDelegate::CreateSP(this, &FExcelAutoFillTool::ProcedureQuery);

	const FName PropName = InProperty ? InProperty->GetFName() : NAME_None;
	QueryTask->ProcessQueryFunc(Object, Function, PropName);
}

FExcelQueryResultData FExcelAutoFillTool::ProcedureQuery(const FExcelQueryData& QueryData)
{
	TMap<FExcelQueryKey, FString> AllResult = QueryData.DefaultData;
	for (const FExcelQueryInfo& Query : QueryData.QueryList)
	{
		if (!AllResult.Contains(Query.Key))
		{
			continue;
		}
		const FString& CurKey = AllResult[Query.Key];
		if (!AllLoadedSheet.Contains(Query.TableName))
		{
			continue;
		}

		const UExcelWorksheet* LoadedSheet = AllLoadedSheet[Query.TableName];

		const UExcelCell* KeyCell = LoadedSheet->FindStringAtColumn(1, CurKey);
		if (!KeyCell)
		{
			continue;
		}
		
		for (const FName& ColumnName : Query.Column)
		{
			const int32 TargetColumIdx = LoadedSheet->FindColumnIndex(ColumnName);
			if (TargetColumIdx < 0)
			{
				continue;
			}

			
			const int32 RowIndex = KeyCell->Row();
			FExcelCellReference CellReference;
			CellReference.Column = TargetColumIdx;
			CellReference.Row = RowIndex;
			FString Result = LoadedSheet->ToString(CellReference);
			AllResult.Add(FExcelQueryKey(ColumnName, Query.TableName), MoveTemp(Result));
		}			
	}

	FExcelQueryResultData ResultData;
	ResultData.Data = MoveTemp(AllResult);
	return MoveTemp(ResultData);
}

bool FExcelAutoFillTool::IsSupportedActor(AActor* Object)
{
	return Object && !Object->HasAnyFlags(RF_ClassDefaultObject | RF_ArchetypeObject) && Object->Tags.Contains(ExcelAutoRefreshName);
}

FName FExcelAutoFillTool::ExcelAutoWatchName = TEXT("ExcelAutoWatch");
FName FExcelAutoFillTool::ExcelAutoRefreshName = TEXT("ExcelAutoRefresh");
void FExcelAutoFillTool::OnObjectPropertyChanged(UObject* Object, FPropertyChangedEvent& Event)
{
	if (Event.Property && Object)
	{
		RefreshOnObject(Object, Event.Property);
	}	
}
PRAGMA_ENABLE_OPTIMIZATION
