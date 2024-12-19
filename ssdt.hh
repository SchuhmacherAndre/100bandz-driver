#ifndef SSDT_HH
#define SSDT_HH

#pragma once

class c_ssdt {
public:
	// courtesy of https://support.microsoft.com/en-us/topic/kb4072698-windows-server-and-azure-stack-hci-guidance-to-protect-against-silicon-based-microarchitectural-and-speculative-execution-side-channel-vulnerabilities-2f965763-00e2-8f98-b632-0d96f30c8c8e#ID0EBBBBJ=FeatureSettingsOverrideMask
	// reg add "HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\Session Manager\Memory Management" /v FeatureSettingsOverrideMask /t REG_DWORD /d 3 /f"
	// reg add "HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\Session Manager\Memory Management" /v FeatureSettingsOverride /t REG_DWORD /d 3 /f"
	// if you dont use this, you will get shadow ver. of kisyscall on 64bit windows (undesirable)
	PULONGLONG GetSSDT();

	// !! whenever this is gonna see the light of the world u will need to check winver and get the correct syscall index according to windows version
	// https://j00ru.vexillium.org/syscalls/nt/64/ (idx)
	PVOID FindFunctionAddressInSSDT(uintptr_t idx);
};
extern KIRQL o_IRQL;
extern c_ssdt* ssdt;



#endif // SSDT_HH