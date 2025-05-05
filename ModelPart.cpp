/**
 * @file ModelPart.cpp
 * @brief Implementation of the ModelPart class, which represents a node in a hierarchical model structure.
 *
 * This class manages hierarchical data items typically used in model tree structures.
 * It includes support for VTK-based STL model visualization and coloring. The class can
 * store a list of child parts, assign parent-child relationships, store metadata, and
 * manage 3D actor representation including color and visibility.
 */

#include "ModelPart.h"
#include <vtkSTLReader.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkPolyData.h>
#include <vtkDataSetMapper.h>

 /**
  * @brief Constructs a ModelPart object.
  * @param data The data associated with this part (e.g., name).
  * @param parent The parent ModelPart in the hierarchy.
  */
ModelPart::ModelPart(const QList<QVariant>& data, ModelPart* parent)
    : m_itemData(data), m_parentItem(parent) {
}

/**
 * @brief Destructor for the ModelPart class.
 * Cleans up any child items.
 */
ModelPart::~ModelPart() {
    qDeleteAll(m_childItems);  // Ensures child objects are deleted
}

/**
 * @brief Appends a child item to this ModelPart.
 * @param item The child ModelPart to append.
 */
void ModelPart::appendChild(ModelPart* item) {
    item->m_parentItem = this;  // Set this as the parent
    m_childItems.append(item);  // Add child to list
}

/**
 * @brief Gets the child item at the specified row.
 * @param row The row index of the child item.
 * @return The child ModelPart, or nullptr if the row is invalid.
 */
ModelPart* ModelPart::child(int row) {
    if (row < 0 || row >= m_childItems.size())
        return nullptr;
    return m_childItems.at(row);
}

/**
 * @brief Gets the number of child items.
 * @return The number of child items.
 */
int ModelPart::childCount() const {
    return m_childItems.count();
}

/**
 * @brief Gets the number of columns in the data.
 * @return The number of columns.
 */
int ModelPart::columnCount() const {
    return m_itemData.count();
}

/**
 * @brief Gets the data for the specified column.
 * @param column The column index.
 * @return The data for the column, or an empty QVariant if the column is invalid.
 */
QVariant ModelPart::data(int column) const {
    if (column < 0 || column >= m_itemData.size())
        return QVariant();
    return m_itemData.at(column);
}

/**
 * @brief Sets the data for the specified column.
 * @param column The column index.
 * @param value The new data value.
 */
void ModelPart::setData(int column, const QVariant& value) {
    if (column < 0 || column >= m_itemData.size())
        return;
    m_itemData.replace(column, value);
}

/**
 * @brief Gets the parent item.
 * @return The parent ModelPart.
 */
ModelPart* ModelPart::parentItem() {
    return m_parentItem;
}

/**
 * @brief Gets the row of this item within its parent.
 * @return The row index.
 */
int ModelPart::row() const {
    if (m_parentItem)
        return m_parentItem->m_childItems.indexOf(const_cast<ModelPart*>(this));
    return 0;  // Top-level item
}

/**
 * @brief Sets the color of the ModelPart.
 * @param R The red component (0-255).
 * @param G The green component (0-255).
 * @param B The blue component (0-255).
 */
void ModelPart::setColour(const unsigned char R, const unsigned char G, const unsigned char B) {
    colourR = R;
    colourG = G;
    colourB = B;

    // Apply the color to the actor if it exists
    if (stlActor) {
        stlActor->GetProperty()->SetColor(R / 255.0, G / 255.0, B / 255.0);
    }
}

/** @brief Gets the red component of the color. */
unsigned char ModelPart::getColourR() const { return colourR; }

/** @brief Gets the green component of the color. */
unsigned char ModelPart::getColourG() const { return colourG; }

/** @brief Gets the blue component of the color. */
unsigned char ModelPart::getColourB() const { return colourB; }

/**
 * @brief Sets the visibility of the ModelPart.
 * @param visible True to make it visible, false to hide it.
 */
void ModelPart::setVisible(bool visible) {
    isVisible = visible;

    // Reflect visibility in the 3D actor
    if (stlActor) {
        stlActor->SetVisibility(visible);
    }
}

/**
 * @brief Gets the visibility of the ModelPart.
 * @return True if visible, false otherwise.
 */
bool ModelPart::visible() const {
    return isVisible;
}

/**
 * @brief Loads an STL file and creates the VTK actor.
 * @param fileName The path to the STL file.
 */
void ModelPart::loadSTL(QString fileName) {
    stlReader = vtkSmartPointer<vtkSTLReader>::New();
    stlReader->SetFileName(fileName.toStdString().c_str());
    stlReader->Update();

    // Create mapper and actor
    stlMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    stlMapper->SetInputConnection(stlReader->GetOutputPort());

    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(stlMapper);

    this->stlActor = actor;
}

/**
 * @brief Gets the VTK actor associated with this ModelPart.
 * @return The VTK actor.
 */
vtkSmartPointer<vtkActor> ModelPart::getActor() {
    return this->stlActor;
}

/**
 * @brief Removes all child items.
 */
void ModelPart::removeAllChildren() {
    qDeleteAll(m_childItems);  // Deletes child pointers
    m_childItems.clear();      // Clears the list
}

/**
 * @brief Gets the color as a QColor object.
 * @return The color as a QColor.
 */
QColor ModelPart::getColor() const {
    return QColor(colourR, colourG, colourB);
}

/**
 * @brief Sets the color from a QColor object.
 * @param color The new color.
 */
void ModelPart::setColor(const QColor& color) {
    setColour(color.red(), color.green(), color.blue());
}

/**
 * @brief Gets a new VTK actor for VR rendering.
 * @return A new VTK actor, or nullptr if there's no original actor.
 *
 * This function creates a separate actor for VR rendering.
 * The original actor is used for GUI rendering.
 */
vtkActor* ModelPart::getNewActor() {
    if (!this->stlActor) {
        return nullptr;
    }

    // Create a new mapper and actor using the same data source
    newMapper = vtkSmartPointer<vtkDataSetMapper>::New();
    newMapper->SetInputConnection(stlReader->GetOutputPort());

    newActor = vtkSmartPointer<vtkActor>::New();
    newActor->SetMapper(newMapper);

    // Copy visual properties from the original actor
    newActor->SetProperty(this->stlActor->GetProperty());

    return newActor;
}
