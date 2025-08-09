#include "pch.h"
#include "FileHelper.h"

#include <filesystem>
#include <fstream>

namespace FS = std::filesystem;
using FPaths = FS::path;

bool FFileHelper::LoadFileToArray(TArray<uint8>& Result, const TCHAR* Filename)
{
	FPaths Path(Filename);

	const uint32 FileSize = static_cast<uint32>(FS::file_size(Path));
	Result.resize(FileSize);

	basic_ifstream<uint8> Input(Path);
	Input.read(&Result[0], FileSize);

	return true;
}

FString FFileHelper::Convert(std::string Src)
{
	const int32 SrcLen = static_cast<int32>(Src.size());
	if (SrcLen == 0)
	{
		return {};
	}

	FString Dest;
	const int32 DestLen = ::MultiByteToWideChar(CP_UTF8, 0, &Src[0], SrcLen, nullptr, 0);
	Dest.resize(DestLen);

	::MultiByteToWideChar(CP_UTF8, 0, &Src[0], SrcLen, &Dest[0], DestLen);
	return Dest;
}
