#include "GameLogic.h"

#include "GameData.h"

static CardputerGameCore::DeepGameRuntime runtime(gameDefinition());

CardputerGameCore::DeepGameRuntime& gameRuntime() {
  return runtime;
}
