#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <stdlib.h>
#include <tchar.h>
#include <wchar.h>
#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include <winsvc.h>
#pragma comment(lib, "advapi32.lib")

using namespace std;

SERVICE_STATUS ServiceStatus;  
SERVICE_STATUS_HANDLE hStatus;
LPCSTR serviceName = "Terminator";

LPCSTR base_path_config = (LPCSTR)"C:\\Users\Tommy\source\repos\BSIT3\BSIT3\config.txt";
const int cMaxConfigData = 64;


int addLogMessage(const char* str) 
{
	errno_t err;  
	FILE* log;
	int code = 0;
	if ((err = fopen_s(&log, "info.txt", "a+")) != 0) 
	{ 
		return -1; 
	}
	fprintf(log, "[code: %u] %s\n", code, str);  fclose(log);  
	return 0;
}

void ControlHandler(DWORD request)
{
	switch (request)
	{
	case SERVICE_CONTROL_STOP:
		addLogMessage("Stopped.");
		ServiceStatus.dwWin32ExitCode = 0;
		ServiceStatus.dwCurrentState = SERVICE_STOPPED;
		SetServiceStatus(hStatus, &ServiceStatus);
		return;
	case SERVICE_CONTROL_SHUTDOWN:
		addLogMessage("Shutdown.");
		ServiceStatus.dwWin32ExitCode = 0;
		ServiceStatus.dwCurrentState = SERVICE_STOPPED;
		SetServiceStatus(hStatus, &ServiceStatus);
		return;
	default:
		break;
	}
	SetServiceStatus(hStatus, &ServiceStatus);
	return;
}

int config_fill(char base_path[256], char service_path[256], char config_data[][64])
{
	ifstream file(base_path_config);
	if (!file.is_open())
	{
		cout << "Fail not found!";
		return -1;
	}
	file.getline(base_path, 256);
	file.getline(service_path, 256);
	int index = 0;
	while (index < cMaxConfigData && (!file.eof()))
	{
		file.getline(config_data[index], 64);
		index++;
	}
	return index;
}

int InstallService()
{
	SC_HANDLE hSCManager = OpenSCManager(NULL, NULL,SC_MANAGER_CONNECT | SC_MANAGER_CREATE_SERVICE);
	

	
	if (hSCManager == 0)
	{
		addLogMessage("Error: Can't open Service Control Manager");
		return -1;
	}
	SC_HANDLE hService = CreateService(hSCManager, serviceName, serviceName, SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS, SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL, base_path_config, NULL, NULL, NULL, NULL, NULL);
	if (hService == 0) {
		int err = GetLastError();
		switch (err) {
		case ERROR_ACCESS_DENIED:
			addLogMessage("Error: ERROR_ACCESS_DENIED");
			break;
		case ERROR_CIRCULAR_DEPENDENCY:
			addLogMessage("Error: ERROR_CIRCULAR_DEPENDENCY");
		break; case ERROR_DUPLICATE_SERVICE_NAME:
			addLogMessage("Error: ERROR_DUPLICATE_SERVICE_NAME");
			break;
		case ERROR_INVALID_HANDLE:
			addLogMessage("Error: ERROR_INVALID_HANDLE");
			break;
		case ERROR_INVALID_NAME: addLogMessage("Error: ERROR_INVALID_NAME");
			break;
		case ERROR_INVALID_PARAMETER:
			addLogMessage("Error: ERROR_INVALID_PARAMETER");
			break;
		case ERROR_INVALID_SERVICE_ACCOUNT:
			addLogMessage("Error: ERROR_INVALID_SERVICE_ACCOUNT");
			break;
		case ERROR_SERVICE_EXISTS:
			addLogMessage("Error: ERROR_SERVICE_EXISTS");
			break;
		default: addLogMessage("Error: Undefined");
		} CloseServiceHandle(hSCManager);
		return -1;
	}
	CloseServiceHandle(hService);
	CloseServiceHandle(hSCManager);
	addLogMessage("Success install service!");
	return 0;
}

int RemoveService() 
{
	SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);   
	if (!hSCManager) 
	{ 
		addLogMessage("Error: Can't open Service Control Manager");      
		return -1; 
	}  
	SC_HANDLE hService = OpenService(hSCManager, serviceName, SERVICE_STOP | DELETE);   
	if (!hService) 
	{ 
		addLogMessage("Error: Can't remove service");      
		CloseServiceHandle(hSCManager);      
		return -1; 
	}      
	
	DeleteService(hService);   
	CloseServiceHandle(hService);   
	CloseServiceHandle(hSCManager);   
	addLogMessage("Success remove service!");  
	return 0; 
}

int StartSService() 
{ 
	SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
	SC_HANDLE hService = OpenServiceA(hSCManager, serviceName, SERVICE_START);   
	if (hService == NULL)
	{
		CloseServiceHandle(hSCManager);
		addLogMessage("Error: bad handle");
		return -1;
	}

	if (!StartServiceA(hService, 0, NULL)) 
	{ 
		int err = GetLastError();
		switch (err) {
		case ERROR_ACCESS_DENIED:
			addLogMessage("Error: ERROR_ACCESS_DENIED");
			break;
		case ERROR_INVALID_HANDLE:
			addLogMessage("Error: ERROR_INVALID_HANDLE");
			break;
		case ERROR_PATH_NOT_FOUND:
			addLogMessage("Error: ERROR_PATH_NOT_FOUND");
			break;
		case ERROR_SERVICE_ALREADY_RUNNING:
			addLogMessage("Error: ERROR_SERVICE_ALREADY_RUNNING");
			break;
		case ERROR_SERVICE_DATABASE_LOCKED:
			addLogMessage("ERROR_SERVICE_DATABASE_LOCKED");
			break;
		case ERROR_SERVICE_DEPENDENCY_DELETED:
			addLogMessage("Error: ERROR_SERVICE_DEPENDENCY_DELETED");
			break;
		case ERROR_SERVICE_DEPENDENCY_FAIL:
			addLogMessage("Error: ERROR_SERVICE_DEPENDENCY_FAIL");
			break;
		case ERROR_SERVICE_DISABLED:
			addLogMessage("Error: ERROR_SERVICE_DISABLED");
			break;
		case ERROR_SERVICE_LOGON_FAILED:
			addLogMessage("Error: ERROR_SERVICE_LOGON_FAILED");
			break;
		case ERROR_SERVICE_MARKED_FOR_DELETE:
			addLogMessage("Error: ERROR_SERVICE_MARKED_FOR_DELETE");
			break;
		case ERROR_SERVICE_NO_THREAD:
			addLogMessage("Error: ERROR_SERVICE_NO_THREAD");
			break;
		case ERROR_SERVICE_REQUEST_TIMEOUT:
			addLogMessage("Error: ERROR_SERVICE_REQUEST_TIMEOUT");
			break;
		default: addLogMessage("Error: Undefined");
		}
		CloseServiceHandle(hSCManager);     
		addLogMessage("Error: Can't start service");    
		return -1; 
	}      
	CloseServiceHandle(hService);   
	CloseServiceHandle(hSCManager);   
	return 0; 
}

int StopService()
{
	char com[64];
	sprintf(com, "net stop %ws", serviceName);
	system(com);
	return 0;
}

void ServiceMain(int argc, char** argv) 
{
	ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;    
	ServiceStatus.dwCurrentState = SERVICE_START_PENDING;
	ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
	ServiceStatus.dwWin32ExitCode = 0;
	ServiceStatus.dwServiceSpecificExitCode = 0;
	ServiceStatus.dwCheckPoint = 0;
	ServiceStatus.dwWaitHint = 0;
	//serviceStatusHandle = RegisterServiceCtrlHandler(serviceName, (LPHANDLER_FUNCTION)ControlHandler);
	hStatus = RegisterServiceCtrlHandler(serviceName, (LPHANDLER_FUNCTION)ControlHandler);
	if (hStatus == (SERVICE_STATUS_HANDLE)0)
	{ 
		return; 
	}
	      
	ServiceStatus.dwCurrentState = SERVICE_RUNNING;
	SetServiceStatus(hStatus, &ServiceStatus);



	while (ServiceStatus.dwCurrentState == SERVICE_RUNNING)
	{ 
		char base_path[256] = { 0 };
		char service_path[256] = { 0 };
		char config_data[cMaxConfigData][64] = { 0 };
		int count_config = 0;
		count_config = config_fill(base_path, service_path, config_data);
		if (count_config < 0)
		{
			//error
		}
		for (int i = 0; i < count_config; i++)
		{
			char com[512];
			printf(com, "\"C:\\Program Files\\7-Zip\\7z\" u -ssw -mx2 -r0 %s %s%s", service_path, base_path, config_data[i]);
			system(com);
		}
		Sleep(5000);
	}
	return;
}

int _tmain(int argc, _TCHAR* argv[])
{
	if (argc < 2)
	{
		SERVICE_TABLE_ENTRY ServiceTable[1];
		ServiceTable[0].lpServiceName = (LPSTR)serviceName;
		ServiceTable[0].lpServiceProc = (LPSERVICE_MAIN_FUNCTION)ServiceMain;
		if (!StartServiceCtrlDispatcher(ServiceTable))
		{
			addLogMessage("Error: StartServiceCtrlDispatcher");
		}

	}
	else if (_tcscmp(argv[argc - 1], _T("install")) == 0)
	{
		InstallService();
	}
	else if (_tcscmp(argv[argc - 1], _T("start")) == 0)
	{
		StartSService();
	}
	else if (_tcscmp(argv[argc - 1], _T("stop")) == 0)
	{
		StopService();
	}
	else if (_tcscmp(argv[argc - 1], _T("remove")) == 0)
	{
		RemoveService();
	}
	return 1;
}