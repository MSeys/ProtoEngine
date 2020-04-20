#pragma once
#include <codecvt>
#include <locale>

template<class T>
void SafeDelete(T& pObjectToDelete)
{
	if (pObjectToDelete != nullptr)
	{
		delete(pObjectToDelete);
		pObjectToDelete = nullptr;
	}
}

inline float SecondsToMicroSeconds(float seconds)
{
	return std::roundf(seconds * 1'000'000);
}

inline float MicroSecondsToSeconds(float microSeconds)
{
	return microSeconds / 1'000'000;
}

template<typename T>
T& VoidCast(void* pData)
{
	return *static_cast<T*>(pData);
}

inline std::string WStringToString(const std::wstring& wstring)
{
	using convert_type = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_type, wchar_t> converter;
	return converter.to_bytes(wstring);
}

inline std::wstring StringToWString(const std::string& string)
{
	using convert_type = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_type, wchar_t> converter;
	return converter.from_bytes(string);
}

#define ToCString(value) std::to_string(value).c_str()
