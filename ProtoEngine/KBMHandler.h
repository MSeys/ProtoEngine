#pragma once
class KBMHandler
{
public:
	void Init();
	void Destroy();

	void UpdateOutPoll();
	void UpdateInPoll(const SDL_Event& e);
	void Process();

	bool AddInput(SDL_Keycode keyCode);

	Key& GetInput(SDL_Keycode keyCode);
	MouseButton& GetMouseButton(const MouseState& mouseState) const noexcept;
	Mouse& GetMouse() const noexcept;

private:
	std::unordered_map<SDL_Keycode, Key*> m_Keys{};

	std::array<MouseButton*, 2> m_pMouseButtons{};
	Mouse* m_pMouse{};

	Key* m_pDefaultKey{};
};