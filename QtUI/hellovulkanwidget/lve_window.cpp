#include "lve_window.h"
#include "hellovulkanwidget.h"
#include "dtaorendersystem.h"

LveWindow::LveWindow()
{
}

LveWindow::~LveWindow()
{
    //this->destroy();
}

QVulkanWindowRenderer *LveWindow::createRenderer()
{
    //return new VulkanRenderer(this);
    return new DtaoRenderSystem(this);
    //return (QVulkanWindowRenderer *)nullptr;
}
/*
inline VkExtent2D LveWindow::getExtent() {
    return { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };
}
*/
