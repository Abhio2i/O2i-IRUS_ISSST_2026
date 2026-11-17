#ifndef ADDITEMDIALOG_H
#define ADDITEMDIALOG_H

#include "GUI/Hierarchytree/customtrajectorydialog.h"
#include <QDialog>
#include <QVariantMap>
#include <QHBoxLayout>
#include <QCompleter>
#include <QStringListModel>
#include <string>
#include <QListWidget>
#include <QList>

class QLineEdit;
class QComboBox;
class QCheckBox;
class QGroupBox;
class QSpinBox;
class QPushButton;
class QVBoxLayout;
class Hierarchy;

class AddItemDialog : public QDialog
{
    Q_OBJECT

public:
    QString getSelectedEntityId() const;

    enum DialogType {
        EntityType,
        FolderType
    };

    enum DialogMode {
        NormalMode,
        ComponentSensorMode,
        ComponentIFFMode,
        ComponentRadioMode,
        ComponentWeaponMode
    };

    QVariantMap getEntityComponents() const;

    AddItemDialog(DialogType type,
                  const QString &specificType,
                  DialogMode dialogMode,
                  Hierarchy* hierarchy,
                  QWidget *parent = nullptr,
                  const QString& editorContext = "");

    struct EntitySelection {
        QString entityId;
        QString displayName;
        QString profileName;
        QString name;
        int     count = 1;
        QString team  = "None";

        bool    scEnabled = false;
        bool    scSensors = false, scIff = false, scRadio = false;
        int     scTypeIndex = 0;
        QString city;
        int     range = 100;
        int     minRadio = 10, maxRadio = 100;
        int     minRadar = 50, maxRadar = 500;
        int     minSpeed = 800, maxSpeed = 5000;
        int     minTurn = 25,  maxTurn = 50;
        int     sectorStart = 0,  sectorEnd = 90;
        int     sectorMinRange = 10, sectorMaxRange = 100;
        TrajectoryConfig traj;
    };

    int  selectionCount() const { return m_selections.isEmpty() ? 1 : m_selections.size(); }
    void activateSelection(int index);


    CustomTrajectoryDialog customDialog;
    QString getName() const;
    void setNumber(int value);
    int getNumber() const;
    QVariantMap getComponents() const;
    QString getSensorType() const;
    QString getProfileId() const;
    QString getProfileName() const;
    QString getComponentType() const;
    QString getRadioType() const;
    QString getIFFType() const;
    QString getWeaponType() const;
    bool isScenarioconfigEnabled() const;
    QString getScType() const;
    QPointF getCity() const;
    int getScRange() const;
    int getMinRadioRange() const;
    int getMaxRadioRange() const;
    int getMinRadarRange() const;
    int getMaxRadarRange() const;
    QString getTrajectory() const;
    int getMinPlaneSpeed() const;
    int getMaxPlaneSpeed() const;
    int getMinTurnRadius() const;
    int getMaxTurnRadius() const;
    bool isScEnabled() const { return isScenarioconfigEnabled(); }
    int getRange() const { return getScRange(); }
    bool isScSensorsEnabled() const;
    bool isScIffEnabled() const;
    bool isScRadioEnabled() const;
    void populateEntityProfiles(const QString &profileTypeFilter = "",
                                const QString &categoryFilter = "All");
    QString getSelectedTeam() const;
    bool isSectorType() const;
    int  getSectorStartAngle() const;
    int  getSectorEndAngle() const;
    int  getSectorMinRange() const;
    int  getSectorMaxRange() const;

private slots:
    void onScCheckBoxStateChanged(int state);
    void onScTypeChanged(const QString& text);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    QString m_editorContext;
    QString selectedEntityId;
    QVariantMap entityComponents;
    void setupUI(DialogType type);
    void setupScSection();
    bool detectDatabaseEditorFromWindow();
    bool validateInputs();
    QHBoxLayout* createMinMaxSpinBoxPair(const QString& label,
                                         QSpinBox*& minSpinBox,
                                         QSpinBox*& maxSpinBox,
                                         int minDefault,
                                         int maxDefault,
                                         int minRange,
                                         int maxRange,
                                         QString unit = " m");

    void populateSensorProfiles();
    void populateIFFProfiles();
    void populateRadioProfiles();
    void populateWeaponProfiles();
    void populateProfiles(const QString& profileType);

    // ---- Sensor dialog (additemdialog_sensor.cpp) ----
    void buildSensorAddNewButton(QVBoxLayout *mainLayout);
    void connectSensorAddNewButton(bool isDatabaseEditor);
    QComboBox* createSensorCategoryFilter();
    void buildSensorTypeRow(QVBoxLayout *mainLayout, bool isDatabaseEditor);
    void applySensorTypeFromSelection(bool isDatabaseEditor);
    void resetSensorTypeOnSearchCleared();
    void resetSensorTypeToDefault();
    bool buildSensorDisplayName(const std::string& entityId, const QString& eName,
                                const QString& categoryFilter, QString& displayName) const;
    bool validateSensorSelection();

    void buildRadioTypeRow(QVBoxLayout *mainLayout);
    void resetRadioTypeToDefault();

    void buildIFFTypeRow(QVBoxLayout *mainLayout);
    void resetIFFTypeToDefault();

    QLineEdit *nameLineEdit;
    QLineEdit *numberLineEdit;
    QComboBox *sensorTypeComboBox;
    QComboBox *radioTypeComboBox;
    QComboBox *iffTypeComboBox;
    QComboBox *weaponTypeComboBox = nullptr;
    QComboBox *profileComboBox;
    QCheckBox *scCheckBox;
    QGroupBox *scOptionsGroup;
    QWidget   *scComponentsWidget = nullptr;
    QComboBox *scTypeComboBox;
    QComboBox *cityComboBox;
    QLineEdit *rangeLineEdit;
    QCheckBox *scSensorsCheckBox = nullptr;
    QCheckBox *scIffCheckBox     = nullptr;
    QCheckBox *scRadioCheckBox   = nullptr;
    QSpinBox *minRadioRangeSpinBox;
    QSpinBox *maxRadioRangeSpinBox;
    QSpinBox *minRadarRangeSpinBox;
    QSpinBox *maxRadarRangeSpinBox;
    QComboBox *trajectoryComboBox;
    QSpinBox *minPlaneSpeedSpinBox;
    QSpinBox *maxPlaneSpeedSpinBox;
    QSpinBox *minTurnRadiusSpinBox;
    QSpinBox *maxTurnRadiusSpinBox;
    QMap<QString, QCheckBox*> componentCheckboxes;
    QString specificType;
    DialogMode m_dialogMode;
    Hierarchy* m_hierarchy;
    void populateComponentsFromEntity(const QString& entityId, const QString& profileName);
    QLineEdit *entitySearchLineEdit;
    QComboBox *profileFilterComboBox;
    QCompleter *entityCompleter;
    QMap<QString, QVariantList> entityMap;
    void clearEntitySelection();
    QString m_profileContext;
    QString determineProfileContext(const QString& specificType,
                                    DialogMode dialogMode,
                                    const QString& editorContext);
    QComboBox *teamSelectComboBox = nullptr;
    bool        m_addNewMode            = false;
    QWidget*    m_entitySearchContainer = nullptr;
    QPushButton* m_addNewBtn            = nullptr;
    QWidget* m_sensorTypeContainer = nullptr;
    QWidget* m_radioTypeContainer = nullptr;
    QWidget* m_iffTypeContainer = nullptr;

    bool m_multiSelect = false;
    QList<EntitySelection> m_selections;
    int  m_currentSel = -1;
    QListWidget* m_selectedList = nullptr;

    void addSelection(const QString& displayName, const QString& id, const QString& profileName);
    void removeCurrentSelection();
    void saveFormToSelection(int idx);
    void loadSelectionToForm(int idx);
    void refreshSelectionItemText(int idx);
    QWidget  *scRangeWidget  = nullptr;
    QWidget  *scSectorWidget = nullptr;
    QSpinBox *sectorStartSpin    = nullptr;
    QSpinBox *sectorEndSpin      = nullptr;
    QSpinBox *sectorMinRangeSpin = nullptr;
    QSpinBox *sectorMaxRangeSpin = nullptr;

};

#endif // ADDITEMDIALOG_H
