#include <stdio.h>
#include <stdlib.h>

char* test_components[] =
{
	"UnitTest_trcCounter",
	"UnitTest_trcDiagnostics",
	"UnitTest_trcEntryTable",
	"UnitTest_trcError",
	"UnitTest_trcEvent",
	"UnitTest_trcEventBuffer",
	"UnitTest_trcExtension",
	"UnitTest_trcHeap",
	"UnitTest_trcInterval",
	"UnitTest_trcISR",
	"UnitTest_trcMultiCoreEventBuffer",
	"UnitTest_trcObject",
	"UnitTest_trcPrint",
	"UnitTest_trcRunnable",
	"UnitTest_trcStackMonitor",
	"UnitTest_trcStateMachine",
	"UnitTest_trcStaticBuffer",
	"UnitTest_trcString",
	"UnitTest_trcTask",
	"UnitTest_trcTimestamp"/*,
	"UnitTest_trcEventVerification" not 64-bit compatible*/
};

char* test_kernelports[] =
{
	""
};

char* test_streamports[] =
{
	""
};

int main(void)
{
	int i;
	int ret = 0;

	printf("UNIT TESTS BEGIN\n\n");

	printf("Testing components:\n\n");
	for (i = 0; i < sizeof(test_components) / sizeof(char*); i++)
	{
		if (test_components[i][0] == 0)
		{
			continue;
		}
		ret += system(test_components[i]);
		printf("\n");
	}
	printf("Testing components done.\n\n");

	printf("Testing kernelports:\n\n");
	for (i = 0; i < sizeof(test_kernelports) / sizeof(char*); i++)
	{
		if (test_kernelports[i][0] == 0)
		{
			continue;
		}
		ret += system(test_kernelports[i]);
		printf("\n");
	}
	printf("Testing kernelports done.\n\n");

	printf("Testing streamports:\n\n");
	for (i = 0; i < sizeof(test_streamports) / sizeof(char*); i++)
	{
		if (test_streamports[i][0] == 0)
		{
			continue;
		}
		ret += system(test_streamports[i]);
		printf("\n");
	}
	printf("Testing streamports done.\n\n");

	printf("UNIT TESTS END\n\n");

	printf("UNIT TEST ERRORS: %d\n\n", ret);

	printf("--------------------------------------\n\n");

	return ret;
}
