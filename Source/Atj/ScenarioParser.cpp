#include "ScenarioParser.h"

#include "JsonObjectConverter.h"
#include "Misc/Paths.h"

// Sets default values
AScenarioParser::AScenarioParser()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AScenarioParser::BeginPlay()
{
	Super::BeginPlay();

	FString dataPath = FPaths::GameSourceDir() + "Atj/data/data.json";
	FString result;
	FFileHelper::LoadFileToString(result, *dataPath);
/*
	FJsonData JsonData;
	FJsonObjectConverter::JsonObjectStringToUStruct<FJsonData>(
		result,
		&JsonData,
		0, 0);

	FString foo0 = JsonData.foo;
	FString barfoo0 = JsonData.bar_arr[0].barfoo;
	UE_LOG(LogTemp, Warning, TEXT("%s"), *barfoo0);
	*/

	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
	TSharedRef< TJsonReader<> > JsonReader = TJsonReaderFactory<>::Create(result);

	// Deserialize the JSON data

	FSimulatorData simulatorData;

	if (FJsonSerializer::Deserialize(JsonReader, JsonObject) &&
		JsonObject.IsValid())
	{
		/*FJsonObjectConverter::JsonObjectToUStruct<FJsonData>(
			npcs,
			&simulatorData.npcs,
			0, 0);
			*/

		// Parse npcs
		TArray<TSharedPtr<FJsonValue>> npcs = JsonObject->GetArrayField("npcs");
		for (int32 i = 0; i < npcs.Num(); i++)
		{
			FString npc = npcs[i]->AsString();
			simulatorData.npcs.Add(npc);
		};

		// Parse objects
		TArray<TSharedPtr<FJsonValue>> objects = JsonObject->GetArrayField("objects");
		for (int32 i = 0; i < objects.Num(); i++)
		{
			FString object = objects[i]->AsString();
			simulatorData.objects.Add(object);
		};

		// Parse triggers
		TArray<TSharedPtr<FJsonValue>> triggers = JsonObject->GetArrayField("triggers");
		for (const auto trigger : triggers)
		{
			auto triggerObject = trigger->AsObject();
			FString triggerName = triggerObject->GetStringField("name");
			UE_LOG(LogTemp, Warning, TEXT("DEBUG Trigger: %s"), *(triggerName));

			FTrigger fTrigger;

			// Parse conditions
			UE_LOG(LogTemp, Warning, TEXT("1"));
			const auto conditions = triggerObject->GetArrayField("conditions");
			UE_LOG(LogTemp, Warning, TEXT("2"));
			for (const auto& condition : conditions) {
				UE_LOG(LogTemp, Warning, TEXT("3"));
				FCondition fCondition;
				fCondition.check = condition->AsObject()->GetStringField("check");
				fCondition.npc = condition->AsObject()->GetStringField("npc");
				fCondition.location = condition->AsObject()->GetStringField("location");
				fTrigger.conditions.Add(fCondition);
			}

			// Parse actions
			const auto actions = triggerObject->GetArrayField("actions");
			for (const auto& action : actions) {
				FString actionStr = action->AsString();
				fTrigger.actions.Add(actionStr);
			}

			// Add data to struct
			simulatorData.triggers.Add(triggerName, fTrigger);
		};

		// TODO: Parse Actions
		// TODO: Parse Routines

		// Print serialized data
		for (const auto& npc : simulatorData.npcs)
		{
			UE_LOG(LogTemp, Warning, TEXT("NPC: %s"), *npc);
		};
		for (const auto& object : simulatorData.objects)
		{
			UE_LOG(LogTemp, Warning, TEXT("Object: %s"), *object);
		};
		for (const auto& trigger : simulatorData.triggers)
		{
			UE_LOG(LogTemp, Warning, TEXT("Trigger Name: %s"), *trigger.Key);
			for (const auto& condition : trigger.Value.conditions) {
				UE_LOG(LogTemp, Warning, TEXT("Trigger Condition Check: %s"), *(condition.check));
				UE_LOG(LogTemp, Warning, TEXT("Trigger Condition Npc: %s"), *(condition.npc));
				UE_LOG(LogTemp, Warning, TEXT("Trigger Condition Location: %s"), *(condition.location));
			}
			for (const auto& action : trigger.Value.actions) {
				UE_LOG(LogTemp, Warning, TEXT("Trigger Action: %s"), *action);
			}
		};
	};

}

// Called every frame
void AScenarioParser::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

