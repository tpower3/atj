#include "ScenarioParser.h"

#include "JsonObjectConverter.h"
#include "Misc/Paths.h"

// Sets default values
AScenarioParser::AScenarioParser()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

FScenarioData AScenarioParser::GetScenarioData() {
	if (!_scenarioData.IsSet()) {
		ParseScenario();
	}
	return _scenarioData.GetValue();
}

TSharedRef<FCondition> AScenarioParser::ParseConditionNpcMoodCheck(const FJsonObject& conditionObject) const
{
	TSharedRef<FCondition_NpcMoodCheck> fCondition = MakeShared<FCondition_NpcMoodCheck>();
	fCondition->type = ConditionTypes::NpcMoodCheck;
	fCondition->npc = conditionObject.GetStringField("npc");
	fCondition->check = conditionObject.GetStringField("check");
	fCondition->value = conditionObject.GetStringField("value");
	return fCondition;
}

TSharedRef<FCondition> AScenarioParser::ParseConditionNpcPositionCheck(const FJsonObject& conditionObject) const
{
	TSharedRef<FCondition_NpcPositionCheck> fCondition = MakeShared<FCondition_NpcPositionCheck>();
	fCondition->type = ConditionTypes::NpcPositionCheck;
	fCondition->npc = conditionObject.GetStringField("npc");
	fCondition->object = conditionObject.GetStringField("object");
	fCondition->distance = conditionObject.GetStringField("distance");
	return fCondition;
}

TSharedRef<FCondition> AScenarioParser::ParseConditionItemInObjectSlotCheck(const FJsonObject& conditionObject) const
{
	TSharedRef<FCondition_ItemInObjectSlotCheck> fCondition = MakeShared<FCondition_ItemInObjectSlotCheck>();
	fCondition->type = ConditionTypes::ItemInObjectSlotCheck;
	fCondition->item = conditionObject.GetStringField("item");
	fCondition->object = conditionObject.GetStringField("object");
	return fCondition;
}

TSharedRef<FSignal> AScenarioParser::ParseSignalBindNpc(const FJsonObject& signalObject)
{
	TSharedRef<FSignal_BindNpc> fSignal = MakeShared<FSignal_BindNpc>();
	fSignal->type = SignalTypes::BindNpc;
	fSignal->npc = signalObject.GetStringField("npc");
	fSignal->routine = signalObject.GetStringField("routine");
	return fSignal;
}

TSharedRef<FSignal> AScenarioParser::ParseSignalEndGameFailure(const FJsonObject& signalObject)
{
	TSharedRef<FSignal_EndGameFailure> fSignal = MakeShared<FSignal_EndGameFailure>();
	fSignal->type = SignalTypes::EndGameFailure;
	return fSignal;
}

TSharedRef<FSignal> AScenarioParser::ParseSignalEndGameSuccess(const FJsonObject& signalObject)
{
	TSharedRef<FSignal_EndGameSuccess> fSignal = MakeShared<FSignal_EndGameSuccess>();
	fSignal->type = SignalTypes::EndGameSuccess;
	return fSignal;
}

TSharedRef<FSignal> AScenarioParser::ParseSignalIncrementMood(const FJsonObject& signalObject)
{
	TSharedRef<FSignal_IncrementMood> fSignal = MakeShared<FSignal_IncrementMood>();
	fSignal->type = SignalTypes::IncrementMood;
	fSignal->npc = signalObject.GetStringField("npc");
	fSignal->value = signalObject.GetStringField("value");
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

static TSharedRef<FTask> ParseTaskBehavior(const FJsonObject& taskObject)
{
	TSharedRef<FTask_Behavior> fTask = MakeShared<FTask_Behavior>();
	fTask->type = TaskTypes::Behavior;
	fTask->sync_time = taskObject.GetStringField("sync_time");
	fTask->behavior = taskObject.GetStringField("behavior");
	fTask->target = taskObject.GetStringField("target");
	return fTask;
}

static TSharedRef<FTask> ParseTaskEvaluateTrigger(const FJsonObject& taskObject)
{
	TSharedRef<FTask_EvaluateTrigger> fTask = MakeShared<FTask_EvaluateTrigger>();
	fTask->type = TaskTypes::EvaluateTrigger;
	fTask->sync_time = taskObject.GetStringField("sync_time");
	fTask->trigger = taskObject.GetStringField("trigger");
	fTask->pass_action = taskObject.GetStringField("pass_action");
	fTask->fail_action = taskObject.GetStringField("fail_action");
	return fTask;
}

static TSharedRef<FTask> ParseTaskExecuteAction(const FJsonObject& taskObject)
{
	TSharedRef<FTask_ExecuteAction> fTask = MakeShared<FTask_ExecuteAction>();
	fTask->type = TaskTypes::ExecuteAction;
	fTask->sync_time = taskObject.GetStringField("sync_time");
	fTask->action = taskObject.GetStringField("action");
	return fTask;
}

// Called when the game starts or when spawned
void AScenarioParser::BeginPlay()
{
	Super::BeginPlay();
}

void AScenarioParser::ParseScenario()
{
	FString dataPath = FPaths::GameSourceDir() + "Atj/data/mvp_scenario.json";
	FString result;
	FFileHelper::LoadFileToString(result, *dataPath);

	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
	TSharedRef< TJsonReader<> > JsonReader = TJsonReaderFactory<>::Create(result);

	// Deserialize the JSON data

	FScenarioData scenarioData;

	if (FJsonSerializer::Deserialize(JsonReader, JsonObject) &&
		JsonObject.IsValid())
	{
		// Parse debug mode
		bool isDebugMode;
		JsonObject->TryGetBoolField("isDebugMode", isDebugMode);
		scenarioData.isDebugMode = isDebugMode;

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

			const bool alwaysEvaluate = triggerObject->GetBoolField("always_evaluate");
			fTrigger.alwaysEvaluate = alwaysEvaluate;

			// Parse conditions
			const auto all_of_conditions = triggerObject->GetArrayField("all_of");
			for (const auto& condition : all_of_conditions) {
				const auto conditionObject = condition->AsObject();

				const auto type = conditionObject->GetStringField("type");
				if (type == "npc_mood_check") {
					TSharedRef<FCondition> fCondition = ParseConditionNpcMoodCheck(*conditionObject);
					fTrigger.allOf.Add(fCondition);
				} else if (type == "npc_position_check") {
					TSharedRef<FCondition> fCondition = ParseConditionNpcPositionCheck(*conditionObject);
					fTrigger.allOf.Add(fCondition);
				} else if (type == "item_in_object_slot_check") {
					TSharedRef<FCondition> fCondition = ParseConditionItemInObjectSlotCheck(*conditionObject);
					fTrigger.allOf.Add(fCondition);
				}
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
				} else if (type == "end_game_failure") {
					TSharedRef<FSignal> fSignal = ParseSignalEndGameFailure(*signalObject);
					fAction.signals.Add(fSignal);
				} else if (type == "end_game_success") {
					TSharedRef<FSignal> fSignal = ParseSignalEndGameSuccess(*signalObject);
					fAction.signals.Add(fSignal);
				} else if (type == "increment_mood") {
					TSharedRef<FSignal> fSignal = ParseSignalIncrementMood(*signalObject);
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
				auto taskObject = task->AsObject();

				const auto type = taskObject->GetStringField("type");
				if (type == "behavior") {
					TSharedRef<FTask> fTask = ParseTaskBehavior(*taskObject);
					fRoutine.tasks.Add(fTask);
				}
				else if (type == "evaluate_trigger") {
					TSharedRef<FTask> fTask = ParseTaskEvaluateTrigger(*taskObject);
					fRoutine.tasks.Add(fTask);
				}
				else if (type == "execute_action") {
					TSharedRef<FTask> fTask = ParseTaskExecuteAction(*taskObject);
					fRoutine.tasks.Add(fTask);
				}
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
			for (const auto& condition : trigger.Value.allOf) {
				UE_LOG(LogTemp, Warning, TEXT("Trigger Condition Type: %s"), *(UEnum::GetValueAsString<ConditionTypes>(condition->type)));
				switch (condition->type) {
					case ConditionTypes::NpcMoodCheck:
					{
						const TSharedRef<FCondition_NpcMoodCheck> conditionCast = StaticCastSharedRef<FCondition_NpcMoodCheck>(condition);
						UE_LOG(LogTemp, Warning, TEXT("Trigger Condition Npc: %s"), *(conditionCast->npc));
						UE_LOG(LogTemp, Warning, TEXT("Trigger Condition Check: %s"), *(conditionCast->check));
						UE_LOG(LogTemp, Warning, TEXT("Trigger Condition Value: %s"), *(conditionCast->value));

						UE_LOG(LogTemp, Warning, TEXT("---"));
					}
					break;
					case ConditionTypes::NpcPositionCheck:
					{
						const TSharedRef<FCondition_NpcPositionCheck> conditionCast = StaticCastSharedRef<FCondition_NpcPositionCheck>(condition);
						UE_LOG(LogTemp, Warning, TEXT("Trigger Condition Npc: %s"), *(conditionCast->npc));
						UE_LOG(LogTemp, Warning, TEXT("Trigger Condition Object: %s"), *(conditionCast->object));
						UE_LOG(LogTemp, Warning, TEXT("Trigger Condition Distance: %s"), *(conditionCast->distance));

						UE_LOG(LogTemp, Warning, TEXT("---"));
					}
					break;
					case ConditionTypes::ItemInObjectSlotCheck:
					{
						const TSharedRef<FCondition_ItemInObjectSlotCheck> conditionCast = StaticCastSharedRef<FCondition_ItemInObjectSlotCheck>(condition);
						UE_LOG(LogTemp, Warning, TEXT("Trigger Condition Item: %s"), *(conditionCast->item));
						UE_LOG(LogTemp, Warning, TEXT("Trigger Condition Object: %s"), *(conditionCast->object));

						UE_LOG(LogTemp, Warning, TEXT("---"));
					}
					break;
				}
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
				case SignalTypes::EndGameFailure:
				{
					const TSharedRef<FSignal_EndGameFailure> signalCast = StaticCastSharedRef<FSignal_EndGameFailure>(signal);
					UE_LOG(LogTemp, Warning, TEXT("---"));
				}
				break;
				case SignalTypes::EndGameSuccess:
				{
					const TSharedRef<FSignal_EndGameSuccess> signalCast = StaticCastSharedRef<FSignal_EndGameSuccess>(signal);
					UE_LOG(LogTemp, Warning, TEXT("---"));
				}
				break;
				case SignalTypes::IncrementMood:
				{
					const TSharedRef<FSignal_IncrementMood> signalCast = StaticCastSharedRef<FSignal_IncrementMood>(signal);
					UE_LOG(LogTemp, Warning, TEXT("Action Signal Npc: %s"), *(signalCast->npc));
					UE_LOG(LogTemp, Warning, TEXT("Action Signal Value: %s"), *(signalCast->value));
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
				UE_LOG(LogTemp, Warning, TEXT("Routine Task Type: %s"), *(UEnum::GetValueAsString<TaskTypes>(task->type)));
				UE_LOG(LogTemp, Warning, TEXT("Routine Task Sync_Time: %s"), *(task->sync_time));
				switch (task->type) {
				case TaskTypes::Behavior:
				{
					const TSharedRef<FTask_Behavior> taskCast = StaticCastSharedRef<FTask_Behavior>(task);
					UE_LOG(LogTemp, Warning, TEXT("Routine Task Behavior: %s"), *(taskCast->behavior));
					UE_LOG(LogTemp, Warning, TEXT("Routine Task Target: %s"), *(taskCast->target));
				}
				break;
				case TaskTypes::EvaluateTrigger:
				{
					const TSharedRef<FTask_EvaluateTrigger> taskCast = StaticCastSharedRef<FTask_EvaluateTrigger>(task);
					UE_LOG(LogTemp, Warning, TEXT("Routine Task Trigger: %s"), *(taskCast->trigger));
					UE_LOG(LogTemp, Warning, TEXT("Routine Task pass_action: %s"), *(taskCast->pass_action));
					UE_LOG(LogTemp, Warning, TEXT("Routine Task fail_action: %s"), *(taskCast->fail_action));
				}
				break;
				case TaskTypes::ExecuteAction:
				{
					const TSharedRef<FTask_ExecuteAction> taskCast = StaticCastSharedRef<FTask_ExecuteAction>(task);
					UE_LOG(LogTemp, Warning, TEXT("Routine Task Action: %s"), *(taskCast->action));
				}
				break;
				UE_LOG(LogTemp, Warning, TEXT("---"));
				}
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

