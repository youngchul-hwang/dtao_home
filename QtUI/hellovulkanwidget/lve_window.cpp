#include "lve_window.h"
#include "hellovulkanwidget.h"

LveWindow::LveWindow() : QVulkanWindow()
{

    //this->setSurfaceType(SurfaceType::VulkanSurface);
    //this->create();

}

LveWindow::~LveWindow()
{
    //this->destroy();
}

QVulkanWindowRenderer *LveWindow::createRenderer()
{
    //return new VulkanRenderer(this);
    return (QVulkanWindowRenderer *)nullptr;
}

void LveWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR *surface){

    //*surface = QVulkanInstance::surfaceForWindow(this->parent());
}
