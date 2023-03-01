#include "src/c-api/core.hpp"

void Core_SetTickDelegate(TickDelegate delegate)
{
	MainComponent::getInstance()->setTickDelegate(delegate);
}
