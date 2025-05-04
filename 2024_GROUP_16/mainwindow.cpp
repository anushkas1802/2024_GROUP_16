/**
 * @file mainwindow.cpp
 * @brief Implementation of the MainWindow class for handling STL file visualization and VR interaction.
 *
 * This file defines the main window UI logic for managing STL file loading, rendering using VTK, and VR rendering in a Qt application.
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ModelPart.h"
#include "ModelPartList.h"
#include "optiondialog.h"
#include "VRRenderThread.h"

 // Qt includes
#include <QFileDialog>
#include <QMenu>
#include <QMessageBox>
#include <QFileInfo>
#include <QDir>
#include <QFileInfoList>
#include <QDebug>

// VTK includes
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkCamera.h>
#include <vtkNew.h>
#include <vtkSmartPointer.h>
#include <vtkNamedColors.h>
#include <vtkCylinderSource.h>
#include <vtkSTLReader.h>
#include <vtkDataSetMapper.h>
#include <vtkCallbackCommand.h>

/**
 * @brief Constructs the MainWindow and sets up UI components and signal connections.
 * @param parent The parent widget.
 */
MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Signal-slot connections
    connect(ui->addButton, &QPushButton::released, this, &MainWindow::handleButton);
    connect(ui->openOptions, &QPushButton::released, this, &MainWindow::handleOpenOptions);
    connect(this, &MainWindow::statusUpdateMessageSignal, ui->statusbar, &QStatusBar::showMessage);
    connect(ui->startVRButton, &QPushButton::clicked, this, &MainWindow::handleStartVR);
    connect(ui->stopVRButton, &QPushButton::clicked, this, &MainWindow::handleStopVR);

    // Tree view setup
    this->partList = new ModelPartList("PartsList");
    ui->treeView->setModel(this->partList);
    ui->treeView->addAction(ui->actionItemOptions);
    ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeView, &QTreeView::customContextMenuRequested, this, &MainWindow::showContextMenu);
    connect(ui->treeView, &QTreeView::clicked, this, &MainWindow::handleTreeClicked);

    // Menu actions
    connect(ui->actionOpenSingleFile, &QAction::triggered, this, &MainWindow::on_actionOpenSingleFile_triggered);
    connect(ui->actionClearTreeView, &QAction::triggered, this, &MainWindow::on_actionClearTreeView_triggered);

    setupVTK();

    emit statusUpdateMessageSignal("Loaded Level0 parts (invisible)", 2000);
    vrThread = new VRRenderThread(this);
}

/**
 * @brief Destructor. Ensures any running VR thread is safely stopped before closing.
 */
MainWindow::~MainWindow()
{
    if (vrThread && vrThread->isRunning())
    {
        vrThread->issueCommand(VRRenderThread::END_RENDER, 0.0);
        vrThread->wait();
    }
    delete vrThread;
    delete ui;
}

/**
 * @brief Initializes the VTK rendering system.
 */
void MainWindow::setupVTK()
{
    if (!ui->vtkWidget) {
        qWarning("vtkWidget is not initialized in the UI file!");
        return;
    }

    renderWindow = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
    ui->vtkWidget->setRenderWindow(renderWindow);

    renderer = vtkSmartPointer<vtkRenderer>::New();
    renderWindow->AddRenderer(renderer);
    renderer->SetBackground(0.1, 0.1, 0.1);

    renderWindow->Render();
}

/**
 * @brief Updates the status bar with a given message.
 * @param message Message to show.
 * @param timeout Duration in milliseconds.
 */
void MainWindow::statusUpdateMessage(const QString& message, int timeout)
{
    ui->statusbar->showMessage(message, timeout);
}

/**
 * @brief Simple test button handler.
 */
void MainWindow::handleButton()
{
    QMessageBox::information(this, "Test", "Add button was clicked");
    emit statusUpdateMessageSignal("Add button was clicked", 2000);
}

/**
 * @brief Opens the option dialog for the currently selected tree item.
 */
void MainWindow::on_actionItemOptions_triggered()
{
    QModelIndex index = ui->treeView->currentIndex();
    ModelPart* selectedPart = static_cast<ModelPart*>(index.internalPointer());

    if (!selectedPart) {
        QMessageBox::warning(this, "No Selection", "Please select an item first.");
        return;
    }

    OptionDialog optionDialog(this);
    QColor currentColor(selectedPart->getColourR(), selectedPart->getColourG(), selectedPart->getColourB());
    optionDialog.setValues(selectedPart->data(0).toString(), currentColor, selectedPart->visible());

    if (optionDialog.exec() == QDialog::Accepted) {
        selectedPart->setData(0, optionDialog.getName());
        QColor chosenColor = optionDialog.getColor();
        selectedPart->setColour(chosenColor.red(), chosenColor.green(), chosenColor.blue());
        selectedPart->setVisible(optionDialog.isVisible());

        partList->dataChanged(index, index);
        updateRender();
        emit statusUpdateMessageSignal("Updated item options", 2000);
    }
}

/**
 * @brief Opens option dialog manually for testing.
 */
void MainWindow::handleOpenOptions()
{
    OptionDialog optionDialog(this);
    optionDialog.exec();
    emit statusUpdateMessageSignal("Open Options button was clicked", 2000);
}

/**
 * @brief Handles clicks in the tree view and displays selected item.
 */
void MainWindow::handleTreeClicked()
{
    QModelIndex index = ui->treeView->currentIndex();
    ModelPart* selectedPart = static_cast<ModelPart*>(index.internalPointer());

    if (selectedPart) {
        QString text = selectedPart->data(0).toString();
        emit statusUpdateMessageSignal("Selected item: " + text, 2000);
    }
}

/**
 * @brief Opens a folder and loads all STL parts recursively.
 */
void MainWindow::on_actionOpenFile_triggered()
{
    QString folderPath = QFileDialog::getExistingDirectory(this, "Select Repository Folder", QDir::homePath());

    if (!folderPath.isEmpty()) {
        partList->clear();
        renderer->RemoveAllViewProps();
        loadInitialPartsFromFolder(folderPath);
    }
}

/**
 * @brief Shows a context menu when user right-clicks in the tree view.
 */
void MainWindow::showContextMenu(const QPoint& pos)
{
    QModelIndex index = ui->treeView->indexAt(pos);
    if (!index.isValid()) return;

    ui->treeView->setCurrentIndex(index);
    QMenu contextMenu(this);
    contextMenu.addAction(ui->actionItemOptions);
    contextMenu.exec(ui->treeView->viewport()->mapToGlobal(pos));
}

/**
 * @brief Updates the VTK scene with visible actors from the model tree.
 */
void MainWindow::updateRender()
{
    renderer->RemoveAllViewProps();

    int topLevelCount = partList->rowCount(QModelIndex());
    for (int i = 0; i < topLevelCount; ++i) {
        QModelIndex topIndex = partList->index(i, 0, QModelIndex());
        updateRenderFromTree(topIndex);
    }

    if (renderer->GetActors()->GetNumberOfItems() > 0)
        renderer->ResetCamera();

    renderWindow->Render();
}

/**
 * @brief Recursively adds visible actors to the renderer.
 */
void MainWindow::updateRenderFromTree(const QModelIndex& index)
{
    if (!index.isValid()) return;

    ModelPart* selectedPart = static_cast<ModelPart*>(index.internalPointer());

    if (selectedPart && selectedPart->visible()) {
        vtkSmartPointer<vtkActor> actor = selectedPart->getActor();
        if (actor) renderer->AddActor(actor);
    }

    int rows = partList->rowCount(index);
    for (int i = 0; i < rows; i++) {
        updateRenderFromTree(partList->index(i, 0, index));
    }
}

/**
 * @brief Loads STL parts from the specified directory.
 * @param folderPath Path to the folder.
 */
void MainWindow::loadInitialPartsFromFolder(const QString& folderPath)
{
    QDir dir(folderPath);
    if (!dir.exists()) {
        qDebug() << "Directory does not exist:" << folderPath;
        return;
    }

    loadPartsRecursively(dir, partList->getRootItem());
    updateRender();
}

/**
 * @brief Starts the VR rendering thread and sends actors.
 */
void MainWindow::handleStartVR()
{
    if (vrThread && vrThread->isRunning()) {
        QMessageBox::information(this, "VR", "VR is already running.");
        return;
    }

    if (vrThread) vrThread->deleteLater();
    vrThread = new VRRenderThread();

    addVisiblePartsToVR(vrThread);
    vrThread->start();

    emit sendActors(renderer->GetActors());
    emit statusUpdateMessageSignal("VR started", 2000);
}

/**
 * @brief Recursively loads STL parts from a directory and adds to model tree.
 * @param dir Directory to scan.
 * @param parentItem Parent tree item.
 */
void MainWindow::loadPartsRecursively(const QDir& dir, ModelPart* parentItem)
{
    QStringList filters = { "*.stl", "*.STL" };

    QFileInfoList fileList = dir.entryInfoList(filters, QDir::Files);
    for (const QFileInfo& fileInfo : fileList) {
        QString filePath = fileInfo.absoluteFilePath();
        ModelPart* part = new ModelPart({ fileInfo.fileName(), 0 }, parentItem);
        parentItem->appendChild(part);
        part->loadSTL(filePath);
        part->setVisible(false);
    }

    QFileInfoList dirList = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (const QFileInfo& subdirInfo : dirList) {
        QDir subdir(subdirInfo.absoluteFilePath());
        ModelPart* folderItem = new ModelPart({ subdirInfo.fileName(), 0 }, parentItem);
        parentItem->appendChild(folderItem);
        loadPartsRecursively(subdir, folderItem);
    }
}

/**
 * @brief Collects visible parts and passes them to the VR thread.
 * @param thread VR render thread instance.
 */
void MainWindow::addVisiblePartsToVR(VRRenderThread* thread)
{
    int topLevelCount = partList->rowCount(QModelIndex());
    for (int i = 0; i < topLevelCount; ++i) {
        QModelIndex topIndex = partList->index(i, 0, QModelIndex());
        addPartsFromTree(topIndex, thread);
    }
}

/**
 * @brief Adds individual part actor to the VR thread.
 * @param index Model index of the part.
 * @param thread VR render thread.
 */
void MainWindow::addPartsFromTree(const QModelIndex& index, VRRenderThread* thread)
{
    ModelPart* selectedPart = static_cast<ModelPart*>(index.internalPointer());
    if (selectedPart->visible()) {
        vtkSmartPointer<vtkActor> actor = selectedPart->getNewActor();
        if (actor) thread->addActorOffline(actor);
    }

    int rows = partList->rowCount(index);
    for (int i = 0; i < rows; i++) {
        addPartsFromTree(partList->index(i, 0, index), thread);
    }
}

/**
 * @brief Loads a single STL file and displays it.
 */
void MainWindow::on_actionOpenSingleFile_triggered()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Open STL File", QDir::homePath(), "STL Files (*.stl *.STL)");
    if (filePath.isEmpty()) return;

    QFileInfo fileInfo(filePath);
    partList->addPart(fileInfo.fileName(), filePath);
    updateRender();
    emit statusUpdateMessageSignal("Loaded single file: " + fileInfo.fileName(), 2000);
}

/**
 * @brief Clears the model tree and the render view.
 */
void MainWindow::on_actionClearTreeView_triggered()
{
    partList->clear();
    renderer->RemoveAllViewProps();
    renderWindow->Render();
    emit statusUpdateMessageSignal("Tree view and VTK scene cleared", 2000);
}

/**
 * @brief Safely stops the VR thread.
 */
void MainWindow::handleStopVR()
{
    if (vrThread && vrThread->isRunning()) {
        vrThread->issueCommand(VRRenderThread::END_RENDER, 0.0);
        vrThread->wait();
        emit statusUpdateMessageSignal("VR thread stopped", 2000);
    }
    else {
        emit statusUpdateMessageSignal("VR thread was not running", 2000);
    }
}
