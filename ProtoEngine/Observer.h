#pragma once
#include <utility>
#include <iostream>

class Observer
{
public:
	Observer(std::vector<int> listeningEvents = {}) : m_ListeningEvents(std::move(listeningEvents)) {}
	virtual ~Observer() = default;

	Observer(const Observer& other) = delete;
	Observer(Observer&& other) = delete;
	Observer& operator=(const Observer& other) = delete;
	Observer& operator=(Observer&& other) = delete;
	
	virtual void Notify(void* pData, int event) = 0;
	std::vector<int> GetListeningEvents() const { return m_ListeningEvents; }
	
protected:
	std::vector<int> m_ListeningEvents;
};