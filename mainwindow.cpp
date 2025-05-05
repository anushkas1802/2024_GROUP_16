/**
 * @file mainwindow.cpp
 * @brief Implementation of the MainWindow class for handling STL file visualization and VR interaction.
 *
 * This file defines the main window UI logic for managing STL file loading, rendering using VTK,
 * and VR rendering in a Qt application.
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
 *
 * This constructor initializes the main window, sets up the user interface using the provided `ui` object,
 * establishes signal-slot connections for various UI elements, initializes the model part list and tree view,
 * sets up the VTK rendering environment, and potentially starts the VR rendering thread.
 *
 * @param parent The parent widget for this main window, typically `nullptr` for the top-level window.
 */
MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // --- Connect UI buttons to their respective slot functions ---
    connect(ui->addButton, &QPushButton::released, this, &MainWindow::handleButton);
    connect(ui->openOptions, &QPushButton::released, this, &MainWindow::handleOpenOptions);
    connect(this, &MainWindow::statusUpdateMessageSignal, ui->statusbar, &QStatusBar::showMessage);
    connect(ui->startVRButton, &QPushButton::clicked, this, &MainWindow::handleStartVR);
    connect(ui->stopVRButton, &QPushButton::clicked, this, &MainWindow::handleStopVR);

    // --- Set up tree view with model part list ---
    this->partList = new ModelPartList("PartsList");
    ui->treeView->setModel(this->partList);
    ui->treeView->addAction(ui->actionItemOptions);
    ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeView, &QTreeView::customContextMenuRequested, this, &MainWindow::showContextMenu);
    connect(ui->treeView, &QTreeView::clicked, this, &MainWindow::handleTreeClicked);

    // --- Setup menu actions ---
    connect(ui->actionOpenSingleFile, &QAction::triggered, this, &MainWindow::on_actionOpenSingleFile_triggered);
    connect(ui->actionClearTreeView, &QAction::triggered, this, &MainWindow::on_actionClearTreeView_triggered);

    // --- Initialize VTK renderer ---
    setupVTK();

    // Emit initial status message
    emit statusUpdateMessageSignal("Loaded Level0 parts (invisible)", 2000);

    // Start VR thread
    vrThread = new VRRenderThread(this);
}

/**
 * @brief Destructor. Stops the VR thread if it is running and cleans up resources.
 */
MainWindow::~MainWindow()
{
    if (vrThread && vrThread->isRunning()) {
        vrThread->issueCommand(VRRenderThread::END_RENDER, 0.0);
        vrThread->wait();
    }
    delete vrThread;
    delete ui;
}

/**
 * @brief Initializes the VTK rendering window and renderer.
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

    renderer->SetBackground(0.1, 0.1, 0.1); // Dark background
    renderWindow->Render();
}

/**
 * @brief Updates the status bar with a message.
 * @param message The message to display.
 * @param timeout Duration in milliseconds to display the message.
 */
void MainWindow::statusUpdateMessage(const QString& message, int timeout)
{
    ui->statusbar->showMessage(message, timeout);
}

/**
 * @brief Dummy handler for add button.
 * Displays a message box and emits a status message.
 */
void MainWindow::handleButton()
{
    QMessageBox msgBox;
    msgBox.setText("Add button was clicked");
    msgBox.exec();
    emit statusUpdateMessageSignal("Add button was clicked", 2000);
}

/**
 * @brief Opens the option dialog for the selected tree item.
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

        // Placeholder for future VR actor update logic
        if (vrThread && vrThread->isRunning()) {
            vtkActor* actor = selectedPart->getActor();
            // TODO: Implement actor update in VR thread
        }

        selectedPart->setVisible(optionDialog.isVisible());
        partList->dataChanged(index, index);
        updateRender();
        emit statusUpdateMessageSignal("Updated item options", 2000);
    }
}

/**
 * @brief Opens a test OptionDialog, typically for UI testing.
 */
void MainWindow::handleOpenOptions()
{
    OptionDialog optionDialog(this);
    optionDialog.exec();
    emit statusUpdateMessageSignal("Open Options button was clicked", 2000);
}

/**
 * @brief Emits a status message when a tree item is clicked.
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
 * @brief Handles folder selection and loads all STL files within.
 */
void MainWindow::on_actionOpenFile_triggered()
{
    QString folderPath = QFileDialog::getExistingDirectory(this, "Select Repositry Folder", QDir::homePath());

    if (!folderPath.isEmpty()) {
        partList->clear();
        renderer->RemoveAllViewProps();
        loadInitialPartsFromFolder(folderPath);
    }
}

/**
 * @brief Shows a context menu for the tree view.
 * @param pos Position of the right-click event.
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
 * @brief Updates the render window with all currently visible model parts.
 */
void MainWindow::updateRender()
{
    renderer->RemoveAllViewProps();
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

/**
 * @brief Recursively adds visible parts to the VTK renderer from the model tree.
 * @param index Current index in the model tree.
 */
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

/**
 * @brief Loads STL model parts from the given directory recursively.
 * @param folderPath The path to the root directory.
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
