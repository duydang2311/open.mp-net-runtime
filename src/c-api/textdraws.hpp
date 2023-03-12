#pragma once

#include "Server/Components/TextDraws/textdraws.hpp"
#include "src/utils/export.hpp"

DLL_EXPORT Vector2 TextDrawBase_GetPosition(const ITextDrawBase* td);
DLL_EXPORT void TextDrawBase_SetPosition(ITextDrawBase* td, Vector2 position);
DLL_EXPORT void TextDrawBase_SetText(ITextDrawBase* td, const char* text);
DLL_EXPORT const char* TextDrawBase_GetText(const ITextDrawBase* td);
DLL_EXPORT void TextDrawBase_SetLetterSize(ITextDrawBase* td, Vector2 size);
DLL_EXPORT Vector2 TextDrawBase_GetLetterSize(const ITextDrawBase* td);
DLL_EXPORT void TextDrawBase_SetTextSize(ITextDrawBase* td, Vector2 size);
DLL_EXPORT Vector2 TextDrawBase_GetTextSize(const ITextDrawBase* td);
DLL_EXPORT void TextDrawBase_SetAlignment(ITextDrawBase* td, TextDrawAlignmentTypes alignment);
DLL_EXPORT TextDrawAlignmentTypes TextDrawBase_GetAlignment(const ITextDrawBase* td);
DLL_EXPORT void TextDrawBase_SetColour(ITextDrawBase* td, uint32_t colour);
DLL_EXPORT uint32_t TextDrawBase_GetLetterColour(const ITextDrawBase* td);
DLL_EXPORT void TextDrawBase_UseBox(ITextDrawBase* td, bool use);
DLL_EXPORT bool TextDrawBase_HasBox(ITextDrawBase* td);
DLL_EXPORT void TextDrawBase_SetBoxColour(ITextDrawBase* td, uint32_t colour);
DLL_EXPORT uint32_t TextDrawBase_GetBoxColour(const ITextDrawBase* td);
DLL_EXPORT void TextDrawBase_SetShadow(ITextDrawBase* td, int shadow);
DLL_EXPORT int TextDrawBase_GetShadow(const ITextDrawBase* td);
DLL_EXPORT void TextDrawBase_SetOutline(ITextDrawBase* td, int outline);
DLL_EXPORT int TextDrawBase_GetOutline(const ITextDrawBase* td);
DLL_EXPORT void TextDrawBase_SetBackgroundColour(ITextDrawBase* td, uint32_t colour);
DLL_EXPORT uint32_t TextDrawBase_GetBackgroundColour(const ITextDrawBase* td);
DLL_EXPORT void TextDrawBase_SetStyle(ITextDrawBase* td, TextDrawStyle style);
DLL_EXPORT TextDrawStyle TextDrawBase_GetStyle(const ITextDrawBase* td);
DLL_EXPORT void TextDrawBase_SetProportional(ITextDrawBase* td, bool proportional);
DLL_EXPORT bool TextDrawBase_IsProportional(ITextDrawBase* td);
DLL_EXPORT void TextDrawBase_SetSelectable(ITextDrawBase* td, bool selectable);
DLL_EXPORT bool TextDrawBase_IsSelectable(ITextDrawBase* td);
DLL_EXPORT void TextDrawBase_SetPreviewModel(ITextDrawBase* td, int model);
DLL_EXPORT int TextDrawBase_GetPreviewModel(const ITextDrawBase* td);
DLL_EXPORT void TextDrawBase_SetPreviewRotation(ITextDrawBase* td, Vector3 rotation);
DLL_EXPORT Vector3 TextDrawBase_GetPreviewRotation(const ITextDrawBase* td);
DLL_EXPORT void TextDrawBase_SetPreviewVehicleColour(ITextDrawBase* td, int colour1, int colour2);
DLL_EXPORT void TextDrawBase_GetPreviewVehicleColour(const ITextDrawBase* td, int* colour1, int* colour2);
DLL_EXPORT void TextDrawBase_SetPreviewZoom(ITextDrawBase* td, float zoom);
DLL_EXPORT float TextDrawBase_GetPreviewZoom(const ITextDrawBase* td);
DLL_EXPORT void TextDrawBase_Restream(ITextDrawBase* td);

DLL_EXPORT ITextDrawBase* TextDraw_CreateText(Vector2 position, const char* text);
DLL_EXPORT ITextDrawBase* TextDraw_CreatePreviewModel(Vector2 position, int model);

DLL_EXPORT void Player_BeginSelection(IEntity* player, uint32_t highlight);
DLL_EXPORT bool Player_IsSelecting(IEntity* player);
DLL_EXPORT void Player_EndSelection(IEntity* player);
DLL_EXPORT ITextDrawBase* PlayerTextDraw_CreateText(IEntity* player, Vector2 position, const char* text);
DLL_EXPORT ITextDrawBase* PlayerTextDraw_CreatePreviewModel(IEntity* player, Vector2 position, int model);
