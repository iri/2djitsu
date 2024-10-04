
# Минимальная версия CMake
cmake_minimum_required(VERSION 3.10)

# Название проекта
project(2djitsuProject)

# Устанавливаем стандарт C++
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Указываем исходные файлы
set(SOURCE_FILES 2dj.cpp)

# Включаемые директории
include_directories(.)

# Указываем пути к библиотекам
link_directories(C:/MinGW/lib)

# Указываем библиотеки для линковки
set(LIBS mingw32 SDL2main SDL2 SDL2_image yaml-cpp)

# Опции для компилятора
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

# Создаем исполняемый файл
add_executable(2dj ${SOURCE_FILES})

# Линкуем библиотеки
target_link_libraries(2dj ${LIBS})

# Включаемые директории для MinGW
include_directories(C:/MinGW/include)

# Поиск и включение библиотеки yaml-cpp
find_package(yaml-cpp REQUIRED)
include_directories(${YAML_CPP_INCLUDE_DIRS})
target_link_libraries(2dj ${YAML_CPP_LIBRARIES})

# Указываем путь для итогового файла
set_target_properties(2dj PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/.."
)

