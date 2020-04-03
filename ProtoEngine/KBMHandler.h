#pragma once
class KBMHandler
{
public:
	void Init();
	void Destroy();

	void UpdateOutPoll();
	void UpdateInPoll(const SDL_Event& e);
	void Process();

	bool AddInput(int SDLKey);

	Key& GetInput(int SDLKey, bool doesCreate = true);
	MouseButton& GetInput(const StickState& mouseState) const noexcept;
	Mouse& GetInput() const noexcept;

private:
	std::unordered_map<int, Key*> m_Keys;

	std::array<MouseButton*, 2> m_pMouseButtons{};
	Mouse* m_pMouse{};

	Key* m_pDefaultKey{};
};