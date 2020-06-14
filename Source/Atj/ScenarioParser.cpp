#include "ScenarioParser.h"

#include "JsonObjectConverter.h"
#include "Misc/Paths.h"

// Sets default values
AScenarioParser::AScenarioParser()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

FScenarioData AScenarioParser::GetScenarioData() const {
	return _scenarioData;
}

TSharedRef<FSignal> AScenarioParser::ParseSignalBindNpc(const FJsonObject& signalObject)
{
	TSharedRef<FSignal_BindNpc> fSignal = MakeShared<FSignal_BindNpc>();
	fSignal->type = SignalTypes::BindNpc;
	fSignal->npc = signalObject.GetStringField("npc");
	fSignal->routine = signalObject.GetStringField("routine");
	return fSignal;
}

TSharedRef<FSignal> AScenarioParser::ParseSignalObjectSetState(const FJsonObject& signalObject)
{
	TSharedRef<FSignal_ObjectSetState> fSignal = MakeShared<FSignal_ObjectSetState>();
	fSignal->type = SignalTypes::ObjectSetState;
	fSignal->object = signalObject.GetStringField("object");
	fSignal->state = signalObject.GetStringField("state");
	return fSignal;
}

// Called when the game starts or when spawned
void AScenarioParser::BeginPlay()
{
	Super::BeginPlay();

	FString dataPath = FPaths::GameSourceDir() + "Atj/data/data.json";
	FString result;
	FFileHelper::LoadFileToString(result, *dataPath);

	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
	TSharedRef< TJsonReader<> > JsonReader = TJsonReaderFactory<>::Create(result);

	// Deserialize the JSON data

	FScenarioData scenarioData;

	if (FJsonSerializer::Deserialize(JsonReader, JsonObject) &&
		JsonObject.IsValid())
	{
		// Parse npcs
		TArray<TSharedPtr<FJsonValue>> npcs = JsonObject->GetArrayField("npcs");
		for (int32 i = 0; i < npcs.Num(); i++)
		{
			FString npc = npcs[i]->AsString();
			scenarioData.npcs.Add(npc);
		};

		// Parse objects
		TArray<TSharedPtr<FJsonValue>> objects = JsonObject->GetArrayField("objects");
		for (int32 i = 0; i < objects.Num(); i++)
		{
			FString object = objects[i]->AsString();
			scenarioData.objects.Add(object);
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
			const auto conditions = triggerObject->GetArrayField("conditions");
			for (const auto& condition : conditions) {
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
			scenarioData.triggers.Add(triggerName, fTrigger);
		};

		// Parse Actions
		TArray<TSharedPtr<FJsonValue>> actions = JsonObject->GetArrayField("actions");
		for (const auto action : actions)
		{
			auto actionObject = action->AsObject();
			FString actionName = actionObject->GetStringField("name");
			UE_LOG(LogTemp, Warning, TEXT("DEBUG Action: %s"), *(actionName));

			FAction fAction;

			// Parse signals
			const auto signals = actionObject->GetArrayField("signals");
			for (const auto& signal : signals) {
				const auto signalObject = signal->AsObject();

				const auto type = signalObject->GetStringField("type");
				if (type == "bind_npc") {
					TSharedRef<FSignal> fSignal = ParseSignalBindNpc(*signalObject);
					fAction.signals.Add(fSignal);
				} else if (type == "object_set_state") {
					TSharedRef<FSignal> fSignal = ParseSignalObjectSetState(*signalObject);
					fAction.signals.Add(fSignal);
				}
			}

			// Add data to struct
			scenarioData.actions.Add(actionName, fAction);
		}

		// Parse Routines
		TArray<TSharedPtr<FJsonValue>> routines = JsonObject->GetArrayField("routines");
		for (const auto routine : routines)
		{
			auto routineObject = routine->AsObject();
			FString routineName = routineObject->GetStringField("name");
			UE_LOG(LogTemp, Warning, TEXT("DEBUG Routine: %s"), *(routineName));

			FRoutine fRoutine;

			// Parse tasks
			const auto tasks = routineObject->GetArrayField("tasks");
			for (const auto& task : tasks) {
				FTask fTask;
				fTask.type = task->AsObject()->GetStringField("type");
				fTask.sync_time = task->AsObject()->GetStringField("sync_time");
				fTask.behavior = task->AsObject()->GetStringField("behavior");
				fTask.target = task->AsObject()->GetStringField("target");
				fRoutine.tasks.Add(fTask);
			}

			// Add data to struct
			scenarioData.routines.Add(routineName, fRoutine);
		}

		// Print serialized data
		// Print Npcs
		for (const auto& npc : scenarioData.npcs)
		{
			UE_LOG(LogTemp, Warning, TEXT("NPC: %s"), *npc);
		};
		// Print Objects
		for (const auto& object : scenarioData.objects)
		{
			UE_LOG(LogTemp, Warning, TEXT("Object: %s"), *object);
		};
		// Print Triggers
		for (const auto& trigger : scenarioData.triggers)
		{
			UE_LOG(LogTemp, Warning, TEXT("Trigger Name: %s"), *trigger.Key);
			for (const auto& condition : trigger.Value.conditions) {
				UE_LOG(LogTemp, Warning, TEXT("Trigger Condition Check: %s"), *(condition.check));
				UE_LOG(LogTemp, Warning, TEXT("Trigger Condition Npc: %s"), *(condition.npc));
				UE_LOG(LogTemp, Warning, TEXT("Trigger Condition Location: %s"), *(condition.location));
				UE_LOG(LogTemp, Warning, TEXT("---"));
			}
			for (const auto& action : trigger.Value.actions) {
				UE_LOG(LogTemp, Warning, TEXT("Trigger Action: %s"), *action);
			}
		};
		// Print Actions
		for (const auto& action : scenarioData.actions)
		{
			UE_LOG(LogTemp, Warning, TEXT("Action Name: %s"), *action.Key);
			for (const auto signal : action.Value.signals) {
				UE_LOG(LogTemp, Warning, TEXT("Action Signal Type: %s"), *(UEnum::GetValueAsString<SignalTypes>(signal->type)));
				switch (signal->type) {
				case SignalTypes::BindNpc:
				{
					const TSharedRef<FSignal_BindNpc> signalCast = StaticCastSharedRef<FSignal_BindNpc>(signal);
					UE_LOG(LogTemp, Warning, TEXT("Action Signal Npc: %s"), *(signalCast->npc));
					UE_LOG(LogTemp, Warning, TEXT("Action Signal Routine: %s"), *(signalCast->routine));
					UE_LOG(LogTemp, Warning, TEXT("---"));
				}
					break;
				case SignalTypes::ObjectSetState:
				{
					const TSharedRef<FSignal_ObjectSetState> signalCast = StaticCastSharedRef<FSignal_ObjectSetState>(signal);
					UE_LOG(LogTemp, Warning, TEXT("Action Signal Object: %s"), *(signalCast->object));
					UE_LOG(LogTemp, Warning, TEXT("Action Signal State: %s"), *(signalCast->state));
					UE_LOG(LogTemp, Warning, TEXT("---"));
				}
					break;
				}
			}
		};
		// Print Routines
		for (const auto& routine : scenarioData.routines)
		{
			UE_LOG(LogTemp, Warning, TEXT("Routine Name: %s"), *routine.Key);
			for (const auto& task : routine.Value.tasks) {
				UE_LOG(LogTemp, Warning, TEXT("Routine Task Type: %s"), *(task.type));
				UE_LOG(LogTemp, Warning, TEXT("Routine Task Sync_Time: %s"), *(task.sync_time));
				UE_LOG(LogTemp, Warning, TEXT("Routine Task Behavior: %s"), *(task.behavior));
				UE_LOG(LogTemp, Warning, TEXT("Routine Task Target: %s"), *(task.target));
				UE_LOG(LogTemp, Warning, TEXT("---"));
			}
		};
	};

	_scenarioData = scenarioData;

}

// Called every frame
void AScenarioParser::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

