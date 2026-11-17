/* =============================================================================
 * FILE:         additemdialog_radio.cpp
 * MODULE:       Add Item Dialog - Radio
 * PROJECT:      Indigenous Scenario and Sensor Simulation Toolkit (ISSST)
 * ORGANISATION: Oxygen 2 Innovation (O2I).
 *
 * DESCRIPTION:  Radio-specific parts of AddItemDialog (Add Radio / component
 *               radios): radio type row, radio profile population, getters.
 *               Any change related to the Radio dialog goes in this file only.
 * =============================================================================
 */

#include "additemdialog.h"
#include "core/Hierarchy/hierarchy.h"
#include <QComboBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>

// %%% "Radio Type:" row %%%
void AddItemDialog::buildRadioTypeRow(QVBoxLayout *mainLayout)
{
    m_radioTypeContainer = new QWidget(this);
    QHBoxLayout *radioTypeLayout = new QHBoxLayout(m_radioTypeContainer);
    radioTypeLayout->setContentsMargins(0, 0, 0, 0);
    QLabel *radioLabel = new QLabel("Radio Type:", this);
    radioLabel->setStyleSheet("color: white;");
    radioTypeLayout->addWidget(radioLabel);
    radioTypeComboBox = new QComboBox(this);
    QStringList radioTypes = m_hierarchy
                                 ? m_hierarchy->getAvailableRadioTypes()
                                 : QStringList({"Generic"});
    radioTypeComboBox->addItems(radioTypes);
    radioTypeComboBox->setCurrentText("Generic");
    radioTypeComboBox->setStyleSheet("color: white;");
    radioTypeLayout->addWidget(radioTypeComboBox);
    radioTypeLayout->addStretch();
    mainLayout->addWidget(m_radioTypeContainer);
}

// %%% clearEntitySelection() radio part %%%
void AddItemDialog::resetRadioTypeToDefault()
{
    if (radioTypeComboBox) {
        radioTypeComboBox->setCurrentText("Generic");
    }
}

/* Populate radio profiles */
void AddItemDialog::populateRadioProfiles()
{
    populateProfiles("Radio");
}

/* Get selected radio type */
QString AddItemDialog::getRadioType() const {
    return radioTypeComboBox ? radioTypeComboBox->currentText() : "Generic";
}
