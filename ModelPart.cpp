#include "ModelPart.h"
#include <vtkSTLReader.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkPolyData.h>
#include <vtkDataSetMapper.h>
ModelPart::ModelPart(const QList<QVariant>& data, ModelPart* parent)
    : m_itemData(data), m_parentItem(parent) {
}

ModelPart::~ModelPart() {
    qDeleteAll(m_childItems);
}

void ModelPart::appendChild(ModelPart* item) {
    item->m_parentItem = this;
    m_childItems.append(item);
}

ModelPart* ModelPart::child(int row) {
    if (row < 0 || row >= m_childItems.size())
        return nullptr;
    return m_childItems.at(row);
}

int ModelPart::childCount() const {
    return m_childItems.count();
}

int ModelPart::columnCount() const {
    return m_itemData.count();
}

QVariant ModelPart::data(int column) const {
    if (column < 0 || column >= m_itemData.size())
        return QVariant();
    return m_itemData.at(column);
}

void ModelPart::setData(int column, const QVariant& value) {
    if (column < 0 || column >= m_itemData.size())
        return;
    m_itemData.replace(column, value);
}

ModelPart* ModelPart::parentItem() {
    return m_parentItem;
}

int ModelPart::row() const {
    if (m_parentItem)
        return m_parentItem->m_childItems.indexOf(const_cast<ModelPart*>(this));
    return 0;
}

void ModelPart::setColour(const unsigned char R, const unsigned char G, const unsigned char B) {
    colourR = R;
    colourG = G;
    colourB = B;

    if (stlActor) {
        stlActor->GetProperty()->SetColor(R / 255.0, G / 255.0, B / 255.0);
    }
}

unsigned char ModelPart::getColourR() const { return colourR; }
unsigned char ModelPart::getColourG() const { return colourG; }
unsigned char ModelPart::getColourB() const { return colourB; }

void ModelPart::setVisible(bool visible) {
    isVisible = visible;
    if (stlActor) {
        stlActor->SetVisibility(visible);
    }
}

bool ModelPart::visible() const {
    return isVisible;
}

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

vtkSmartPointer<vtkActor> ModelPart::getActor() {
    return this->stlActor;
}

void ModelPart::removeAllChildren() {
    qDeleteAll(m_childItems);
    m_childItems.clear();
}

// ? New methods

QColor ModelPart::getColor() const {
    return QColor(colourR, colourG, colourB);
}

void ModelPart::setColor(const QColor& color) {
    setColour(color.red(), color.green(), color.blue());
}

//vtkActor* ModelPart::getNewActor() {
    /* This is a placeholder function that you will need to modify if you want to use it
     *
     * The default mapper/actor combination can only be used to render the part in
     * the GUI, it CANNOT also be used to render the part in VR. This means you need
     * to create a second mapper/actor combination for use in VR - that is the role
     * of this function. */


     /* 1. Create new mapper */

     /* 2. Create new actor and link to mapper */

     /* 3. Link the vtkProperties of the original actor to the new actor. This means
      *    if you change properties of the original part (colour, position, etc), the
      *    changes will be reflected in the GUI AND VR rendering.
      *
      *    See the vtkActor documentation, particularly the GetProperty() and SetProperty()
      *    functions.
      */


      /* The new vtkActor pointer must be returned here */
  //    return nullptr;

  //}

vtkActor* ModelPart::getNewActor() {
    if (!this->stlActor) {
        return nullptr;
    }
    newMapper = vtkSmartPointer<vtkDataSetMapper>::New();
    newMapper->SetInputConnection(stlReader->GetOutputPort());

    newActor = vtkSmartPointer<vtkActor>::New();
    newActor->SetMapper(newMapper);

    newActor->SetProperty(this->stlActor->GetProperty());

    return newActor;
}

