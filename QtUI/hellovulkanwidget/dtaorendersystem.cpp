
#include "dtaorendersystem.h"

#include <QVulkanWindow>
#include <QVulkanInstance>
#include <vulkan/vulkan.h>

DtaoRenderSystem::DtaoRenderSystem(LveWindow *w)
    : lveWindow(w)
{
    qDebug() << "\n\n$$$$$ DtaoRenderSystem()";
}

DtaoRenderSystem::~DtaoRenderSystem()
{
    qDebug() << "\n\n$$$$$ ~DtaoRenderSystem()";
    deleteLveDevice();
    deleteLveRenderer();
}

void DtaoRenderSystem::startNextFrame(){

}

void DtaoRenderSystem::initResources() {
    qDebug() << "\n\n$$$$$ DtaoRenderSystem::initResources()";
    createLveDevice();
    createLveRenderer();
}

void DtaoRenderSystem::releaseResources() {
    qDebug() << "\n\n$$$$$ DtaoRenderSystem::releaseResources()";
}

void DtaoRenderSystem::initSwapChainResources() {
    qDebug() << "\n\n$$$$$ DtaoRenderSystem::initSwapChainResources()";
    QSize image_size = this->lveWindow->swapChainImageSize();
    VkExtent2D extent;
    extent.width = image_size.width();
    extent.height = image_size.height();
    this->lveRenderer->setExtent(extent);
    this->lveRenderer->recreateSwapChain();
}


void DtaoRenderSystem::releaseSwapChainResources() {
    qDebug() << "\n\n$$$$$ DtaoRenderSystem::releaseSwapChainResources()";
}

void DtaoRenderSystem::createLveDevice(){
    qDebug() << "\n\n$$$$$ DtaoRenderSystem::creatLveDevice()";
    this->lveDevice = new LveDevice(this->lveWindow, this->lveWindow->vulkanInstance());
}

void DtaoRenderSystem::deleteLveDevice(){
    qDebug() << "\n\n$$$$$ DtaoRenderSystem::deleteLveDevice()";
    delete this->lveDevice;
}

void DtaoRenderSystem::createLveRenderer(){
    qDebug() << "\n\n$$$$$ DtaoRenderSystem::createLveRenderer()";
    this->lveRenderer = new LveRenderer(*this->lveWindow, *this->lveDevice);
}

void DtaoRenderSystem::deleteLveRenderer(){
    qDebug() << "\n\n$$$$$ DtaoRenderSystem::deleteLveRenderer()";
    delete this->lveRenderer;
}
