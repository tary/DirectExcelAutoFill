#include "ExcelAutoFillTool.h"

#include "DirectExcelLibrary.h"
#include "ExcelAutoConfigInterface.h"
#include "ExcelQueryTypes.h"
#include "ExcelToolSettings.h"
#include "ExcelWorkbook.h"

PRAGMA_DISABLE_OPTIMIZATION


void FExcelAutoFillTool::OnRegister()
{
	ObjectPropChangedHandle = FCoreUObjectDelegates::OnObjectPropertyChanged.AddRaw(this, &FExcelAutoFillTool::OnObjectPropertyChanged);
	SettingChangedHandle = UExcelToolSettings::OnExcelToolSettingsModify.AddRaw(this, &FExcelAutoFillTool::OnSettingChanged);

	PostEngineInitHandle = FCoreDelegates::OnPostEngineInit.AddRaw(this, &FExcelAutoFillTool::OnPostEngineInit);
}

void FExcelAutoFillTool::OnUnRegister()
{
	AllLoadedSheet.Reset();
	AllLoadedWorkbook.Reset();
	FCoreUObjectDelegates::OnObjectPropertyChanged.Remove(ObjectPropChangedHandle);
	UExcelToolSettings::OnExcelToolSettingsModify.Remove(SettingChangedHandle);
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

void FExcelAutoFillTool::RefreshOnObject(UObject* Object, FName ChangedByProp)
{
	if (!Object)
	{
		return;
	}
	
	static FExcelQueryRequest CellQueryRequest;
	if (!Object->GetClass()->ImplementsInterface(UExcelAutoConfigInterface::StaticClass()))
	{
		return;
	}

	CellQueryRequest.Result.Reset();
	CellQueryRequest.QueryList.Reset();
	if (!IExcelAutoConfigInterface::Execute_GetExcelAutoCompleteData(Object, ChangedByProp, CellQueryRequest))
	{
		return;
	}

	for (FExcelQueryInfo& Query : CellQueryRequest.QueryList)
	{
		if (!CellQueryRequest.Result.Contains(Query.KeyName))
		{
			continue;
		}
		const FString& CurKey = CellQueryRequest.Result[Query.KeyName];
		if (!AllLoadedSheet.Contains(Query.TableName))
		{
			continue;
		}

		const UExcelWorksheet* LoadedSheet = AllLoadedSheet[Query.TableName];
		for (const TPair<FName, FName>& ColumnIT : Query.ColumnQuest)
		{
			const FName ColumnName = ColumnIT.Key;
			const FName ColumnKey = ColumnIT.Value;
			const int32 TargetColumIdx = LoadedSheet->FindColumnIndex(ColumnName);
			if (TargetColumIdx < 0)
			{
				continue;
			}

			const UExcelCell* KeyCell = LoadedSheet->FindStringAtColumn(1, CurKey);
			if (!KeyCell)
			{
				continue;
			}
			const int32 RowIndex = KeyCell->Row();
			FExcelCellReference CellReference;
			CellReference.Column = TargetColumIdx;
			CellReference.Row = RowIndex;
			FString Result = LoadedSheet->ToString(CellReference);
			CellQueryRequest.Result.Add(ColumnKey, MoveTemp(Result));
		}			
	}

	if (CellQueryRequest.Result.IsEmpty())
	{
		return;
	}
	IExcelAutoConfigInterface::Execute_SetExcelAutoCompleteData(Object, ChangedByProp, CellQueryRequest);
}

bool FExcelAutoFillTool::IsSupportedActor(UObject* Object)
{
	return Object && Object->GetClass()->ImplementsInterface(UExcelAutoConfigInterface::StaticClass());
}

void FExcelAutoFillTool::OnObjectPropertyChanged(UObject* Object, FPropertyChangedEvent& Event)
{
	if (Event.Property && Object)
	{
		RefreshOnObject(Object, Event.Property->GetFName());
	}	
}
PRAGMA_ENABLE_OPTIMIZATION
