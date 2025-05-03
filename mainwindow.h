#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QModelIndex>
#include <QDir>

#include "VRRenderThread.h"

// Forward declarations
class ModelPart;
class ModelPartList;

// VTK includes
#include <vtkSmartPointer.h>
#include <vtkRenderer.h>
#include <vtkGenericOpenGLRenderWindow.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void statusUpdateMessageSignal(const QString &message, int timeout);
    void sendActors(vtkActorCollection* actors);

private slots:
    void handleButton();
    void updateRender();
    void updateRenderFromTree(const QModelIndex& index);
    void handleOpenOptions();
    void handleTreeClicked();
    void on_actionOpenFile_triggered();
    void on_actionOpenSingleFile_triggered();
    void on_actionItemOptions_triggered();
    void statusUpdateMessage(const QString &message, int timeout); 
    void loadInitialPartsFromFolder(const QString& folderPath);
    void loadPartsRecursively(const QDir& dir, ModelPart* parentItem);
    void startVRRendering();
    void handleStartVR();
    void on_actionClearTreeView_triggered();
    void handleStopVR();
private:
    QModelIndex contextMenuIndex;  // To track right-clicked item

private:
    VRRenderThread* vrThread = nullptr;


private:
    Ui::MainWindow *ui;
    ModelPartList* partList;
    // VTK Rendering Components
    vtkSmartPointer<vtkRenderer> renderer;
    vtkSmartPointer<vtkGenericOpenGLRenderWindow> renderWindow;

    void setupVTK(); 
    void showContextMenu(const QPoint &pos);

    void addVisiblePartsToVR(VRRenderThread* thread);
    void addPartsFromTree(const QModelIndex& index, VRRenderThread* thread);
};

#endif // MAINWINDOW_H
