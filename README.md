# DirectExcelAutoFill
基于[DirectExcel 5.0版本](https://www.unrealengine.com/marketplace/en-US/product/directexcel)  扩展了用于自动根据Excel更新配置功能

#### 接口 **IExcelAutoConfigInterface** 用于定义支持数据更新的Actor

  - GenerateExcelQueryData 获取查询数据, 返回 false 标志不需要更新
    
    + ```FExcelQueryRequest```
      
       * ```Result``` 查询结果及中间值 
       * ```QueryList``` 查询任务队列
       
     + ```FExcelQueryInfo```
       
       * ```SetTableName``` 设置表名
       * ```SetKeyName``` 设置查询的Key, 从```FExcelQueryRequest::Result```中取,可以使用前面的查询结果
       * ```AddColumnQuest(InColumnName, InResultKey)``` 增加查询字段
       
         1. 在```TableName```的表中查找第一列等于 ```FExcelQueryRequest::Result[FExcelQueryInfo::KeyName]``` 的行号
         2. 找到```InColumnName```列的数据, 并保存在```FExcelQueryRequest::Result[InResultKey]```中
    
    ```cpp
    FExcelQueryInfo MonsterInfo = FExcelQueryInfo(TEXT("Monster")) //表名Monster
	  .SetKeyName(TEXT("MonsterID")) //查询的Key
	  .AddColumnQuest(TEXT("AppearanceID"), TEXT("Monster.AppearanceID"))
	  .AddColumnQuest(TEXT("ModelScaling"), TEXT("ModelScaling"));
    ```
    
  - SetExcelAutoCompleteData 接受查询结果
  
#### 编辑器支持

  - 实现了接口 **IExcelAutoConfigInterface**的Actor右键增加菜单**更新数据**, 支持根据Excel更新数据
  - Tools/重新加载表格 菜单支持重新加载Excel, Excel暂时并未支持自动更新,需要手动刷新
  
#### 示例**ExampleProjectEditor**

  - DeployNPC 实现了接口 **IExcelAutoConfigInterface**, 根据MonsterID自动更新相关资源及缩放
  - Excel目录 为示例表格
