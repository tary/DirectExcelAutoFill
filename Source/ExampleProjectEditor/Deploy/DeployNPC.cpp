#include "DeployNPC.h"


#if WITH_EDITOR
bool ADeployNPC::GetExcelAutoCompleteData_Implementation(FName ChangedPropName, FExcelQueryRequest& Request)
{	
	if ((!ChangedPropName.IsNone() && ChangedPropName != GET_MEMBER_NAME_CHECKED(ANPCDeployNode, MonsterID)) || MonsterID <= 0)
	{
		return false;
	}

	static FExcelQueryInfo MonsterInfo = FExcelQueryInfo(TEXT("Monster"))
	.SetKeyName(TEXT("MonsterID"))
	.AddColumnQuest(TEXT("AppearanceID"), TEXT("Monster.AppearanceID"))
	.AddColumnQuest(TEXT("ModelScaling"), TEXT("ModelScaling"));

	static FExcelQueryInfo AppearanceInfo = FExcelQueryInfo(TEXT("Appearance"))
	.SetKeyName(TEXT("Monster.AppearanceID"))
	.AddColumnQuest(TEXT("BasicAppearance"), GET_MEMBER_NAME_CHECKED(ADeployNPC, BasicApp))
	.AddColumnQuest(TEXT("CustomAppearance"), GET_MEMBER_NAME_CHECKED(ADeployNPC, CustomApp));

	Request = FExcelQueryRequest(TEXT("DeployNPC"));
	Request.Result.Add(TEXT("MonsterID"), FString::FromInt(MonsterID));
	Request.QueryList.Add(MonsterInfo);
	Request.QueryList.Add(AppearanceInfo);
	return true;
}

void ADeployNPC::SetExcelAutoCompleteData_Implementation(FName ChangedPropName, const FExcelQueryRequest& Request)
{
	if (Request.Result.Contains(GET_MEMBER_NAME_CHECKED(ADeployNPC, BasicApp)))
	{
		const FSoftObjectPath ValuePath = Request.Result[GET_MEMBER_NAME_CHECKED(ADeployNPC, BasicApp)];
		BasicApp = Cast<UBasicAppearanceAsset>(ValuePath.ResolveObject());
		UpdateAppearanceConfig();
	}

	if (Request.Result.Contains(GET_MEMBER_NAME_CHECKED(ADeployNPC, CustomApp)))
	{
		const FSoftObjectPath ValuePath = Request.Result[GET_MEMBER_NAME_CHECKED(ADeployNPC, CustomApp)];
		CustomApp = Cast<UCustomAppearanceAsset>(ValuePath.ResolveObject());
	}
		
	if (Request.Result.Contains(TEXT("ModelScaling")))
	{
		const FString ValueInt = Request.Result[TEXT("ModelScaling")];
		if (ValueInt.IsNumeric())
		{
			const float Scale = FCString::Atoi(*ValueInt) * 0.01f;
			SetActorScale3D(FVector(Scale));
		}
	}
}
#endif

void ADeployNPC::UpdateAppearanceConfig() const
{
	//update mesh by asset	
}

