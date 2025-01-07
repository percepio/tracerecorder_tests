# Percepio TraceRecorder Unit Tests
This folder contains the unit tests for Percepio TraceRecorder using cmocka.

> Licensed under the Apache License, Version 2.0 (the "License");
> you may not use this file except in compliance with the License.
> You may obtain a copy of the License at
>
>     http://www.apache.org/licenses/LICENSE-2.0
>
> Unless required by applicable law or agreed to in writing, software
> distributed under the License is distributed on an "AS IS" BASIS,
> WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
> See the License for the specific language governing permissions and
> limitations under the License.

> The tests are currently performed per API function. This means multiple "unit tests" are performed in each "test".
> These will be refactored and split into separate tests in the future.

> update_mocks.py is used to propagate any changes to a mock to all unit test projects.
> A copy of the "common" mocks are in the unit_test_mocks folder.
> Changing a "common" mock will update that mock in any unit test project that uses it.
> Changing a mock in a unit test project will provide the user the option to update the "common" mock which will then be used to update all other unit tests.

## Usage:
Example commands are for 64-bit and 32-bit Linux or Linux Subsystems for Windows.

> Modify CMakeLists.txt so RECORDER_DIR correctly points at your TraceRecorder source directory (from the "add_subdirectory" projects' perspective).

### Generate build files:
`cmake -S . -B build64 -DCMAKE_C_FLAGS="-m64"`
or
`cmake -S . -B build32 -DCMAKE_C_FLAGS="-m32"`

### Build:
`make -C build64/`
or
`make -C build32/`

### Clean:
`make clean -C build64/`
or
`make clean -C build32/`

### Run all tests:
`run-parts build64/bin`
or
`run-parts build32/bin`

Each test also return the number of errors, so if return value from execution != 0 then an error has occurred.
Example shell script:

	FAILURE=0
	for program in buildXX/bin/*; do
		echo ""
		echo $program
	
		$program
		if [ $? -ne 0 ]; then
			FAILURE=1
		fi
	done
	if [ "$FAILURE" == 1 ]; then
		echo "Unit tests failed!"
		exit 1
	fi
