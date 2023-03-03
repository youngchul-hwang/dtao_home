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
    qDebug() << "\n$$$$$ LveWindow::createRenderer()";
    return new DtaoRenderSystem(this);

}

