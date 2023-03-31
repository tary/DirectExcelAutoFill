#include "ExcelQueryProvider.h"
#include "ExcelQueryTask.h"

#if WITH_EDITOR
void UExcelQueryTask::ExcelAutoFillFunc(FName ChangedPropName)
{
	//example property
	int32 ExampleProperty = 0;
		
	// if ((!ChangedPropName.IsNone() && ChangedPropName != GET_MEMBER_NAME_CHECKED(ANPCDeployNode, ExampleProperty)) || ExampleProperty <= 0)
	// {
	// 	return;
	// }
	
	IExcelQueryExecutor* QueryExecutor = static_cast<IExcelQueryExecutor*>(IModularFeatures::Get().GetModularFeatureImplementation(IExcelQueryExecutor::FeatureName, 0));
	if (QueryExecutor)
	{
		static FExcelQueryData QueryData;
		if (QueryData.QueryList.IsEmpty())
		{
			FExcelQueryInfo MonsterInfo = FExcelQueryInfo(TEXT("Character"));
			{
				MonsterInfo.Key = FExcelQueryKey(TEXT("MonsterID"));
				MonsterInfo.Column.AddUnique(TEXT("AppearanceID"));
				MonsterInfo.Column.AddUnique(TEXT("ModelScaling"));
			}
	
			FExcelQueryInfo AppearanceInfo = FExcelQueryInfo(TEXT("Appearance"));
			{
				AppearanceInfo.Key = FExcelQueryKey(TEXT("AppearanceID"), TEXT("Character"));
				AppearanceInfo.Column.AddUnique(TEXT("BasicAppearance"));
				AppearanceInfo.Column.AddUnique(TEXT("CustomAppearance"));
			}
			QueryData.QueryList.Add(MonsterInfo);
			QueryData.QueryList.Add(AppearanceInfo);
		}
		QueryData.AddDefaultValue(FString::FromInt(ExampleProperty), TEXT("MonsterID"));
	
		FExcelQueryResultData ResultData = QueryExecutor->ProcedureQuery(QueryData);
		const FString BasicAppearance = ResultData.GetValue(TEXT("BasicAppearance"), TEXT("Appearance"));
	}
}
#endif

void UExcelQueryTask::ProcessQueryFunc(UObject* InObject, UFunction* InFunc, FName PropName)
{
	struct
	{
		FName ChangedPropName;
	} Param;

	Param.ChangedPropName = PropName;
	
	TGuardValue AutoRestore(GAllowActorScriptExecutionInEditor, true);
	InObject->ProcessEvent(InFunc, &Param);
}
