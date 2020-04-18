#pragma once
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

#define ToCString(value) std::to_string(value).c_str()