/* =============================================================================
 * FILE:         additemdialog_sensor.cpp
 * MODULE:       Add Item Dialog - Sensor
 * PROJECT:      Indigenous Scenario and Sensor Simulation Toolkit (ISSST)
 * ORGANISATION: Oxygen 2 Innovation (O2I).
 *
 * DESCRIPTION:  Sensor-specific parts of AddItemDialog (Add Sensor / component
 *               sensors): "Add New" button, sensor category filter, sensor type
 *               row, auto-type from selected sensor, sensor validation and
 *               sensor profile population.
 *               Any change related to the Sensor dialog goes in this file only.
 * =============================================================================
 */

#include "additemdialog.h"
#include "GUI/Hierarchytree/additemdialog-styles.h"
#include "core/Hierarchy/hierarchy.h"
#include "core/Hierarchy/EntityProfiles/sensor.h"
#include <QPushButton>
#include <QComboBox>
#include <QCompleter>
#include <QLineEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>

// %%% "Add New" button (top-right of Sensor dialog) %%%
void AddItemDialog::buildSensorAddNewButton(QVBoxLayout *mainLayout)
{
    QHBoxLayout *addNewBtnLayout = new QHBoxLayout();
    addNewBtnLayout->addStretch();
    m_addNewBtn = new QPushButton("✚  Add New", this);
    m_addNewBtn->setCheckable(true);
    m_addNewBtn->setChecked(false);
    m_addNewBtn->setFixedHeight(28);
    m_addNewBtn->setCursor(Qt::PointingHandCursor);
    m_addNewBtn->setStyleSheet(
        "QPushButton {"
        "  background-color: #1565C0;"
        "  color: white;"
        "  border: none;"
        "  border-radius: 4px;"
        "  padding: 4px 14px;"
        "  font-size: 12px;"
        "  font-weight: bold;"
        "}"
        "QPushButton:hover  { background-color: #1976D2; }"
        "QPushButton:checked {"
        "  background-color: #424242;"
        "  color: #90CAF9;"
        "}"
        "QPushButton:checked:hover { background-color: #616161; }"
        );
    addNewBtnLayout->addWidget(m_addNewBtn);
    mainLayout->addLayout(addNewBtnLayout);
}

// %%% Toggle behaviour of "Add New" / "Search DB" %%%
void AddItemDialog::connectSensorAddNewButton(bool isDatabaseEditor)
{
    if (!m_addNewBtn) {
        return;
    }
    connect(m_addNewBtn, &QPushButton::toggled, this, [this, isDatabaseEditor](bool checked) {
        m_addNewMode = checked;
        m_entitySearchContainer->setVisible(!checked);
        if (m_sensorTypeContainer) {
            m_sensorTypeContainer->setVisible(isDatabaseEditor ? true : checked);
        }
        m_addNewBtn->setText(checked ? "← Search DB" : "✚  Add New");
        if (checked) {
            selectedEntityId.clear();
            if (entitySearchLineEdit) {
                entitySearchLineEdit->clear();
                entitySearchLineEdit->setCompleter(nullptr);
                if (entityCompleter && entityCompleter->popup())
                    entityCompleter->popup()->hide();
            }
            if (nameLineEdit)     nameLineEdit->setText("Sensor");
            if (sensorTypeComboBox) {
                sensorTypeComboBox->setEnabled(true);
                sensorTypeComboBox->setToolTip("");
                sensorTypeComboBox->setCurrentText("Generic");
            }
        } else {
            if (entitySearchLineEdit) {
                entitySearchLineEdit->clear();
                entitySearchLineEdit->setCompleter(entityCompleter);
            }
            selectedEntityId.clear();
            if (nameLineEdit) nameLineEdit->setText("Sensor");
        }
    });
}

// %%% Sensor category filter (next to search box) %%%
QComboBox* AddItemDialog::createSensorCategoryFilter()
{
    QComboBox *combo = new QComboBox(this);
    QStringList filterTypes;
    filterTypes.append("All");
    filterTypes += m_hierarchy
                       ? m_hierarchy->getAvailableSensorTypes()
                       : QStringList({"Generic", "CSM", "ESM", "EO", "IR", "Sonar", "AIS", "ADSB", "AESA"});

    for (const QString &t : filterTypes) {
        combo->addItem(t, t);
    }

    combo->setCurrentText("All");
    combo->setFixedWidth(70);
    combo->setStyleSheet(
        "QComboBox { color: white; background-color: #1A3652; "
        "border: 1px solid #27446d; padding: 4px; }"
        "QComboBox::drop-down { border: none; width: 20px; }"
        "QComboBox::down-arrow { image: url(:/icons/images/down.png); "
        "width: 16px; height: 16px; }"
        "QComboBox QAbstractItemView { background-color: #1A3652; "
        "color: white; selection-background-color: #27446d; }");
    return combo;
}

// %%% "Sensor Type:" row %%%
void AddItemDialog::buildSensorTypeRow(QVBoxLayout *mainLayout, bool isDatabaseEditor)
{
    m_sensorTypeContainer = new QWidget(this);
    QHBoxLayout *sensorTypeLayout = new QHBoxLayout(m_sensorTypeContainer);
    sensorTypeLayout->setContentsMargins(0, 0, 0, 0);
    QLabel *sensorLabel = new QLabel("Sensor Type:", this);
    sensorLabel->setStyleSheet("color: white;");
    sensorTypeLayout->addWidget(sensorLabel);
    sensorTypeComboBox = new QComboBox(this);
    QStringList sensorTypes = m_hierarchy
                                  ? m_hierarchy->getAvailableSensorTypes()
                                  : QStringList({"Generic", "CSM", "ESM", "EO", "IR", "Sonar", "AIS", "ADSB", "AESA"});
    sensorTypeComboBox->addItems(sensorTypes);
    sensorTypeComboBox->setCurrentText("Generic");
    sensorTypeComboBox->setStyleSheet("color: white;");
    sensorTypeLayout->addWidget(sensorTypeComboBox);
    sensorTypeLayout->addStretch();
    if (isDatabaseEditor) {
        m_sensorTypeContainer->setVisible(true);
    } else {
        m_sensorTypeContainer->setVisible(false);
    }
    mainLayout->addWidget(m_sensorTypeContainer);
}

// %%% Sensor chosen from completer -> set type combo from DB %%%
void AddItemDialog::applySensorTypeFromSelection(bool isDatabaseEditor)
{
    if (!sensorTypeComboBox) {
        return;
    }
    Hierarchy* lib = m_hierarchy;
    if (lib) {
        auto it = lib->Sensors.find(selectedEntityId.toStdString());
        if (it != lib->Sensors.end() && it->second) {
            Sensor* sens = it->second;
            QString subTypeStr = sens->subTypeToString(sens->subType);
            int idx = sensorTypeComboBox->findText(subTypeStr);
            if (idx >= 0) sensorTypeComboBox->setCurrentIndex(idx);
        }
    }
    if (!isDatabaseEditor) {
        sensorTypeComboBox->setEnabled(false);
        sensorTypeComboBox->setToolTip("Type automatically set from selected sensor");
    }
}

// %%% Search box cleared -> unlock + reset sensor type %%%
void AddItemDialog::resetSensorTypeOnSearchCleared()
{
    if (!sensorTypeComboBox) {
        return;
    }
    sensorTypeComboBox->setEnabled(true);
    sensorTypeComboBox->setToolTip("");
    if (sensorTypeComboBox->findText("Generic") >= 0)
        sensorTypeComboBox->setCurrentText("Generic");
    else if (sensorTypeComboBox->count() > 0)
        sensorTypeComboBox->setCurrentIndex(0);
}

// %%% clearEntitySelection() sensor part %%%
void AddItemDialog::resetSensorTypeToDefault()
{
    if (sensorTypeComboBox) {
        sensorTypeComboBox->setEnabled(true);
        sensorTypeComboBox->setToolTip("");
        sensorTypeComboBox->setCurrentText("Generic");
    }
}

// %%% Display name for a sensor in the search list %%%
// Returns false if the sensor is filtered out by categoryFilter.
bool AddItemDialog::buildSensorDisplayName(const std::string& entityId,
                                           const QString& eName,
                                           const QString& categoryFilter,
                                           QString& displayName) const
{
    QString subType = "";
    auto it = m_hierarchy->Sensors.find(entityId);
    if (it != m_hierarchy->Sensors.end() && it->second) {
        subType = it->second->subTypeToString(it->second->subType);
    }
    if (!categoryFilter.isEmpty() && categoryFilter != "All") {
        if (subType != categoryFilter) return false;
    }
    displayName = subType.isEmpty() ? eName : eName + " (" + subType + ")";
    return true;
}

// %%% Validation: sensor must be picked from DB (or Add New used) %%%
bool AddItemDialog::validateSensorSelection()
{
    if (!entitySearchLineEdit ||
        entitySearchLineEdit->text().trimmed().isEmpty() ||
        selectedEntityId.isEmpty())
    {
        QMessageBox msgBox(this);
        msgBox.setStyleSheet(AddItemDialogStyles::MessageBox);
        msgBox.setWindowTitle("Sensor Selection Required");
        msgBox.setText("Please select a Sensor from the Database before proceeding.\n\n"
                       "Use the search field to find and select a sensor,\n"
                       "or click '✚ Add New' to create a new one.");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.exec();
        if (entitySearchLineEdit) entitySearchLineEdit->setFocus();
        return false;
    }
    return true;
}

/* Populate sensor profiles */
void AddItemDialog::populateSensorProfiles()
{
    populateProfiles("Sensor");
}

/* Get selected sensor type */
QString AddItemDialog::getSensorType() const {
    return sensorTypeComboBox ? sensorTypeComboBox->currentText() : "Generic";
}
