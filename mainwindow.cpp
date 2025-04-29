#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ModelPart.h"
#include "ModelPartList.h"
#include "optiondialog.h"

#include <QFileDialog>
#include <QMenu>
#include <QMessageBox>
#include <QFileInfo>
#include <QDir>
#include <QFileInfoList>
#include <QDebug>

#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkCamera.h>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->addButton, &QPushButton::released, this, &MainWindow::handleButton);
    connect(ui->openOptions, &QPushButton::released, this, &MainWindow::handleOpenOptions);
    connect(this, &MainWindow::statusUpdateMessageSignal, ui->statusbar, &QStatusBar::showMessage);

    this->partList = new ModelPartList("PartsList");
    ui->treeView->setModel(this->partList);
    ui->treeView->addAction(ui->actionItemOptions);
    ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeView, &QTreeView::customContextMenuRequested, this, &MainWindow::showContextMenu);
    connect(ui->treeView, &QTreeView::clicked, this, &MainWindow::handleTreeClicked);

    setupVTK();
    //loadInitialPartsFromFolder("C:/Users/eeyas37/2024_eeyas37/group16/2024_GROUP_16/Levels");
   

    //emit statusUpdateMessageSignal("Loaded Level0 parts (invisible)", 2000);
}

MainWindow::~MainWindow()
{
    delete ui;
}

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

void MainWindow::statusUpdateMessage(const QString& message, int timeout)
{
    ui->statusbar->showMessage(message, timeout);
}

void MainWindow::handleButton()
{
    QMessageBox msgBox;
    msgBox.setText("Add button was clicked");
    msgBox.exec();
    emit statusUpdateMessageSignal("Add button was clicked", 2000);
}

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

void MainWindow::handleOpenOptions()
{
    OptionDialog optionDialog(this);
    optionDialog.exec();
    emit statusUpdateMessageSignal("Open Options button was clicked", 2000);
}

void MainWindow::handleTreeClicked()
{
    QModelIndex index = ui->treeView->currentIndex();
    ModelPart* selectedPart = static_cast<ModelPart*>(index.internalPointer());

    if (selectedPart) {
        QString text = selectedPart->data(0).toString();
        emit statusUpdateMessageSignal("Selected item: " + text, 2000);
    }
}

void MainWindow::on_actionOpenFile_triggered()
{
    QString folderPath = QFileDialog::getExistingDirectory(this, "Select Repositry Folder", QDir::homePath());

    if (!folderPath.isEmpty()) {
        partList->clear();
        renderer->RemoveAllViewProps();

        loadInitialPartsFromFolder(folderPath);

    }
}

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


void MainWindow::updateRenderFromTree(const QModelIndex& index)
{
    if (!index.isValid()) return;

    ModelPart* selectedPart = static_cast<ModelPart*>(index.internalPointer());

    if (selectedPart && selectedPart->visible()) {
        vtkSmartPointer<vtkActor> actor = selectedPart->getActor();
        qDebug() << "Checking part:" << selectedPart->data(0).toString()
            << "| Visible:" << selectedPart->visible()
            << "| Actor:" << (actor != nullptr);

        if (actor) {
            renderer->AddActor(actor);
        }
    }

    int rows = partList->rowCount(index);
    for (int i = 0; i < rows; i++) {
        updateRenderFromTree(partList->index(i, 0, index));
    }
}

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
