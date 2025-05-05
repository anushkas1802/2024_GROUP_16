// Includes of the code file headers
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ModelPart.h"
#include "ModelPartList.h"
#include "optiondialog.h"
#include "VRRenderThread.h"

// Q includes
#include <QFileDialog>
#include <QMenu>
#include <QMessageBox>
#include <QFileInfo>
#include <QDir>
#include <QFileInfoList>
#include <QDebug>

// VTK headers
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
#include <vtkDataSetmapper.h>
#include <vtkCallbackCommand.h>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    connect(this, &MainWindow::statusUpdateMessageSignal, ui->statusbar, &QStatusBar::showMessage);
    connect(ui->startVRButton, &QPushButton::clicked, this, &MainWindow::handleStartVR);

    this->partList = new ModelPartList("PartsList");
    ui->treeView->setModel(this->partList);
    ui->treeView->addAction(ui->actionItemOptions);
    ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeView, &QTreeView::customContextMenuRequested, this, &MainWindow::showContextMenu);
    connect(ui->treeView, &QTreeView::clicked, this, &MainWindow::handleTreeClicked);

    connect(ui->actionOpenSingleFile, &QAction::triggered, this, &MainWindow::on_actionOpenSingleFile_triggered);
    connect(ui->actionClearTreeView, &QAction::triggered, this, &MainWindow::on_actionClearTreeView_triggered);

    connect(ui->stopVRButton, &QPushButton::clicked, this, &MainWindow::handleStopVR);


    setupVTK();

    emit statusUpdateMessageSignal("Loaded Level0 parts (invisible)", 2000);

    vrThread = new VRRenderThread(this);

}

// Destructor - Stops the VrThread from running, incase it is active when program is being closed
MainWindow::~MainWindow()
{
    if (vrThread && vrThread->isRunning())
    {
        vrThread->issueCommand(VRRenderThread::END_RENDER, 0.0);
        vrThread->wait(); //Wait for thread to safely exit
    }
    delete vrThread;
    delete ui;
}

void MainWindow::setupVTK()
{
    if (!ui->vtkWidget) {
        // Sends a warning if the vtkWidget is missing from the ui_mainwindow file
        qWarning("vtkWidget is not initialized in the UI file!");
        return;
    }
    // Creates an OpenGL window and connects it to the vtkWidget
    renderWindow = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
    ui->vtkWidget->setRenderWindow(renderWindow);
    // Creates a new renderer, which draws the 3d scene, ands the renderer renderwindow
    renderer = vtkSmartPointer<vtkRenderer>::New();
    renderWindow->AddRenderer(renderer);
    // Sets background colour to grey
    renderer->SetBackground(0.1, 0.1, 0.1);
    // Triggers initial render 
    renderWindow->Render();
}

void MainWindow::statusUpdateMessage(const QString& message, int timeout)
{
    ui->statusbar->showMessage(message, timeout);
}


void MainWindow::on_actionItemOptions_triggered()
{
    // Get the currently selected item in the tree view, attaches a pointer to a ModelPart, which represents the stl Model
    QModelIndex index = ui->treeView->currentIndex();
    ModelPart* selectedPart = static_cast<ModelPart*>(index.internalPointer());

    // If no item is selected, show a warning message and exit
    if (!selectedPart) {
        QMessageBox::warning(this, "No Selection", "Please select an item first.");
        return;
    }

    // Create and initialize the option dialog
    OptionDialog optionDialog(this);
    QColor currentColor(selectedPart->getColourR(), selectedPart->getColourG(), selectedPart->getColourB());
    optionDialog.setValues(selectedPart->data(0).toString(), currentColor, selectedPart->visible());

    // If the user clicks "OK" in the dialog
    if (optionDialog.exec() == QDialog::Accepted) {
        // Update the model part's name using the dialog's new name
        selectedPart->setData(0, optionDialog.getName());

        // Get the chosen color from the dialog and update the model part
        QColor chosenColor = optionDialog.getColor();
        selectedPart->setColour(chosenColor.red(), chosenColor.green(), chosenColor.blue());

        // If the VR thread is running, update the corresponding VTK actor color
        if (vrThread && vrThread->isRunning()) {
            vtkActor* actor = selectedPart->getActor();
        }

        // Update the visibility of the model part
        selectedPart->setVisible(optionDialog.isVisible());

        // Notify the model/view that the data for this index has changed
        partList->dataChanged(index, index);

        // Trigger a render update in the viewer
        updateRender();

        // Emit a signal to display a status message for 2 seconds
        emit statusUpdateMessageSignal("Updated item options", 2000);
    }
}


// Handle when tree view is clicked and emits a message
void MainWindow::handleTreeClicked()
{
    QModelIndex index = ui->treeView->currentIndex();
    ModelPart* selectedPart = static_cast<ModelPart*>(index.internalPointer());

    if (selectedPart) {
        QString text = selectedPart->data(0).toString();
        emit statusUpdateMessageSignal("Selected item: " + text, 2000);
    }
}
// This is the handling of the OpenFile Button, however it actually opens a repositry
void MainWindow::on_actionOpenFile_triggered()
{
    QString folderPath = QFileDialog::getExistingDirectory(this, "Select Repositry Folder", QDir::homePath());

    if (!folderPath.isEmpty()) {
        partList->clear();
        renderer->RemoveAllViewProps();

        loadInitialPartsFromFolder(folderPath);

    }
}

// Handles the selection of a part when the user right clicks on a stlfile allowing them to open option dialog
void MainWindow::showContextMenu(const QPoint& pos)
{
    QModelIndex index = ui->treeView->indexAt(pos);
    if (!index.isValid()) return;

    ui->treeView->setCurrentIndex(index); // Make sure context menu affects selected part
    QMenu contextMenu(this);
    contextMenu.addAction(ui->actionItemOptions);
    contextMenu.exec(ui->treeView->viewport()->mapToGlobal(pos));
}

void MainWindow::updateRender() {
    // Clears all existing actors
    renderer->RemoveAllViewProps();
    // Recursively add each isible actor from the tree
    int topLevelCount = partList->rowCount(QModelIndex());
    for (int i = 0; i < topLevelCount; ++i) {
        QModelIndex topIndex = partList->index(i, 0, QModelIndex());
        updateRenderFromTree(topIndex);
    }

    if (renderer->GetActors()->GetNumberOfItems() > 0) {
        renderer->ResetCamera();
    }

    renderWindow->Render();
}
// Recursively moves through the model tree and adds visible parts to the renderer
void MainWindow::updateRenderFromTree(const QModelIndex& index)
{
    if (!index.isValid()) return;

    ModelPart* selectedPart = static_cast<ModelPart*>(index.internalPointer());

    if (selectedPart && selectedPart->visible()) {
        vtkSmartPointer<vtkActor> actor = selectedPart->getActor();

        if (actor) {
            renderer->AddActor(actor);
        }
    }

    int rows = partList->rowCount(index);
    for (int i = 0; i < rows; i++) {
        updateRenderFromTree(partList->index(i, 0, index));
    }
}

// Loads model parts from a specified folder and its subfolders, then updates the render view
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
// Code for the button that starts the VR
void MainWindow::startVRRendering() {

    // loop through tree and add actors using add actor offline

    if (vrThread && !vrThread->isRunning()) {
        vrThread->start();
        emit statusUpdateMessageSignal("VR thread started", 2000);
    }
    else {
        emit statusUpdateMessageSignal("VR thread is already running", 2000);
    }
}

// Recursively loads all STL files and subfolders from a given directory into the model tree.
void MainWindow::loadPartsRecursively(const QDir& dir, ModelPart* parentItem)
{
    QStringList filters;
    filters << "*.stl" << "*.STL";

    // Load STL files in this directory
    QFileInfoList fileList = dir.entryInfoList(filters, QDir::Files);
    for (const QFileInfo& fileInfo : fileList) {
        QString filePath = fileInfo.absoluteFilePath();
        ModelPart* part = new ModelPart({ fileInfo.fileName(), 0 }, parentItem);
        parentItem->appendChild(part);

        part->loadSTL(filePath);

        part->setVisible(false);  // Default invisible

        qDebug() << "Loaded" << filePath << "and set to invisible.";
    }

    // Now handle subdirectories
    QFileInfoList dirList = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (const QFileInfo& subdirInfo : dirList) {
        QString subdirPath = subdirInfo.absoluteFilePath();
        QDir subdir(subdirPath);

        // Create a ModelPart to represent the folder
        ModelPart* folderItem = new ModelPart({ subdirInfo.fileName(), 0 }, parentItem);
        parentItem->appendChild(folderItem);

        qDebug() << "Created folder node:" << subdirPath;

        // Recursively load parts from the subfolder
        loadPartsRecursively(subdir, folderItem);
    }

}
//
void MainWindow::handleStartVR() {
    if (vrThread && vrThread->isRunning()) {

        QMessageBox::information(this, "VR", "VR is already running.");
            return;
    }

    if (vrThread) {
        vrThread->deleteLater(); // deletion of previous thread
    }

    vrThread = new VRRenderThread();

    addVisiblePartsToVR(vrThread);

    vrThread->start();

    qDebug() << "Emitting sendActors with" << renderer->GetActors()->GetNumberOfItems() << "actors";
    emit sendActors(renderer->GetActors());


    emit statusUpdateMessageSignal("VR started", 2000);
    }

void MainWindow::addVisiblePartsToVR(VRRenderThread* thread) {
    int topLevelCount = partList->rowCount(QModelIndex());
    for (int i = 0; i < topLevelCount; ++i) {
        QModelIndex topIndex = partList->index(i, 0, QModelIndex());
        addPartsFromTree(topIndex, thread);
    }
}

void MainWindow ::addPartsFromTree(const QModelIndex& index, VRRenderThread* thread){

    ModelPart* selectedPart = static_cast<ModelPart*>(index.internalPointer());
    if (selectedPart->visible()) {
        vtkSmartPointer<vtkActor> actor = selectedPart->getNewActor();
        if (actor) {
            thread->addActorOffline(actor);
        }
    }
    int rows = partList->rowCount(index);
    for (int i = 0; i < rows; i++) {
        addPartsFromTree(partList->index(i, 0, index), thread);
    }
}

void MainWindow::on_actionOpenSingleFile_triggered()
{
    QString filePath = QFileDialog::getOpenFileName(
        this,
        "Open STL File",
        QDir::homePath(),
        "STL Files (*.stl *.STL)");

    if (filePath.isEmpty())
        return;

    QFileInfo fileInfo(filePath);
    partList->addPart(fileInfo.fileName(), filePath);

    updateRender();
    emit statusUpdateMessageSignal("Loaded single file: " + fileInfo.fileName(), 2000);
    qDebug() << "Loaded single file:" << filePath;
}

void MainWindow::on_actionClearTreeView_triggered()
{
    // Clear the model (removes all ModelPart entries)
    partList->clear();

    // Clear all VTK actors from the renderer
    renderer->RemoveAllViewProps();

    // Trigger a render update to reflect the empty scene
    renderWindow->Render();

    // Optionally show a status bar message
    emit statusUpdateMessageSignal("Tree view and VTK scene cleared", 2000);

    // Optional debug
    qDebug() << "Cleared tree view and VTK scene.";
}

void MainWindow::handleStopVR() {
    if (vrThread && vrThread->isRunning()) {
        vrThread->issueCommand(VRRenderThread::END_RENDER, 0.0); // assuming END_RENDER properly stops rendering
        vrThread->wait(); // Wait until the thread has stopped
        emit statusUpdateMessageSignal("VR thread stopped", 2000);
        qDebug() << "VR thread stopped safely.";
    }
    else {
        emit statusUpdateMessageSignal("VR thread was not running", 2000);
        qDebug() << "No VR thread running to stop.";
    }
}
