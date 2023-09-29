#include <stdio.h>
#include <stdlib.h>
#include <nvml.h>

// Note: Nvidia only provides 64bit nvml.lib/dll, so this program can only be compiled
// for x64 platform.
// Actually, nvmlSystemGetDriverVersion() can report version numbers for GeForce drivers,
// no Quadro/Tesla cards needed.
//
// Ref: https://docs.nvidia.com/deploy/nvml-api/nvml-api-reference.html

int main(int argc, char* argv[])
{
	printf("Hello, nvmlGetDriverVer!\n");

	nvmlReturn_t merr = nvmlInit(); 
	// -- We need to initialize NVML first, otherwise, we will get NVML_ERROR_UNINITIALIZED.
	if(merr) {
		printf("nvmlInit() returns: %d\n", merr);
		return 4;
	}

	char szVer[100] = {};
	 merr = nvmlSystemGetDriverVersion(szVer, sizeof(szVer)-1);
	if(!merr) {
		printf("nvmlSystemGetDriverVersion() returns: %s\n", szVer);
	}
	else {
		printf("nvmlSystemGetDriverVersion() fails, errcode: %d\n", merr);
	}

	nvmlShutdown();

	return merr;
}
