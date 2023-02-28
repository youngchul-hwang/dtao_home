#ifndef LVEWINDOW_H
#define LVEWINDOW_H

#include <QVulkanWindow>
class QVulkanWindowRenderer;

class LveWindow : public QVulkanWindow
{
    Q_OBJECT

public:
    LveWindow();
    ~LveWindow();

    LveWindow(const LveWindow &) = delete;
    LveWindow &operator=(const LveWindow &) = delete;

public:
    QVulkanWindowRenderer *createRenderer() override;
    //VkExtent2D getExtent();

signals:
    void vulkanInfoReceived(const QString &text);
    void frameQueued(int colorValue);
};

#endif // LVEWINDOW_H
