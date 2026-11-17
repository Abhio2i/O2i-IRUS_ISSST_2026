/* =============================================================================
 * FILE:         additemdialog_iff.cpp
 * MODULE:       Add Item Dialog - IFF
 * PROJECT:      Indigenous Scenario and Sensor Simulation Toolkit (ISSST)
 * ORGANISATION: Oxygen 2 Innovation (O2I).
 *
 * DESCRIPTION:  IFF-specific parts of AddItemDialog (Add IFF / component
 *               IFFs): IFF type row, IFF profile population, getters.
 *               Any change related to the IFF dialog goes in this file only.
 * =============================================================================
 */

#include "additemdialog.h"
#include "core/Hierarchy/hierarchy.h"
#include <QComboBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>

// %%% "IFF Type:" row %%%
void AddItemDialog::buildIFFTypeRow(QVBoxLayout *mainLayout)
{
    m_iffTypeContainer = new QWidget(this);
    QHBoxLayout *iffTypeLayout = new QHBoxLayout(m_iffTypeContainer);
    iffTypeLayout->setContentsMargins(0, 0, 0, 0);
    QLabel *iffLabel = new QLabel("IFF Type:", this);
    iffLabel->setStyleSheet("color: white;");
    iffTypeLayout->addWidget(iffLabel);
    iffTypeComboBox = new QComboBox(this);
    QStringList iffTypes = m_hierarchy
                               ? m_hierarchy->getAvailableIFFTypes()
                               : QStringList({"Generic"});
    iffTypeComboBox->addItems(iffTypes);
    iffTypeComboBox->setCurrentText("Generic");
    iffTypeComboBox->setStyleSheet("color: white;");
    iffTypeLayout->addWidget(iffTypeComboBox);
    iffTypeLayout->addStretch();
    mainLayout->addWidget(m_iffTypeContainer);
}

// %%% clearEntitySelection() IFF part %%%
void AddItemDialog::resetIFFTypeToDefault()
{
    if (iffTypeComboBox) {
        iffTypeComboBox->setCurrentText("Generic");
    }
}

/* Populate IFF profiles */
void AddItemDialog::populateIFFProfiles()
{
    populateProfiles("IFF");
}

/* Get selected IFF type */
QString AddItemDialog::getIFFType() const {
    return iffTypeComboBox ? iffTypeComboBox->currentText() : "Generic";
}
