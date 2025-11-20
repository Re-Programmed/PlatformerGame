#include "SettingsGlobals.h"
#include "AppDataFileManager.h"

#include <iostream>

namespace GAME_NAME
{
	namespace AppData
	{
		namespace Settings
		{
			UI16_AppDataGlobalVariable SettingsGlobals::MaxThreads("MaximumThreads");
			UI16_AppDataGlobalVariable SettingsGlobals::WindowResolutionX("WindowScaleX");
			UI16_AppDataGlobalVariable SettingsGlobals::WindowResolutionY("WindowScaleY");
			BOOL_AppDataGlobalVariable SettingsGlobals::Fullscreen("Fullscreen");

			void SettingsGlobals::LoadAppDataVariables()
			{
				MaxThreads.Value = std::stoi(AppDataFileManager::GetValue("settings\\performance.dat", MaxThreads.VariableName, "32"));
				WindowResolutionY.Value = std::stoi(AppDataFileManager::GetValue("settings\\graphics.dat", WindowResolutionY.VariableName, "0"));
				WindowResolutionX.Value = std::stoi(AppDataFileManager::GetValue("settings\\graphics.dat", WindowResolutionX.VariableName, "0"));
				Fullscreen.Value = AppDataFileManager::GetValue("settings\\graphics.dat", Fullscreen.VariableName, "f").starts_with('t');
			}

			void SettingsGlobals::SaveUpdatedVariables()
			{
				std::cout << "SAVING SETTINGS VARIABLES" << std::endl;
				AppDataFileManager::SetValue("settings\\graphics.dat", WindowResolutionX.VariableName, std::to_string(WindowResolutionX.Value));
				AppDataFileManager::SetValue("settings\\graphics.dat", WindowResolutionY.VariableName, std::to_string(WindowResolutionY.Value));
				AppDataFileManager::SetValue("settings\\graphics.dat", Fullscreen.VariableName, Fullscreen.Value ? "t" : "f");
			}
		}
	}
}