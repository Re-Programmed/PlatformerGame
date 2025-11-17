#pragma once

#ifdef _DEBUG

#include <vector>
#include <string>

class DebugCommands
{
public:
	static void HandleKeybinds();

	/// <summary>
	/// Starts a thread for recieving debugging commands from the console.
	/// </summary>
	static void RunRecieverThread();
	/// <summary>
	/// Handles commands sent via the console.
	/// </summary>
	static void HandleCommands();
private:
	static std::vector<std::string> m_queuedCommands;

	static std::vector<std::string> getParams(std::string input);
};

#endif // _DEBUG
