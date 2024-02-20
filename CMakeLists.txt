cmake_minimum_required(VERSION 3.10)
project(PickingBenchmark)

find_package(VTK COMPONENTS
  CommonCore
  CommonDataModel
  CommonSystem
  FiltersSources
  InteractionStyle
  RenderingOpenGL2
  )

add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${PROJECT_NAME}.cxx)
target_link_libraries(${PROJECT_NAME} PRIVATE ${VTK_LIBRARIES})
vtk_module_autoinit(
  TARGETS ${PROJECT_NAME}
  MODULES ${VTK_LIBRARIES}
  )
