/**
 * @file mainwindow.h
 * @brief Declaration of the MainWindow class for the main application window.
 *
 * This header file declares the MainWindow class, which is the central widget
 * of the application. It manages the user interface, the visualization of 3D
 * models using the Visualization Toolkit (VTK), and the interaction with a
 * separate thread responsible for Virtual Reality (VR) rendering.
 */
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
#include <vtkActorCollection.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

/**
 * @brief The MainWindow class.
 * This class defines the main window of the application, responsible for the user interface,
 * 3D rendering using VTK, and interaction with the VR rendering thread.
 */
    class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief MainWindow constructor.
     * Constructs the main window.
     * @param parent The parent widget, defaults to nullptr.
     */
    explicit MainWindow(QWidget* parent = nullptr);
    /**
     * @brief MainWindow destructor.
     * Destroys the main window, ensuring proper cleanup of resources,
     * especially the VR rendering thread.
     */
    ~MainWindow();

signals:
    /**
     * @brief Signal to update the status bar with a message.
     * This signal is emitted when a status update is needed in the main window's
     * status bar.
     * @param message The message to display.
     * @param timeout The duration (in milliseconds) to display the message.
     */
    void statusUpdateMessageSignal(const QString& message, int timeout);
    /**
     * @brief Signal to send the collection of actors to another component (e.g., VR thread).
     * This signal is emitted to pass the current set of VTK actors, representing
     * the visible 3D models, to another part of the application, such as the VR
     * rendering thread.
     * @param actors The collection of VTK actors.
     */
    void sendActors(vtkActorCollection* actors);

private slots:
    /**
     * @brief Handles the click event of a button (for testing).
     * This slot is connected to a button in the user interface, primarily for
     * testing purposes to verify signal-slot connections.
     */
    void handleButton();
    /**
     * @brief Updates the VTK render window with the current model.
     * This slot triggers a redraw of the 3D scene in the VTK render window,
     * reflecting any changes in the loaded models or their properties.
     */
    void updateRender();
    /**
     * @brief Recursively updates the render from the model tree.
     * This slot traverses the model tree structure and adds the visible
     * VTK actors of the model parts to the renderer.
     * @param index The current index in the model tree being processed.
     */
    void updateRenderFromTree(const QModelIndex& index);
    /**
     * @brief Handles the action to open the options dialog.
     * This slot is invoked when the user requests to open the options dialog,
     * typically for modifying application settings or viewing information.
     */
    void handleOpenOptions();
    /**
     * @brief Handles the click event on an item in the tree view.
     * This slot is called when the user clicks on an item in the tree view,
     * allowing for actions based on the selected model part.
     */
    void handleTreeClicked();
    /**
     * @brief Handles the action to open a directory of files.
     * This slot is triggered when the user chooses to open a directory,
     * prompting them to select a folder containing 3D model files.
     */
    void on_actionOpenFile_triggered();
    /**
     * @brief Handles the action to open a single file.
     * This slot is invoked when the user selects to open a single 3D model file
     * through the application's menu or interface.
     */
    void on_actionOpenSingleFile_triggered();
    /**
     * @brief Handles the action to open the options for a selected item.
     * This slot is called when the user requests to view or modify the options
     * of a specific model part selected in the tree view.
     */
    void on_actionItemOptions_triggered();
    /**
     * @brief Updates the status bar with a message.
     * This slot receives the message and timeout duration to display a temporary
     * message in the application's status bar.
     * @param message The message to display.
     * @param timeout The duration (in milliseconds) to display the message.
     */
    void statusUpdateMessage(const QString& message, int timeout);
    /**
     * @brief Loads initial model parts from a specified folder.
     * This slot is responsible for loading 3D model files (e.g., STL) from a
     * given directory and adding them to the model tree and the VTK renderer.
     * @param folderPath The path to the folder containing the model files.
     */
    void loadInitialPartsFromFolder(const QString& folderPath);
    /**
     * @brief Recursively loads model parts from a directory and its subdirectories.
     * This slot traverses a directory structure, identifies supported 3D model
     * files, and adds them as ModelPart objects to the model tree.
     * @param dir The current directory to process.
     * @param parentItem The parent item in the model tree to which new parts will be added.
     */
    void loadPartsRecursively(const QDir& dir, ModelPart* parentItem);
    /**
     * @brief Starts the VR rendering process.
     * This slot initiates the Virtual Reality rendering by communicating with
     * the VR rendering thread.
     */
    void startVRRendering();
    /**
     * @brief Handles the click event to start VR rendering.
     * This slot is connected to a UI element (e.g., a button) that triggers
     * the start of the VR rendering.
     */
    void handleStartVR();
    /**
     * @brief Handles the action to clear the tree view.
     * This slot is invoked when the user chooses to clear all the items from
     * the model tree view, effectively removing all loaded models.
     */
    void on_actionClearTreeView_triggered();
    /**
     * @brief Handles the click event to stop VR rendering.
     * This slot is connected to a UI element that allows the user to stop the
     * ongoing Virtual Reality rendering process.
     */
    void handleStopVR();
private:
    /**
     * @brief Stores the index of the tree view item for context menu operations.
     * This member variable holds the QModelIndex of the item in the tree view
     * that was right-clicked, allowing context-sensitive actions.
     */
    QModelIndex contextMenuIndex;  // To track right-clicked item

private:
    /**
     * @brief Pointer to the VR rendering thread.
     * This pointer manages the separate thread responsible for handling the
     * Virtual Reality rendering of the 3D models.
     */
    VRRenderThread* vrThread = nullptr;


private:
    /**
     * @brief Pointer to the UI object generated by Qt Designer.
     * This pointer provides access to the user interface elements defined in
     * the associated .ui file.
     */
    Ui::MainWindow* ui;
    /**
     * @brief Model for storing and managing the 3D model parts in a tree structure.
     * This member variable holds the data model that organizes the loaded 3D
     * models in a hierarchical tree structure, displayed in the tree view.
     */
    ModelPartList* partList;
    /**
     * @brief VTK renderer for the 3D scene.
     * This smart pointer manages the VTK renderer object, which is responsible
     * for rendering the 3D models in the visualization window.
     */
    vtkSmartPointer<vtkRenderer> renderer;
    /**
     * @brief VTK render window for OpenGL rendering.
     * This smart pointer manages the VTK render window, which provides the
     * OpenGL context for rendering the 3D scene. It is embedded within a
     * Qt widget.
     */
    vtkSmartPointer<vtkGenericOpenGLRenderWindow> renderWindow;

    /**
     * @brief Initializes the VTK rendering environment.
     * This private function sets up the VTK render window and renderer,
     * associating them with the Qt widget designed to host the visualization.
     */
    void setupVTK();
    /**
     * @brief Shows the context menu at the specified position in the tree view.
     * This private slot is triggered when the user right-clicks on an item in
     * the tree view, displaying a context-sensitive menu with available actions.
     * @param pos The position where the context menu is requested, in widget coordinates.
     */
    void showContextMenu(const QPoint& pos);

    /**
     * @brief Adds the currently visible parts to the VR rendering thread.
     * This private function iterates through the model tree and gathers the
     * VTK actors of the currently visible model parts, then sends this collection
     * to the VR rendering thread for rendering in the virtual reality environment.
     * @param thread The VR rendering thread to which the actors are added.
     */
    void addVisiblePartsToVR(VRRenderThread* thread);
    /**
     * @brief Recursively adds parts from the model tree to the VR rendering thread.
     * This private function traverses the model tree structure and, for each
     * visible model part, retrieves its VTK actor and adds it to the collection
     * managed by the VR rendering thread.
     * @param index The current index in the model tree being processed.
     * @param thread The VR rendering thread to which the actors are added.
     */
    void addPartsFromTree(const QModelIndex& index, VRRenderThread* thread);
};

#endif // MAINWINDOW_H