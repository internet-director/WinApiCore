# WinApi core and some utils 

## Build mode

- Release: noncrt mode, exception dont supported
- Debug: default

## Core:
- dynamic import from dll aka API(DLL, Function) in wobf, multithreading supported(optional)
- direct syscall aka SYS(NtFunction) in wobf, multithreading doesn't supported, if NtFunction does not finded, return NtAlwaysError(returned NT(-1))
- memory basic utils
- winapi implementation on ntapi
- multithreading tools (mutex, condition_variable, jthread)
### Deprecated:
- processMonitor(search existing process) and process(open existing process or create new process)

## Utils
- ls: list all files in the current directory
- kill: kill a process by name
- waiter: test program, infinity(no) sleep
