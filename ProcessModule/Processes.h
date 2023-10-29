#ifndef SL_CHILD_PROCESSES
#define SL_CHILD_PROCESSES

#include <windows.h>
#include <atlstr.h>
#include <string>

#include <locale>
#include <codecvt>
#include <string>

#include <iostream>



class SL_Pipe{
	
	SECURITY_ATTRIBUTES saAttr = { sizeof(SECURITY_ATTRIBUTES) };
	public:
	HANDLE PipeOut, PipeIn;
	bool isClosed = true;



	SL_Pipe(){
		saAttr.bInheritHandle = TRUE; // Pipe handles are inherited by child process.
		saAttr.lpSecurityDescriptor = NULL;
	};
	~SL_Pipe(){
		Close();
	}

	bool Open(){
		// Create a pipe
		isClosed = false;
		return CreatePipe(&PipeOut, &PipeIn, &saAttr, 0);
	};
	void Close(){
		if (isClosed)
			return;
		std::cout << "close\n"; //FIXME: not to std
  		CloseHandle(PipeOut);
  		CloseHandle(PipeIn);
		isClosed = true;
	};

};


/**
 * @brief 
 * This process after launch will live on even after parent process is terminated.
 * You need to kill this process before parent closes.
 */
class Child_Process
{

	SL_Pipe	pipe;

	STARTUPINFOW si = { sizeof(STARTUPINFOW) };
	PROCESS_INFORMATION pi = { 0 };

	bool isProcessCreated = false;
	bool isProcessEnded = false;
	bool isProcessClosed = false;

	private:
	void init_Process(std::wstring programPath,std::wstring args){
		isProcessCreated = false;
		isProcessEnded = false;

		bool ispipeSucces = pipe.Open();

		if(!pipe.Open())
		{
			std::cout << GetLastError() << "\n"; //FIXME: not to std
			pipe.Close();
		}


		si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
		si.hStdOutput = pipe.PipeIn;
		si.hStdError = pipe.PipeIn;
		si.hStdInput = NULL;
		si.wShowWindow = SW_NORMAL; // Prevents cmd window from flashing.
		// Requires STARTF_USESHOWWINDOW in dwFlags.

		isProcessCreated = CreateProcessW((LPWSTR)L"D:/Programing/Untracked/Processes/exampleprogram/x64/Debug/exampleprogram.exe",	//[in, optional]      LPCSTR                lpApplicationName,
										(LPWSTR)L"helo asd1 asd2 asd3",	//[in, out, optional] LPSTR                 lpCommandLine,
											NULL,	//[in, optional]      LPSECURITY_ATTRIBUTES lpProcessAttributes,
											NULL,	//[in, optional]      LPSECURITY_ATTRIBUTES lpThreadAttributes,
											TRUE,	//[in]                BOOL                  bInheritHandles,
											CREATE_NEW_CONSOLE,	//[in]                DWORD                 dwCreationFlags, //FIXME: remove this flag
											NULL,	//[in, optional]      LPVOID                lpEnvironment,
											NULL,	//[in, optional]      LPCSTR                lpCurrentDirectory,
											&si,	//[in]                LPSTARTUPINFOA        lpStartupInfo,
											&pi);	//[out]               LPPROCESS_INFORMATION lpProcessInformation


		if(!isProcessCreated)
		{
			pipe.Close();
		}
	};
	/**
	 * @brief Closes the process, clean up after
	 * Does not terminate, the process, will wait for the process to end
	 */
	void close_process(){
		if (isProcessClosed)
			return;
		CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
		pipe.Close();
		isProcessClosed = true;
	}
	
	public:
	/**
	 * @brief Get the output of the process since the last read
	 * If nothing is present will return empty string, does not block caller
	 * @return std::string 
	 */
	std::string get_output()
	{
		if(isProcessEnded)
			return "";
		check_if_process_ended();

		std::string retval = "";

		for (;;)
		{
			char buf[1024];
			DWORD dwRead = 0;
			DWORD dwAvail = 0;

			if (!::PeekNamedPipe(pipe.PipeOut, NULL, 0, NULL, &dwAvail, NULL))
				break;

			if (!dwAvail) // No data available, return
				break;

			if (!::ReadFile(pipe.PipeOut, buf, min(sizeof(buf) - 1, dwAvail), &dwRead, NULL) || !dwRead)
				// Error, the child process might ended
				break;

			buf[dwRead] = 0;
			retval += buf;
			// std::cout << buf;
		}
		return retval;
	}
	void kill_process(){
		UINT a = -1;
        TerminateProcess(pi.hProcess,a); //TODO check what this "a" does
		//close_process(); // FIXME: may make a last read before tarminating
	}

	bool isProcess_Ended()
	{
        return isProcessEnded;
	}
	bool check_if_process_ended()
	{
        return isProcessEnded = (WaitForSingleObject(pi.hProcess, 0) == WAIT_OBJECT_0);
	}
	Child_Process(std::string programPath,std::string args){
		static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		init_Process(converter.from_bytes(programPath),converter.from_bytes(args));
	};
	Child_Process(std::wstring programPath,std::wstring args){
		init_Process(programPath,args);
	};
	~Child_Process(){
		close_process();
	};
};



#endif
