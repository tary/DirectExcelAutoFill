#pragma once

#include "CoreMinimal.h"
#include "DeployNPCNode.h"
#include "ExcelAutoConfigInterface.h"
#include "Appearance/AppearanceConfig.h"
#include "DeployNPC.generated.h"

UCLASS()
class ADeployNPCNode : public AActor
{
public:
	GENERATED_BODY()
	
	UPROPERTY(EditInstanceOnly, Category = Config)
	int32 MonsterID;
};


UCLASS(ClassGroup = ExportConfig, hideCategories = (Actor, Camera, LOD, Cooking, Collision, Replication, Mobile, Input, Rendering, AutoPlayerActivation))
class ADeployNPC : public ADeployNPCNode, public IExcelAutoConfigInterface
{
public:
	GENERATED_BODY()

#if WITH_EDITOR
	virtual bool GetExcelAutoCompleteData_Implementation(FName ChangedPropName, FExcelQueryRequest& Request) override;
	virtual void SetExcelAutoCompleteData_Implementation(FName ChangedPropName, const FExcelQueryRequest& Request) override;
#endif
protected:
	void UpdateAppearanceConfig() const;
	

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = Config, meta = (AllowPrivateAccess = "true", DisplayName = "基础外观(预览)"))
	TObjectPtr<UBasicAppearanceAsset> BasicApp;
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = Config, meta = (AllowPrivateAccess = "true", DisplayName = "自定义外观(预览)"))
	TObjectPtr<UCustomAppearanceAsset> CustomApp;
	
	
	UPROPERTY(Category=Character, VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> Mesh;

};
