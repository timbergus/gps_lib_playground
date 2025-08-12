BUILD=build
PROJECT=gps_lib_playground

init:
	cmake -B $(BUILD) \
  	-DCMAKE_C_COMPILER=/opt/homebrew/opt/llvm/bin/clang \
  	-DCMAKE_CXX_COMPILER=/opt/homebrew/opt/llvm/bin/clang++

project:
	cmake --build $(BUILD)

start:
	./$(BUILD)/$(PROJECT)

test:
	ctest --test-dir $(BUILD)

clean:
	rm -rf $(BUILD)
