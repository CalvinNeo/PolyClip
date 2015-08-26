#ifndef _SCARLET_WIN_SERVICE_H
#define _SCARLET_WIN_SERVICE_H
#include <windows.h>
#include <tchar.h>

namespace scarlet {
	namespace win {
		/*
			windows下的服务类，用于简化服务程序编写
			继承并重写 run stop
			非线程安全，全程序只可存在一个实列
			并在继承类 cpp 文件中定义
			TCHAR scarlet::win::service::m_srvName[128] = { 0 };
			SERVICE_STATUS scarlet::win::service::m_srvStatus = { 0 };
			SERVICE_STATUS_HANDLE scarlet::win::service::m_srvStatusHandle = { 0 };
			scarlet::win::service * scarlet::win::service::m_instance = nullptr;
			*/
		class service {
		public:

			bool initlize(const TCHAR * srvName, service * instance) {
				if (srvName == nullptr || _tcslen(srvName) < 1 || _tcslen(srvName) > 127) {
					return false;
				}

				_tcscpy_s(m_srvName, srvName);
				m_instance = instance;

				return true;
			}

			bool startService() {
				SERVICE_TABLE_ENTRY ServiceTable[] =
				{
					{ m_srvName, (LPSERVICE_MAIN_FUNCTION)serviceMain },
					{ NULL, NULL }
				};

				if (!StartServiceCtrlDispatcher(ServiceTable)) {
					return false;
				}
				return true;
			}

		protected:
			virtual bool init() = 0;
			virtual void run() = 0;
			virtual void stop() = 0;

		private:
			static void WINAPI srvControlHandler(DWORD dwMsg) {
				switch (dwMsg) {
				case SERVICE_CONTROL_STOP:
					m_instance->stop();
					m_srvStatus.dwWin32ExitCode = 0;
					m_srvStatus.dwCurrentState = SERVICE_STOPPED;
					break;
				default:
					break;
				}

				SetServiceStatus(m_srvStatusHandle, &m_srvStatus);
				return;
			}

			static void WINAPI serviceMain(DWORD argc, LPTSTR *argv) {
				m_srvStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
				m_srvStatus.dwCurrentState = SERVICE_STOPPED;
				m_srvStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
				m_srvStatus.dwWin32ExitCode = 0;
				m_srvStatus.dwServiceSpecificExitCode = 0;
				m_srvStatus.dwCheckPoint = 0;
				m_srvStatus.dwWaitHint = 0;

				m_srvStatusHandle = RegisterServiceCtrlHandler(m_srvName, srvControlHandler);

				if (NULL == m_srvStatusHandle) {
					return;
				}

				if (!m_instance->init()) {
					return;
				}

				m_srvStatus.dwCurrentState = SERVICE_RUNNING;
				SetServiceStatus(m_srvStatusHandle, &m_srvStatus);

				m_instance->run();

				m_srvStatus.dwCurrentState = SERVICE_STOPPED;
				m_srvStatus.dwWin32ExitCode = 0;
				SetServiceStatus(m_srvStatusHandle, &m_srvStatus);
			}
		private:
			static TCHAR m_srvName[128];
			static SERVICE_STATUS m_srvStatus;
			static SERVICE_STATUS_HANDLE m_srvStatusHandle;
			static service * m_instance;
		};
	}
}
#endif