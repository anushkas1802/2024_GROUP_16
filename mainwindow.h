#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QModelIndex>
#include <QDir>


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
    void statusUpdateMessageSignal(const QString &message, int timeout); // ✅ Signal for status updates

private slots:
    void handleButton();
    void updateRender();
    void updateRenderFromTree(const QModelIndex& index);
    void handleOpenOptions();
    void handleTreeClicked();
    void on_actionOpenFile_triggered();
    void on_actionItemOptions_triggered();
    void statusUpdateMessage(const QString &message, int timeout); // ✅ Slot for displaying status messages
    void loadInitialPartsFromFolder(const QString& folderPath);
    void loadPartsRecursively(const QDir& dir, ModelPart* parentItem);

private:
    QModelIndex contextMenuIndex;  // To track right-clicked item

private:
    Ui::MainWindow *ui;
    ModelPartList* partList;
    // VTK Rendering Components
    vtkSmartPointer<vtkRenderer> renderer;
    vtkSmartPointer<vtkGenericOpenGLRenderWindow> renderWindow;

    void setupVTK(); // ✅ Function to initialize VTK rendering
    void showContextMenu(const QPoint &pos);
};

#endif // MAINWINDOW_H
