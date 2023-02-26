#include "lve_window.h"
#include "hellovulkanwidget.h"

LveWindow::LveWindow()
{

}

LveWindow::~LveWindow()
{

}

QVulkanWindowRenderer *LveWindow::createRenderer()
{
    return new VulkanRenderer(this);
    //return (QVulkanWindowRenderer *)nullptr;
}


