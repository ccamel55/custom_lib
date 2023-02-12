
# add everything from common because there common lol
target_sources(custom_lib PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/math_lib/math_common.cpp
        ${CMAKE_CURRENT_LIST_DIR}/math_lib/math_common.hpp
        ${CMAKE_CURRENT_LIST_DIR}/types/matrix/matrix3x4.cpp
        ${CMAKE_CURRENT_LIST_DIR}/types/matrix/matrix3x4.hpp
        ${CMAKE_CURRENT_LIST_DIR}/types/matrix/matrix4x4.cpp
        ${CMAKE_CURRENT_LIST_DIR}/types/matrix/matrix4x4.hpp
        ${CMAKE_CURRENT_LIST_DIR}/types/point/point_float.cpp
        ${CMAKE_CURRENT_LIST_DIR}/types/point/point_float.hpp
        ${CMAKE_CURRENT_LIST_DIR}/types/point/point_int.cpp
        ${CMAKE_CURRENT_LIST_DIR}/types/point/point_int.hpp
        ${CMAKE_CURRENT_LIST_DIR}/types/vector/vector2d.cpp
        ${CMAKE_CURRENT_LIST_DIR}/types/vector/vector2d.hpp
        ${CMAKE_CURRENT_LIST_DIR}/types/vector/vector3d.cpp
        ${CMAKE_CURRENT_LIST_DIR}/types/vector/vector3d.hpp
        ${CMAKE_CURRENT_LIST_DIR}/types/color.cpp
        ${CMAKE_CURRENT_LIST_DIR}/types/color.hpp
        )