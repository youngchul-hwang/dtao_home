QT += widgets

HEADERS += \
    dtaorendersystem.h \
    hellovulkanwidget.h \
    ../shared/trianglerenderer.h \
    lve_device.hpp \
    lve_window.h

SOURCES += \
    dtaorendersystem.cpp \
    hellovulkanwidget.cpp \
    lve_device.cpp \
    lve_window.cpp \
    main.cpp \
    ../shared/trianglerenderer.cpp

RESOURCES += hellovulkanwidget.qrc

# install
target.path = $$[QT_INSTALL_EXAMPLES]/vulkan/hellovulkanwidget
INSTALLS += target

unix:!macx|win32: LIBS += -L$$PWD/../../../../Develop_Utils/VulkanSDK/1.3.236.0/Lib/ -lvulkan-1

INCLUDEPATH += $$PWD/../../../../Develop_Utils/VulkanSDK/1.3.236.0/Lib
DEPENDPATH += $$PWD/../../../../Develop_Utils/VulkanSDK/1.3.236.0/Lib


INCLUDEPATH += $$PWD/../../../../Develop_Utils/Libraries/glm
