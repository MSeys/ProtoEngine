#pragma once
#include <iostream>
#include <string>
#include <unordered_map>

#include "Singleton.h"
#include "Observer.h"

namespace Proto
{
	class Messenger : public Singleton<Messenger>
	{
	public:
		Messenger() = default;
		~Messenger()
		{
			for (auto& pair : m_pObservers)
			{
				SafeDelete(pair.second);
			}
		}

		Messenger(const Messenger& other) = delete;
		Messenger(Messenger&& other) = delete;
		Messenger& operator=(const Messenger& other) = delete;
		Messenger& operator=(Messenger&& other) = delete;

		bool AddObserver(Observer* pObserver, const std::string& key)
		{
			if (m_pObservers.find(key) == m_pObservers.end()) // No observer at this key
			{
				m_pObservers[key] = pObserver;
				return true;
			}

			SafeDelete(pObserver);

			ProtoLogger.AddLog(LogLevel::Warning, "Messenger::AddObserver < Failed. There is already an observer on this key.");
			return false;
		}

		bool RemoveObserver(const std::string& key)
		{
			if (m_pObservers.find(key) != m_pObservers.end()) // observer at this key
			{
				m_pObservers.erase(key);
				return true;
			}

			ProtoLogger.AddLog(LogLevel::Warning, "Messenger::RemoveObserver < Failed. No Observer on this key.");
			return false;
		}

		void Notify(void* pData, int event)
		{
			for (auto& pair : m_pObservers)
			{
				const std::vector<int> listeningEvents{ pair.second->GetListeningEvents() };

				if (listeningEvents.empty() || std::find(listeningEvents.cbegin(), listeningEvents.cend(), event) != listeningEvents.cend())
					pair.second->Notify(pData, event);

				// Do nothing, event isn't meant to be listened to by current Observer
			}
		}

	private:
		std::unordered_map<std::string, Observer*> m_pObservers;
	};
}