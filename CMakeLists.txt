cmake_minimum_required(VERSION 3.16)

# Настройки проекта: используем C++17 и виджеты Qt 6.
project(lab4 VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_standard_project_setup()

# Собираем приложение из исходников, заголовков и описания интерфейса.
qt_add_executable(lab4
    main.cpp
    mainwindow.cpp
    mainwindow.h
    substitutioncipher.cpp
    substitutioncipher.h
    mainwindow.ui
)

target_link_libraries(lab4 PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
# Передаём приложению каталог проекта для разрешения относительного пути результата.
target_compile_definitions(lab4 PRIVATE LAB4_PROJECT_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

set_target_properties(lab4 PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
# Устанавливаем исполняемый файл в стандартный для платформы каталог.
install(TARGETS lab4
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
