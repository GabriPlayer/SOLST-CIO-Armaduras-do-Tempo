#include "SolsticioGameMode.h"
#include "SolsticioArmorCharacter.h"

ASolsticioGameMode::ASolsticioGameMode()
{
    DefaultPawnClass = ASolsticioArmorCharacter::StaticClass();
}
