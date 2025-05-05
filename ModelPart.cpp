// Header file for this class
#include "ModelPart.h"

// Include VTK headers 
#include <vtkSTLReader.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkPolyData.h>
#include <vtkDataSetMapper.h>

// Constructor: Initializes with data and an optional parent
ModelPart::ModelPart(const QList<QVariant>& data, ModelPart* parent)
    : m_itemData(data), m_parentItem(parent) {
}

// Destructor: Deletes all child items to free memory
ModelPart::~ModelPart() {
    qDeleteAll(m_childItems);
}

// Adds a child to this model part and sets its parent
void ModelPart::appendChild(ModelPart* item) {
    item->m_parentItem = this;
    m_childItems.append(item);
}

// Returns a pointer to the child at the given row, or nullptr if out of range
ModelPart* ModelPart::child(int row) {
    if (row < 0 || row >= m_childItems.size())
        return nullptr;
    return m_childItems.at(row);
}

// Returns the number of child items
int ModelPart::childCount() const {
    return m_childItems.count();
}

// Returns the number of columns in the item data
int ModelPart::columnCount() const {
    return m_itemData.count();
}

// Retrieves the data at the specified column
QVariant ModelPart::data(int column) const {
    if (column < 0 || column >= m_itemData.size())
        return QVariant();
    return m_itemData.at(column);
}

// Sets the data at a specific column
void ModelPart::setData(int column, const QVariant& value) {
    if (column < 0 || column >= m_itemData.size())
        return;
    m_itemData.replace(column, value);
}

// Returns the parent of this model part
ModelPart* ModelPart::parentItem() {
    return m_parentItem;
}

// Returns the index of this item in the parent's child list
int ModelPart::row() const {
    if (m_parentItem)
        return m_parentItem->m_childItems.indexOf(const_cast<ModelPart*>(this));
    return 0;
}

// Sets the RGB color of the part and updates the actor's visual appearance
void ModelPart::setColour(const unsigned char R, const unsigned char G, const unsigned char B) {
    colourR = R;
    colourG = G;
    colourB = B;

    if (stlActor) {
        stlActor->GetProperty()->SetColor(R / 255.0, G / 255.0, B / 255.0);
    }
}

// Accessor methods for individual RGB color components
unsigned char ModelPart::getColourR() const { return colourR; }
unsigned char ModelPart::getColourG() const { return colourG; }
unsigned char ModelPart::getColourB() const { return colourB; }

// Sets visibility of the actor (part)
void ModelPart::setVisible(bool visible) {
    isVisible = visible;
    if (stlActor) {
        stlActor->SetVisibility(visible);
    }
}

// Returns the visibility state of the actor
bool ModelPart::visible() const {
    return isVisible;
}

// Loads an STL file and creates a corresponding VTK actor
void ModelPart::loadSTL(QString fileName) {
    stlReader = vtkSmartPointer<vtkSTLReader>::New();
    stlReader->SetFileName(fileName.toStdString().c_str());
    stlReader->Update();

    stlMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    stlMapper->SetInputConnection(stlReader->GetOutputPort());

    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(stlMapper);

    this->stlActor = actor;
}

// Returns the existing VTK actor associated with this model part
vtkSmartPointer<vtkActor> ModelPart::getActor() {
    return this->stlActor;
}

// Removes and deletes all children from this part
void ModelPart::removeAllChildren() {
    qDeleteAll(m_childItems);
    m_childItems.clear();
}

// Returns the color of the part as a QColor object
QColor ModelPart::getColor() const {
    return QColor(colourR, colourG, colourB);
}

// Sets the color of the part using a QColor object
void ModelPart::setColor(const QColor& color) {
    setColour(color.red(), color.green(), color.blue());
}

// Creates and returns a new actor with the same geometry and property settings
vtkActor* ModelPart::getNewActor() {
    if (!this->stlActor) {
        return nullptr;
    }

    newMapper = vtkSmartPointer<vtkDataSetMapper>::New();
    newMapper->SetInputConnection(stlReader->GetOutputPort());

    newActor = vtkSmartPointer<vtkActor>::New();
    newActor->SetMapper(newMapper);

    // Copy visual properties from the original actor
    newActor->SetProperty(this->stlActor->GetProperty());

    return newActor;
}
