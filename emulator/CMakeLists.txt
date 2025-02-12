cmake_minimum_required(VERSION "3.22.1")

project("emulator")

add_executable("Emulator" 
	"src/Application.cpp"
	"src/main.cpp" 
	"src/CPU/CPU.cpp" 
	"src/CPU/Opcodes.cpp"
	"src/CPU/OpcodeMapping.cpp"
	"src/MemoryBus.cpp"
)

set_property(TARGET "Emulator" PROPERTY CXX_STANDARD 17)

target_link_libraries("Emulator" PUBLIC thirdparty)

target_include_directories("Emulator"
PUBLIC
	"$<TARGET_PROPERTY:thirdparty,INTERFACE_INCLUDE_DIRECTORIES>"
	"include/"
)
