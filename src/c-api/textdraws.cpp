#include "src/c-api/textdraws.hpp"
#include "src/component.hpp"

Vector2 TextDrawBase_GetPosition(const ITextDrawBase* td)
{
	return td->getPosition();
}

void TextDrawBase_SetPosition(ITextDrawBase* td, Vector2 position)
{
	td->setPosition(position);
}

void TextDrawBase_SetText(ITextDrawBase* td, const char* text)
{
	td->setText(text);
}

const char* TextDrawBase_GetText(const ITextDrawBase* td)
{
	return td->getText().data();
}

void TextDrawBase_SetLetterSize(ITextDrawBase* td, Vector2 size)
{
	td->setLetterSize(size);
}

Vector2 TextDrawBase_GetLetterSize(const ITextDrawBase* td)
{
	return td->getLetterSize();
}

void TextDrawBase_SetTextSize(ITextDrawBase* td, Vector2 size)
{
	td->setTextSize(size);
}

Vector2 TextDrawBase_GetTextSize(const ITextDrawBase* td)
{
	return td->getTextSize();
}

void TextDrawBase_SetAlignment(ITextDrawBase* td, TextDrawAlignmentTypes alignment)
{
	td->setAlignment(alignment);
}

TextDrawAlignmentTypes TextDrawBase_GetAlignment(const ITextDrawBase* td)
{
	return td->getAlignment();
}

void TextDrawBase_SetColour(ITextDrawBase* td, uint32_t argb)
{
	td->setColour(Colour::FromARGB(argb));
}

uint32_t TextDrawBase_GetLetterColour(const ITextDrawBase* td)
{
	return td->getLetterColour().ARGB();
}

void TextDrawBase_UseBox(ITextDrawBase* td, bool use)
{
	td->useBox(use);
}

bool TextDrawBase_HasBox(ITextDrawBase* td)
{
	return td->hasBox();
}

void TextDrawBase_SetBoxColour(ITextDrawBase* td, uint32_t argb)
{
	td->setBoxColour(Colour::FromARGB(argb));
}

uint32_t TextDrawBase_GetBoxColour(const ITextDrawBase* td)
{
	return td->getBoxColour().ARGB();
}

void TextDrawBase_SetShadow(ITextDrawBase* td, int shadow)
{
	td->setShadow(shadow);
}

int TextDrawBase_GetShadow(const ITextDrawBase* td)
{
	return td->getShadow();
}

void TextDrawBase_SetOutline(ITextDrawBase* td, int outline)
{
	td->setOutline(outline);
}

int TextDrawBase_GetOutline(const ITextDrawBase* td)
{
	return td->getOutline();
}

void TextDrawBase_SetBackgroundColour(ITextDrawBase* td, uint32_t argb)
{
	td->setBackgroundColour(Colour::FromARGB(argb));
}

uint32_t TextDrawBase_GetBackgroundColour(const ITextDrawBase* td)
{
	return td->getBackgroundColour().ARGB();
}

void TextDrawBase_SetStyle(ITextDrawBase* td, TextDrawStyle style)
{
	td->setStyle(style);
}

TextDrawStyle TextDrawBase_GetStyle(const ITextDrawBase* td)
{
	return td->getStyle();
}

void TextDrawBase_SetProportional(ITextDrawBase* td, bool proportional)
{
	td->setProportional(proportional);
}

bool TextDrawBase_IsProportional(ITextDrawBase* td)
{
	return td->isProportional();
}

void TextDrawBase_SetSelectable(ITextDrawBase* td, bool selectable)
{
	td->setSelectable(selectable);
}

bool TextDrawBase_IsSelectable(ITextDrawBase* td)
{
	return td->isSelectable();
}

void TextDrawBase_SetPreviewModel(ITextDrawBase* td, int model)
{
	td->setPreviewModel(model);
}

int TextDrawBase_GetPreviewModel(const ITextDrawBase* td)
{
	return td->getPreviewModel();
}

void TextDrawBase_SetPreviewRotation(ITextDrawBase* td, Vector3 rotation)
{
	td->setPreviewRotation(rotation);
}

Vector3 TextDrawBase_GetPreviewRotation(const ITextDrawBase* td)
{
	return td->getPreviewRotation();
}

void TextDrawBase_SetPreviewVehicleColour(ITextDrawBase* td, int colour1, int colour2)
{
	td->setPreviewVehicleColour(colour1, colour2);
}

void TextDrawBase_GetPreviewVehicleColour(const ITextDrawBase* td, int* colour1, int* colour2)
{
	auto pair = td->getPreviewVehicleColour();
	*colour1 = pair.first;
	*colour2 = pair.second;
}

void TextDrawBase_SetPreviewZoom(ITextDrawBase* td, float zoom)
{
	td->setPreviewZoom(zoom);
}

float TextDrawBase_GetPreviewZoom(const ITextDrawBase* td)
{
	return td->getPreviewZoom();
}

void TextDrawBase_Restream(ITextDrawBase* td)
{
	td->restream();
}

ITextDrawBase* TextDraw_CreateText(Vector2 position, const char* text)
{
	return MainComponent::getInstance()->getTextDrawComponent()->create(position, text);
}

ITextDrawBase* TextDraw_CreatePreviewModel(Vector2 position, int model)
{
	return MainComponent::getInstance()->getTextDrawComponent()->create(position, model);
}

void TextDraw_ShowForPlayer(ITextDrawBase* td, IEntity* player)
{
	static_cast<ITextDraw*>(td)->showForPlayer(*static_cast<IPlayer*>(player));
}

void TextDraw_HideForPlayer(ITextDrawBase* td, IEntity* player)
{
	static_cast<ITextDraw*>(td)->hideForPlayer(*static_cast<IPlayer*>(player));
}

bool TextDraw_IsShownForPlayer(ITextDrawBase* td, const IEntity* player)
{
	return static_cast<ITextDraw*>(td)->isShownForPlayer(*static_cast<const IPlayer*>(player));
}

void TextDraw_SetTextForPlayer(ITextDrawBase* td, IEntity* player, const char* text)
{
	static_cast<ITextDraw*>(td)->setTextForPlayer(*static_cast<IPlayer*>(player), text);
}

void Player_BeginSelection(IEntity* player, uint32_t argb)
{
	queryExtension<IPlayerTextDrawData>(static_cast<IPlayer*>(player))->beginSelection(Colour::FromARGB(argb));
}

bool Player_IsSelecting(IEntity* player)
{
	return queryExtension<IPlayerTextDrawData>(static_cast<IPlayer*>(player))->isSelecting();
}

void Player_EndSelection(IEntity* player)
{
	queryExtension<IPlayerTextDrawData>(static_cast<IPlayer*>(player))->endSelection();
}

ITextDrawBase* PlayerTextDraw_CreateText(IEntity* player, Vector2 position, const char* text)
{
	return queryExtension<IPlayerTextDrawData>(static_cast<IPlayer*>(player))->create(position, text);
}

ITextDrawBase* PlayerTextDraw_CreatePreviewModel(IEntity* player, Vector2 position, int model)
{
	return queryExtension<IPlayerTextDrawData>(static_cast<IPlayer*>(player))->create(position, model);
}

void PlayerTextDraw_Show(ITextDrawBase* td)
{
	static_cast<IPlayerTextDraw*>(td)->show();
}

void PlayerTextDraw_Hide(ITextDrawBase* td)
{
	static_cast<IPlayerTextDraw*>(td)->hide();
}

bool PlayerTextDraw_IsShown(ITextDrawBase* td)
{
	return static_cast<IPlayerTextDraw*>(td)->isShown();
}
