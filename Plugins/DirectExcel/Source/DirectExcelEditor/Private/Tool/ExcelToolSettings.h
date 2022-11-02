#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "ExcelToolSettings.generated.h"


USTRUCT(BlueprintType)
struct FExcelTableConfig
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	FName TableName;
	UPROPERTY(EditAnywhere)
	int32 RowDataStartFrom = 6;
	UPROPERTY(EditAnywhere)
	int32 HeaderStartRowIndex = 2;
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnExcelToolSettingsModify, FName);

UCLASS(config=EditorPerProjectUserSettings, meta = (DisplayName = "Tool-Excel导出配置(User)"))
class UExcelToolUserSettings : public UDeveloperSettings
{
	GENERATED_UCLASS_BODY()
public:
	UFUNCTION(BlueprintCallable)
	static UExcelToolUserSettings* GetSettings();

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
	//如: D:\WorkSpace\Tools\Table
	UPROPERTY(config, EditAnywhere, meta = (DisplayName = "Excel表格路径"))
	FDirectoryPath ExcelSourcePath;
};


UCLASS(config=Editor, defaultconfig, meta = (DisplayName = "Tool-Excel导出配置"))
class UExcelToolSettings : public UDeveloperSettings
{
	GENERATED_UCLASS_BODY()
public:
	UFUNCTION(BlueprintCallable)
	static UExcelToolSettings* GetSettings();

#if WITH_EDITOR
	static FOnExcelToolSettingsModify OnExcelToolSettingsModify;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
	//区域表格子大小
	UPROPERTY(config, EditAnywhere, Category = Custom, meta = (DisplayName = "表格加载白名单"))
	TArray<FExcelTableConfig> DataTable;
	UPROPERTY(config, EditAnywhere, Category = Custom, meta = (DisplayName = "忽略表"))
	TArray<FName> IgnoreSheetName;
};
