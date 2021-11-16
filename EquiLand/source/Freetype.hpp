#ifndef SOURCE_FREETYPE_HPP
#define SOURCE_FREETYPE_HPP

#include <includes_pch.h>

struct FreetypeTest
{
	//enum FontBuildMode { FontBuildMode_FreeType, FontBuildMode_Stb };

	//FontBuildMode   BuildMode = FontBuildMode_FreeType;
	bool            WantRebuild = true;
	float           RasterizerMultiply = 1.0f;
	unsigned int    FreeTypeBuilderFlags = ImGuiFreeTypeBuilderFlags_ForceAutoHint;

	// Call _BEFORE_ NewFrame()
	bool PreNewFrame()
	{
		if (!WantRebuild)
			return false;

		ImFontAtlas* atlas = ImGui::GetIO().Fonts;
		for (int n = 0; n < atlas->ConfigData.Size; n++)
			((ImFontConfig*)&atlas->ConfigData[n])->RasterizerMultiply = RasterizerMultiply;


		atlas->FontBuilderIO = ImGuiFreeType::GetBuilderForFreeType();
		atlas->FontBuilderFlags = FreeTypeBuilderFlags;

		atlas->Build();
		WantRebuild = false;
		return true;
	}
};


#endif // !SOURCE_FREETYPE_HPP 