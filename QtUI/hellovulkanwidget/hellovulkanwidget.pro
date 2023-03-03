QT += widgets

HEADERS += \
    LayoutModel.h \
    LayoutPEXData.h \
    PEXCapacitorModel.h \
    PEXResistorModel.h \
    dtaorendersystem.h \
    hellovulkanwidget.h \
    ../shared/trianglerenderer.h \
    lve_camera.hpp \
    lve_device.hpp \
    lve_game_object.hpp \
    lve_model.hpp \
    lve_pipeline.hpp \
    lve_window.h \
    rapidcsv.h \
    simple_render_system.hpp \
    utils.h

SOURCES += \
    LayoutData.cpp \
    LayoutModel.cpp \
    PEXCapacitorModel.cpp \
    PEXData.cpp \
    PEXResistorModel.cpp \
    dtaorendersystem.cpp \
    hellovulkanwidget.cpp \
    lve_camera.cpp \
    lve_device.cpp \
    lve_model.cpp \
    lve_pipeline.cpp \
    lve_window.cpp \
    main.cpp \
    ../shared/trianglerenderer.cpp \
    simple_render_system.cpp

RESOURCES += hellovulkanwidget.qrc


QMAKE_CXXFLAGS += -std=c++17

# install
target.path = $$[QT_INSTALL_EXAMPLES]/vulkan/hellovulkanwidget
INSTALLS += target

unix:!macx|win32: LIBS += -L$$PWD/../../../../Develop_Utils/VulkanSDK/1.3.236.0/Lib/ -lvulkan-1

INCLUDEPATH += $$PWD/../../../../Develop_Utils/VulkanSDK/1.3.236.0/Lib
DEPENDPATH += $$PWD/../../../../Develop_Utils/VulkanSDK/1.3.236.0/Lib


INCLUDEPATH += $$PWD/../../../../Develop_Utils/Libraries/glm

DISTFILES += \
    Data/MVP_PEX_RC_r1_0130.csv \
    Data/MVP_PEX_RC_r1_0130_0208version.csv \
    Data/layout_input_data.csv \
    Data/layout_input_data_old_format.csv \
    Data/pex_cap_test.csv \
    Data/pex_res_test.csv \
    Data/pex_res_test_beol_only.csv \
    Data/pex_res_test_simple.csv \
    Data/test_espin.csv \
    shaders/compile.bat \
    shaders/simple_shader.frag \
    shaders/simple_shader.frag.spv \
    shaders/simple_shader.frag_edge.spv \
    shaders/simple_shader.frag_pex.spv \
    shaders/simple_shader.vert \
    shaders/simple_shader.vert.spv \
    shaders/simple_shader.vert_edge.spv \
    shaders/simple_shader.vert_pex.spv \
    shaders/simple_shader_edge.frag \
    shaders/simple_shader_edge.vert \
    shaders/simple_shader_pex.frag \
    shaders/simple_shader_pex.vert
