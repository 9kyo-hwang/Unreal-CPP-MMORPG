#pragma once

class FFileHelper
{
public:
	static bool LoadFileToArray(TArray<uint8>& Result, const TCHAR* Filename);
	static FString Convert(std::string Src);
};

