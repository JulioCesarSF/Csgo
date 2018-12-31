#include "ServiceManager.h"
#include "GlowService.h"

int main()
{
	std::cout << "Starting..." << std::endl;
	ServiceManager *service_manager = new ServiceManager();
	try
	{		
		if (!service_manager->Load()) return 1;

		service_manager->StartAll();

		bool glowEnabled = true, fakeLagEnabled = true;

		while (true)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			if (::GetAsyncKeyState(VK_DELETE))break;
			
			if ((::GetAsyncKeyState(VK_INSERT) & 0x0001) != 0)
			{				
				glowEnabled = !glowEnabled;
				service_manager->SetEnabled(glowEnabled, "Glow");
			}
				
			if ((::GetAsyncKeyState(VK_HOME) & 0x0001) != 0)
			{				
				fakeLagEnabled = !fakeLagEnabled;
				service_manager->SetEnabled(fakeLagEnabled, "FakeLag");
			}

			cout << "\r" << "Glow[" << glowEnabled << "] FakeLag[" << fakeLagEnabled << "]" << flush;
		}

		std::cout << "\nStopping..." << std::endl;
		service_manager->StopAll();
	}
	catch (std::exception &e)
	{
		cout << e.what() << endl;
	}

	delete service_manager;

	cout << "END\r";	

	return 0;
}
