#include "ExcelToolSettings.h"

UExcelToolUserSettings::UExcelToolUserSettings(const FObjectInitializer& ObjectInitializer)
{
	ExcelSourcePath.Path = TEXT("D:\\WorkSpace\\Tools\\Table");
}

UExcelToolUserSettings* UExcelToolUserSettings::GetSettings()
{
	return GetMutableDefault<UExcelToolUserSettings>();
}

UExcelToolSettings* UExcelToolSettings::GetSettings()
{
	return GetMutableDefault<UExcelToolSettings>();
}

UExcelToolSettings::UExcelToolSettings(const FObjectInitializer& ObjectInitializer)
{
	FExcelTableConfig CharacterConfig;
	CharacterConfig.TableName = TEXT("Character");
	DataTable.Add(CharacterConfig);
	FExcelTableConfig AppearanceConfig;
	AppearanceConfig.TableName = TEXT("Appearance");
	DataTable.Add(AppearanceConfig);

	IgnoreSheetName.Add(TEXT("config"));
}

#if WITH_EDITOR
void UExcelToolUserSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	UExcelToolSettings::OnExcelToolSettingsModify.Broadcast(PropertyChangedEvent.GetPropertyName());
}
void UExcelToolSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	UExcelToolSettings::OnExcelToolSettingsModify.Broadcast(PropertyChangedEvent.GetPropertyName());
}
FOnExcelToolSettingsModify UExcelToolSettings::OnExcelToolSettingsModify;
#endif
