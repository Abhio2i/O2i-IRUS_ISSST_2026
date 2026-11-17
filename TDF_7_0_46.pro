QT += core gui widgets xml svg sql websockets charts
# QT       += core gui
QT += 3dcore 3drender 3dinput 3dextras
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
# TEMPLATE = subdirs
QT += network printsupport
QT += concurrent
QT += sql

CONFIG += c++17
INCLUDEPATH += $$PWD/angelscript/include
LIBS += -L$$PWD/angelscript/lib -langelscript
DEFINES += BT_NO_SIMD_OPERATOR
DEFINES += BT_USE_SSE
DEFINES += BT_USE_SSE_IN_API
# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
INCLUDEPATH += /usr/include/qgis
INCLUDEPATH += /usr/include/qgis/core
INCLUDEPATH += /usr/include/qgis/gui
INCLUDEPATH += /usr/include/qgis/analysis
INCLUDEPATH += $$PWD/GIS_API
INCLUDEPATH += $$PWD/GIS_API/frontend
INCLUDEPATH += $$PWD/GIS_API/frontend/widgets
INCLUDEPATH += $$PWD/GIS_API/frontend/toolbars
INCLUDEPATH += $$PWD/GIS_API/frontend/panels
INCLUDEPATH += $$PWD/GIS_API/frontend/dialogs
INCLUDEPATH += $$PWD/GIS_API/backend
INCLUDEPATH += $$PWD/GIS_API/backend/api
INCLUDEPATH += $$PWD/GIS_API/backend/engine
INCLUDEPATH += $$PWD/GIS_API/backend/managers
INCLUDEPATH += $$PWD/GIS_API/backend/shapes
INCLUDEPATH += $$PWD/GIS_API/backend/formats
INCLUDEPATH += $$PWD/GIS_API/common

LIBS += -L/usr/lib \
    -lqgis_core \
    -lqgis_gui \
    -lqgis_analysis \
    -lproj \

# Header path
INCLUDEPATH += /usr/include/gdal

# Library
LIBS += -lgdal

SOURCES += \
GIS_API/backend/api/airbase_api.cpp \
GIS_API/backend/api/elevation_data.cpp \
GIS_API/backend/api/layer_api.cpp \
GIS_API/backend/api/map_api.cpp \
GIS_API/backend/api/measure_api.cpp \
GIS_API/backend/api/shape_api.cpp \
GIS_API/backend/api/shape_history_api.cpp \
GIS_API/backend/formats/builtin_layer_formats.cpp \
GIS_API/backend/formats/geojson_simple.cpp \
GIS_API/backend/formats/layer_format_registry.cpp \
GIS_API/backend/engine/gis_engine.cpp \
GIS_API/backend/shapes/builtin_shape_types.cpp \
GIS_API/backend/shapes/ishape_type.cpp \
GIS_API/backend/shapes/shape_type_registry.cpp \
GIS_API/backend/managers/layer_manager.cpp \
GIS_API/backend/managers/shape_manager.cpp \
GIS_API/frontend/panels/api_layer_panel.cpp \
GIS_API/frontend/panels/layer_widget.cpp \
GIS_API/frontend/toolbars/api_map_toolbar.cpp \
GIS_API/frontend/widgets/map_widget.cpp \
GIS_API/frontend/dialogs/measure_distance_dialog.cpp \
GIS_API/frontend/ui_action_api.cpp \
    GUI/Canvas/canvas_action_api.cpp \
    GUI/Canvas/canvas_layer/dynamiccanvas.cpp \
    GUI/Canvas/canvas_layer/editorcanvas.cpp \
    GUI/Canvas/canvas_layer/staticcanvas.cpp \
    GUI/Canvas/context_menu/canvas_context_menu.cpp \
    GUI/Canvas/context_menu/entity_context_menu.cpp \
    GUI/Canvas/context_menu/trajectory_context_menu.cpp \
    GUI/Canvas/map_with_canvas.cpp \
    GUI/Canvas/overlay_canvas.cpp \
    GUI/Console/consoleview.cpp \
    GUI/DOCTRINE/doctrineareadefinition.cpp \
    GUI/DOCTRINE/doctrineassumptionsnotes.cpp \
    GUI/DOCTRINE/doctrineparameters.cpp \
    GUI/DOCTRINE/tacticalrules.cpp \
    GUI/Editors/RuntimeStatisticsDialog.cpp \
    GUI/Editors/StatisticsWidgets.cpp \
    GUI/Editors/analysiseditor.cpp \
    GUI/Editors/customresizableoverlaydock.cpp \
    GUI/Editors/databaseeditor.cpp \
    GUI/Editors/missioneditor.cpp \
    GUI/Editors/recentprojectsmanager.cpp \
    GUI/Editors/reportseditor.cpp \
    GUI/Editors/runtimeeditor.cpp \
    GUI/Editors/scenarioeditor.cpp \
    GUI/Feedback/projectinformation.cpp \
    GUI/Hierarchytree/addformationdialog.cpp \
    GUI/Hierarchytree/additemdialog/additemdialog.cpp \
    GUI/Hierarchytree/additemdialog/additemdialog_iff.cpp \
    GUI/Hierarchytree/additemdialog/additemdialog_radio.cpp \
    GUI/Hierarchytree/additemdialog/additemdialog_sensor.cpp \
    GUI/Hierarchytree/additemdialog/addweapondialog.cpp \
    GUI/Hierarchytree/contextmenu.cpp \
    GUI/Hierarchytree/customtrajectorydialog.cpp \
    GUI/Hierarchytree/formationcanvaswidget.cpp \
    GUI/Hierarchytree/hierarchyconnector.cpp \
    GUI/Hierarchytree/hierarchytree.cpp \
    # GUI/Inspector/customparameterdialog.cpp \
    # GUI/Inspector/customparameterdialog.cpp \
    GUI/Inspector/customparameterdialog.cpp \
    GUI/Inspector/inspector.cpp \
    GUI/Inspector/template/colortemplate.cpp \
    GUI/Inspector/template/geocordstemplate.cpp \
    GUI/Inspector/template/iconsdialog.cpp \
    GUI/Inspector/template/imagetemplate.cpp \
    GUI/Inspector/template/optiontemplate.cpp \
    GUI/Inspector/template/vectortemplate.cpp \
    GUI/Logger/loggerdialog.cpp \
    GUI/Menubars/menubar.cpp \
    GUI/Menubars/profileinfodialog.cpp \
    GUI/Navigation/navigationpage.cpp \
    GUI/Panel/EODisplayLib/eoland.cpp \
    GUI/Panel/EODisplayLib/eosky.cpp \
    GUI/Panel/EODisplayLib/eotargets.cpp \
    GUI/Panel/EODisplayLib/eovisionangle.cpp \
    GUI/Panel/EODisplayLib/eowether.cpp \
    GUI/Panel/IRDisplayLib/irland.cpp \
    GUI/Panel/IRDisplayLib/irsky.cpp \
    GUI/Panel/IRDisplayLib/irtargets.cpp \
    GUI/Panel/IRDisplayLib/irvisionangle.cpp \
    GUI/Panel/IRDisplayLib/irwether.cpp \
    GUI/Panel/adsbdisplay.cpp \
    GUI/Panel/aesaradardisplay.cpp \
    GUI/Panel/aisdisplay.cpp \
    GUI/Panel/csmdisplay.cpp \
    GUI/Panel/eodisplay.cpp \
    GUI/Panel/eoopengldisplay.cpp \
    GUI/Panel/esmdisplay.cpp \
    GUI/Panel/ewdisplay.cpp \
    GUI/Panel/iffdisplay.cpp \
    GUI/Panel/irdisplay.cpp \
    GUI/Panel/iropengldisplay.cpp \
    GUI/Panel/radardisplay.cpp \
    GUI/Panel/radiodisplay.cpp \
    GUI/Panel/sonardisplay.cpp \
    GUI/Panel/sonobuoypanel.cpp \
    GUI/Settings/applicationdialog.cpp \
    GUI/Sidebar/sidebarwidget.cpp \
    GUI/Tacticaldisplay/Airbase/airbase.cpp \
    GUI/Tacticaldisplay/Gis/custommapdialog.cpp \
    GUI/Tacticaldisplay/Gis/gislib.cpp \
    GUI/Tacticaldisplay/Gis/gisnetwork.cpp \
    GUI/Tacticaldisplay/Layer/csv.cpp \
    GUI/Tacticaldisplay/Layer/flatgeobuf.cpp \
    GUI/Tacticaldisplay/Layer/geojson.cpp \
    GUI/Tacticaldisplay/Layer/gml.cpp \
    GUI/Tacticaldisplay/Layer/kml.cpp \
    GUI/Tacticaldisplay/Layer/layerinformationdialog.cpp \
    GUI/Tacticaldisplay/Layer/layerpanel.cpp \
    GUI/Tacticaldisplay/Layer/shapefile.cpp \
    GUI/Tacticaldisplay/Measurement/measurement.cpp \
    GUI/Tacticaldisplay/Shape/shapeeventhandler.cpp \
    GUI/Tacticaldisplay/Shape/shapes_feature.cpp \
    GUI/Tacticaldisplay/canvaswidget.cpp \
    GUI/Tacticaldisplay/contextmenu/contextmenu_handler.cpp \
    GUI/Tacticaldisplay/entityinfodialog.cpp \
    GUI/Tacticaldisplay/scalebar.cpp \
    GUI/Tacticaldisplay/tacticaldisplay.cpp \
    GUI/Tacticaldisplay/tooltiphelper.cpp \
    GUI/Tacticaldisplay/waypointeditdialog.cpp \
    GUI/Testscript/angelscripthighlighter.cpp \
    GUI/Testscript/testscriptdialog.cpp \
    GUI/Testscript/textscriptwidget.cpp \
    GUI/Theme/theme.cpp \
    GUI/Timing/graphwidget.cpp \
    GUI/Toolbars/designtoolbar.cpp \
    GUI/Toolbars/networktoolbar.cpp \
    GUI/Toolbars/runtimetoolbar.cpp \
    GUI/Toolbars/standardtoolbar.cpp \
    GUI/Toolbars/timetoolbar.cpp \
    GUI/mainwindow.cpp \
    GUI/measuredistance/measuredistancedialog.cpp \
    GUI/plugins/pluginmanagerdialog.cpp \
    GUI/scene3dwidget/scene3dwidget.cpp \
    GUI/splashwindow.cpp \
    GUI/statusbar.cpp \
    Setup.cpp \
    angelscript/add_on/scriptarray/scriptarray.cpp \
    angelscript/add_on/scriptstdstring/scriptstdstring.cpp \
    # core/Components/attachedenitities.cpp \
    # core/Components/collider.cpp \
    # core/Components/component.cpp \
    # core/Components/dynamicmodel.cpp \
    # core/Components/formation.cpp \
    # core/Components/mesh.cpp \
    # core/Components/meshrenderer2d.cpp \
    # core/Components/mission.cpp \
    # core/Components/networkobject.cpp \
    # core/Components/rigidbody.cpp \
    # core/Components/trajectory.cpp \
    # core/Components/transform.cpp \
    core/Config/scenarioconfig.cpp \
    core/Config/sessionmanager.cpp \
    core/Config/test/scenarioconfig_test.cpp \
    core/DISPlugin/DISNetworkPlugin.cpp \
    core/DISPlugin/bridge/disnetworkbridge.cpp \
    core/DISPlugin/config/disconfigloader.cpp \
    core/DISPlugin/config/disreferencedataloader.cpp \
    core/DISPlugin/core/dismanager.cpp \
    core/DISPlugin/core/pdudispatcher.cpp \
    core/DISPlugin/core/pdusender.cpp \
    core/DISPlugin/core/pduserializer.cpp \
    core/DISPlugin/handlers/acknowledgehandler.cpp \
    core/DISPlugin/handlers/datahandler.cpp \
    core/DISPlugin/handlers/dataqueryhandler.cpp \
    core/DISPlugin/handlers/designatorhandler.cpp \
    core/DISPlugin/handlers/emissionhandler.cpp \
    core/DISPlugin/handlers/entitystatehandler.cpp \
    core/DISPlugin/handlers/exercisehandler.cpp \
    core/DISPlugin/handlers/iffhandler.cpp \
    core/DISPlugin/handlers/setdatahandler.cpp \
    core/DISPlugin/handlers/signalhandler.cpp \
    core/DISPlugin/handlers/transferownershiphandler.cpp \
    core/DISPlugin/handlers/transmitterhandler.cpp \
    core/DISPlugin/handlers/warfarehandlers.cpp \
    core/DISPlugin/transport/DISTransport.cpp \
    core/DISPlugin/utils/coordconverter.cpp \
    core/DISPlugin/utils/deadreckoning.cpp \
    core/DISPlugin/utils/disdatumregistry.cpp \
    core/DISPlugin/utils/entityidmapper.cpp \
    core/DISPlugin/version/pduversionadapter.cpp \
    core/Debug/console.cpp \
    core/Debug/frame.cpp \
    core/Debug/profiler.cpp \
    core/Debug/test/console_test.cpp \
    core/Debug/test/frame_test.cpp \
    core/Dynamics/Model/aircraft.cpp \
    core/Dynamics/Model/seamodel.cpp \
    core/Dynamics/dynamics.cpp \
    core/GlobalRegistry.cpp \
    core/Hierarchy/Components/Test/collider_test.cpp \
    core/Hierarchy/Components/Test/crosssection_test.cpp \
    core/Hierarchy/Components/Test/dynamicmodel_test.cpp \
    core/Hierarchy/Components/Test/iffProfile_test.cpp \
    core/Hierarchy/Components/Test/mesh_test.cpp \
    core/Hierarchy/Components/Test/meshrenderer2d_test.cpp \
    core/Hierarchy/Components/Test/radioProfile_test.cpp \
    core/Hierarchy/Components/Test/sensorprofile_test.cpp \
    core/Hierarchy/Components/Test/trajectory_test.cpp \
    core/Hierarchy/Components/Test/transform_test.cpp \
    core/Hierarchy/Components/Test/weaponProfile_test.cpp \
    core/Hierarchy/Components/attachedenitities.cpp \
    core/Hierarchy/Components/collider.cpp \
    core/Hierarchy/Components/component.cpp \
    core/Hierarchy/Components/crosssection.cpp \
    core/Hierarchy/Components/distypeprofile.cpp \
    core/Hierarchy/Components/dynamicmodel.cpp \
    core/Hierarchy/Components/iffprofile.cpp \
    core/Hierarchy/Components/mesh.cpp \
    core/Hierarchy/Components/meshrenderer2d.cpp \
    core/Hierarchy/Components/mission.cpp \
    core/Hierarchy/Components/networkobject.cpp \
    core/Hierarchy/Components/radioprofile.cpp \
    core/Hierarchy/Components/rigidbody.cpp \
    core/Hierarchy/Components/sensorprofile.cpp \
    core/Hierarchy/Components/trajectory.cpp \
    core/Hierarchy/Components/transform.cpp \
    core/Hierarchy/Components/weaponprofile.cpp \
    core/Hierarchy/ECS/System/DynamicSystem.cpp \
    core/Hierarchy/EntityProfiles/IFFs/customiff.cpp \
    core/Hierarchy/EntityProfiles/Radio/customradio.cpp \
    core/Hierarchy/EntityProfiles/Radio/src/propagation_model.cpp \
    core/Hierarchy/EntityProfiles/Radio/src/radiolib.cpp \
    core/Hierarchy/EntityProfiles/Radio/src/utils/math_utils.cpp \
    core/Hierarchy/EntityProfiles/SensorProfiles/AIS_ADSB/ADSB/ADSB.cpp \
    core/Hierarchy/EntityProfiles/SensorProfiles/AIS_ADSB/AIS/AIS.cpp \
    core/Hierarchy/EntityProfiles/SensorProfiles/AIS_ADSB/RF/RF.cpp \
    core/Hierarchy/EntityProfiles/SensorProfiles/CSM/csmdetector.cpp \
    core/Hierarchy/EntityProfiles/SensorProfiles/CSM/csmfrequency.cpp \
    core/Hierarchy/EntityProfiles/SensorProfiles/CSM/csmmonitor.cpp \
    core/Hierarchy/EntityProfiles/SensorProfiles/CSM/csmsignalmodel.cpp \
    core/Hierarchy/EntityProfiles/SensorProfiles/CSM/csmspectrum.cpp \
    core/Hierarchy/EntityProfiles/SensorProfiles/CSM/csmtypes.cpp \
    core/Hierarchy/EntityProfiles/SensorProfiles/EOIRSensorLib/EOIRCommonLib/environmental_conditions.cpp \
    core/Hierarchy/EntityProfiles/SensorProfiles/EOIRSensorLib/EOSensorLib/eosensorlib.cpp \
    core/Hierarchy/EntityProfiles/SensorProfiles/EOIRSensorLib/IRSensorLib/lwir.cpp \
    core/Hierarchy/EntityProfiles/SensorProfiles/EOIRSensorLib/IRSensorLib/mwir.cpp \
    core/Hierarchy/EntityProfiles/SensorProfiles/EOIRSensorLib/IRSensorLib/stanag_4347.cpp \
    core/Hierarchy/EntityProfiles/SensorProfiles/ESM/esmemitter_aesa.cpp \
    core/Hierarchy/EntityProfiles/SensorProfiles/ESM/esmemitter_radar.cpp \
    core/Hierarchy/EntityProfiles/SensorProfiles/ESM/esmobservationmodel.cpp \
    core/Hierarchy/EntityProfiles/SensorProfiles/ESM/esmppiwidget.cpp \
    core/Hierarchy/EntityProfiles/SensorProfiles/ESM/esmspectrumwidget.cpp \
    core/Hierarchy/EntityProfiles/SensorProfiles/ESM/esmrf.cpp \
    core/Hierarchy/EntityProfiles/SensorProfiles/adsbsensor.cpp \
    core/Hierarchy/EntityProfiles/SensorProfiles/aesaradar.cpp \
    core/Hierarchy/EntityProfiles/SensorProfiles/aisaRadarLib/radarantenna_aesa.cpp \
    core/Hierarchy/EntityProfiles/SensorProfiles/aisaRadarLib/radarmodel_aesa.cpp \
    core/Hierarchy/EntityProfiles/SensorProfiles/aisaRadarLib/radarscheduler.cpp \
    core/Hierarchy/EntityProfiles/SensorProfiles/aisaRadarLib/radarsignallibrary_aesa.cpp \
    core/Hierarchy/EntityProfiles/SensorProfiles/aisaRadarLib/radarsignalprocessor_aesa.cpp \
    core/Hierarchy/EntityProfiles/SensorProfiles/aisaRadarLib/radartracker_aesa.cpp \
    core/Hierarchy/EntityProfiles/SensorProfiles/aisaRadarLib/test-aesa/Radartracker_aesa_test.cpp \
    core/Hierarchy/EntityProfiles/SensorProfiles/aisaRadarLib/test-aesa/radarantenna_aesa_test.cpp \
    core/Hierarchy/EntityProfiles/SensorProfiles/aisaRadarLib/test-aesa/radarmodel_aesa_test.cpp \
    core/Hierarchy/EntityProfiles/SensorProfiles/aisaRadarLib/test-aesa/radarscheduler_test.cpp \
    core/Hierarchy/EntityProfiles/SensorProfiles/aisaRadarLib/test-aesa/radarsignallibrary_aesa_test.cpp \
    core/Hierarchy/EntityProfiles/SensorProfiles/aisaRadarLib/test-aesa/radarsignalprocessor_aesa_test.cpp \
    core/Hierarchy/EntityProfiles/SensorProfiles/aissensor.cpp \
    core/Hierarchy/EntityProfiles/SensorProfiles/csm.cpp \
    core/Hierarchy/EntityProfiles/SensorProfiles/customsensor.cpp \
    core/Hierarchy/EntityProfiles/SensorProfiles/eoSensorLib/eo.cpp \
    core/Hierarchy/EntityProfiles/SensorProfiles/eoSensorLib/eo_ir.cpp \
    core/Hierarchy/EntityProfiles/SensorProfiles/eoSensorLib/eovision.cpp \
    core/Hierarchy/EntityProfiles/SensorProfiles/eoSensorLib/georelativeangle.cpp \
    core/Hierarchy/EntityProfiles/SensorProfiles/eoSensorLib/georelativeelevation.cpp \
    core/Hierarchy/EntityProfiles/SensorProfiles/EOIRSensorLib/EOIRCommonLib/eoircommonlib.cpp \
    core/Hierarchy/EntityProfiles/SensorProfiles/EOIRSensorLib/IRSensorLib/irsensorlib.cpp \
    core/Hierarchy/EntityProfiles/SensorProfiles/eosensor.cpp \
    core/Hierarchy/EntityProfiles/SensorProfiles/esm.cpp \
    core/Hierarchy/EntityProfiles/SensorProfiles/irsensor.cpp \
    core/Hierarchy/EntityProfiles/SensorProfiles/radar.cpp \
    # core/Hierarchy/EntityProfiles/SensorProfiles/radarmodel.cpp \
    core/Hierarchy/EntityProfiles/SensorProfiles/radarlib/radarantenna.cpp \
    core/Hierarchy/EntityProfiles/SensorProfiles/radarlib/radarmodel.cpp \
    core/Hierarchy/EntityProfiles/SensorProfiles/radarlib/radarsignallibrary.cpp \
    core/Hierarchy/EntityProfiles/SensorProfiles/radarlib/radarsignalprocessor.cpp \
    core/Hierarchy/EntityProfiles/SensorProfiles/radarlib/radartracker.cpp \
    core/Hierarchy/EntityProfiles/SensorProfiles/sonar.cpp \
    core/Hierarchy/EntityProfiles/SensorProfiles/sonar/active_sonar.cpp \
    core/Hierarchy/EntityProfiles/SensorProfiles/sonar/sonar_model.cpp \
    core/Hierarchy/EntityProfiles/SensorProfiles/test/aesaradar_test.cpp \
    # core/Hierarchy/EntityProfiles/SensorProfiles/tests/csmjammertest/csmjammer_test.cpp \
    core/Hierarchy/EntityProfiles/Test/fixedpoints_test.cpp \
    core/Hierarchy/EntityProfiles/Test/platform_test.cpp \
    core/Hierarchy/EntityProfiles/Test/specialzone_test.cpp \
    core/Hierarchy/EntityProfiles/fixedpoints.cpp \
    core/Hierarchy/EntityProfiles/formation.cpp \
    core/Hierarchy/EntityProfiles/iff.cpp \
    core/Hierarchy/EntityProfiles/platform.cpp \
    core/Hierarchy/EntityProfiles/radio.cpp \
    core/Hierarchy/EntityProfiles/sensor.cpp \
    core/Hierarchy/EntityProfiles/specialzone.cpp \
    core/Hierarchy/EntityProfiles/weapon.cpp \
    core/Hierarchy/EntityProfiles/weapons/artillery.cpp \
    core/Hierarchy/EntityProfiles/weapons/bomb.cpp \
    core/Hierarchy/EntityProfiles/weapons/chaff.cpp \
    core/Hierarchy/EntityProfiles/weapons/flare.cpp \
    core/Hierarchy/EntityProfiles/weapons/missile.cpp \
    core/Hierarchy/EntityProfiles/weapons/rocket.cpp \
    core/Hierarchy/EntityProfiles/weapons/sonobuoy.cpp \
    core/Hierarchy/EntityProfiles/weapons/torpedo.cpp \
    core/Hierarchy/Struct/action.cpp \
    core/Hierarchy/Struct/color.cpp \
    core/Hierarchy/Struct/condition.cpp \
    core/Hierarchy/Struct/constants.cpp \
    core/Hierarchy/Struct/formationposition.cpp \
    core/Hierarchy/Struct/geocords.cpp \
    core/Hierarchy/Struct/parameter.cpp \
    core/Hierarchy/Struct/test/Geocords_test.cpp \
    core/Hierarchy/Struct/test/Waypoints_test.cpp \
    core/Hierarchy/Struct/test/formationposition_test.cpp \
    core/Hierarchy/Struct/test/parameter_test.cpp \
    core/Hierarchy/Struct/test/vector_test.cpp \
    core/Hierarchy/Struct/vector.cpp \
    core/Hierarchy/Struct/waypoints.cpp \
    core/Hierarchy/Test/folder_test.cpp \
    core/Hierarchy/Test/hierarchy_test.cpp \
    core/Hierarchy/Test/profilecategaory_test.cpp \
    core/Hierarchy/Utils/entityutils.cpp \
    core/Hierarchy/Utils/test/Entityutils_test.cpp \
    core/Hierarchy/entity.cpp \
    core/Hierarchy/folder.cpp \
    core/Hierarchy/hierarchy.cpp \
    core/Hierarchy/profilecategaory.cpp \
    core/InputSystem/inputmanager.cpp \
    core/Network/entitypdu.cpp \
    core/Network/folderpdu.cpp \
    core/Network/libs/lz4.c \
    core/Network/networkmanager.cpp \
    core/Network/networktransport.cpp \
    core/Network/profilepdu.cpp \
    core/Network/transformpdu.cpp \
    core/Plugins/aesalogicregistry.cpp \
    core/Plugins/pluginmanager.cpp \
    core/Plugins/uipluginmanager.cpp \
    core/Recorder/recorder.cpp \
    core/Render/scenerenderer.cpp \
    core/SQLite/sqlite.cpp \
    core/ScriptEngine/AutoScript/autoscriptconfig.cpp \
    core/ScriptEngine/AutoScript/autoscriptcontroller.cpp \
    core/ScriptEngine/AutoScript/autoscriptmanager.cpp \
    core/ScriptEngine/AutoScript/tcpautoscriptsource.cpp \
    core/ScriptEngine/scriptengine.cpp \
    core/ScriptEngine/scriptenginegis.cpp \
    core/SharedMemory/sharedmemorywrapper.cpp \
    core/Simulation/simulation.cpp \
    # core/Struct/action.cpp \
    # core/Struct/color.cpp \
    # core/Struct/condition.cpp \
    # core/Struct/constants.cpp \
    # core/Struct/formationposition.cpp \
    # core/Struct/geocords.cpp \
    # core/Struct/parameter.cpp \
    # core/Struct/task.cpp \
    # core/Struct/vector.cpp \
    # core/Struct/waypoints.cpp \
    core/Simulation/simulation_state.cpp \
    core/Utility/uuid.cpp \
    core/core_test.cpp \
    core/structure/database.cpp \
    # core/structure/entity.cpp \
    # core/structure/folder.cpp \
    # core/structure/hierarchy.cpp \
    core/structure/missioncreator.cpp \
    core/structure/missionexcuter.cpp \
    # core/structure/profilecategaory.cpp \
    core/structure/runtime.cpp \
    core/structure/scenario.cpp \
    dis7/AcknowledgePdu.cpp \
    dis7/AcknowledgeReliablePdu.cpp \
    dis7/AcousticEmitter.cpp \
    dis7/ActionRequestPdu.cpp \
    dis7/ActionRequestReliablePdu.cpp \
    dis7/ActionResponsePdu.cpp \
    dis7/ActionResponseReliablePdu.cpp \
    dis7/AggregateIdentifier.cpp \
    dis7/AggregateMarking.cpp \
    dis7/AggregateType.cpp \
    dis7/AngleDeception.cpp \
    dis7/AngularVelocityVector.cpp \
    dis7/AntennaLocation.cpp \
    dis7/ArealObjectStatePdu.cpp \
    dis7/ArticulatedParts.cpp \
    dis7/Association.cpp \
    dis7/AttachedParts.cpp \
    dis7/Attribute.cpp \
    dis7/AttributePdu.cpp \
    dis7/BeamAntennaPattern.cpp \
    dis7/BeamData.cpp \
    dis7/BeamStatus.cpp \
    dis7/BlankingSector.cpp \
    dis7/ClockTime.cpp \
    dis7/CollisionElasticPdu.cpp \
    dis7/CollisionPdu.cpp \
    dis7/CommentPdu.cpp \
    dis7/CommentReliablePdu.cpp \
    dis7/CommunicationsNodeID.cpp \
    dis7/CreateEntityPdu.cpp \
    dis7/CreateEntityReliablePdu.cpp \
    dis7/DataPdu.cpp \
    dis7/DataQueryDatumSpecification.cpp \
    dis7/DataQueryPdu.cpp \
    dis7/DataQueryReliablePdu.cpp \
    dis7/DataReliablePdu.cpp \
    dis7/DatumSpecification.cpp \
    dis7/DeadReckoningParameters.cpp \
    dis7/DesignatorPdu.cpp \
    dis7/DetonationPdu.cpp \
    dis7/DirectedEnergyAreaAimpoint.cpp \
    dis7/DirectedEnergyDamage.cpp \
    dis7/DirectedEnergyFirePdu.cpp \
    dis7/DirectedEnergyPrecisionAimpoint.cpp \
    dis7/DirectedEnergyTargetEnergyDeposition.cpp \
    dis7/DistributedEmissionsFamilyPdu.cpp \
    dis7/EEFundamentalParameterData.cpp \
    dis7/EightByteChunk.cpp \
    dis7/ElectromagneticEmissionBeamData.cpp \
    dis7/ElectromagneticEmissionSystemData.cpp \
    dis7/ElectromagneticEmissionsPdu.cpp \
    dis7/EmitterSystem.cpp \
    dis7/EngineFuel.cpp \
    dis7/EngineFuelReload.cpp \
    dis7/EntityAssociation.cpp \
    dis7/EntityDamageStatusPdu.cpp \
    dis7/EntityID.cpp \
    dis7/EntityInformationFamilyPdu.cpp \
    dis7/EntityManagementFamilyPdu.cpp \
    dis7/EntityMarking.cpp \
    dis7/EntityStatePdu.cpp \
    dis7/EntityStateUpdatePdu.cpp \
    dis7/EntityType.cpp \
    dis7/EntityTypeVP.cpp \
    dis7/Environment.cpp \
    dis7/EnvironmentGeneral.cpp \
    dis7/EnvironmentType.cpp \
    dis7/EulerAngles.cpp \
    dis7/EventIdentifier.cpp \
    dis7/EventIdentifierLiveEntity.cpp \
    dis7/EventReportPdu.cpp \
    dis7/EventReportReliablePdu.cpp \
    dis7/Expendable.cpp \
    dis7/ExpendableDescriptor.cpp \
    dis7/ExpendableReload.cpp \
    dis7/ExplosionDescriptor.cpp \
    dis7/FalseTargetsAttribute.cpp \
    dis7/FastEntityStatePdu.cpp \
    dis7/FirePdu.cpp \
    dis7/FixedDatum.cpp \
    dis7/FourByteChunk.cpp \
    dis7/FundamentalOperationalData.cpp \
    dis7/GridAxis.cpp \
    dis7/GroupIdentifier.cpp \
    dis7/IFFFundamentalParameterData.cpp \
    dis7/IFFPdu.cpp \
    dis7/IOCommunicationsNode.cpp \
    dis7/IOEffect.cpp \
    dis7/IffDataSpecification.cpp \
    dis7/IntercomCommunicationsParameters.cpp \
    dis7/IntercomIdentifier.cpp \
    dis7/IntercomSignalPdu.cpp \
    dis7/IsPartOfPdu.cpp \
    dis7/JammingTechnique.cpp \
    dis7/LaunchedMunitionRecord.cpp \
    dis7/LayerHeader.cpp \
    dis7/LinearObjectStatePdu.cpp \
    dis7/LinearSegmentParameter.cpp \
    dis7/LiveEntityIdentifier.cpp \
    dis7/LiveEntityPdu.cpp \
    dis7/LiveSimulationAddress.cpp \
    dis7/LogisticsFamilyPdu.cpp \
    dis7/MineEntityIdentifier.cpp \
    dis7/MinefieldFamilyPdu.cpp \
    dis7/MinefieldIdentifier.cpp \
    dis7/MinefieldResponseNackPdu.cpp \
    dis7/MinefieldStatePdu.cpp \
    dis7/ModulationParameters.cpp \
    dis7/ModulationType.cpp \
    dis7/Munition.cpp \
    dis7/MunitionDescriptor.cpp \
    dis7/MunitionReload.cpp \
    dis7/NamedLocationIdentification.cpp \
    dis7/ObjectIdentifier.cpp \
    dis7/ObjectType.cpp \
    dis7/OwnershipStatus.cpp \
    dis7/Pdu.cpp \
    dis7/PduContainer.cpp \
    dis7/PduHeader.cpp \
    dis7/PduStatus.cpp \
    dis7/PduSuperclass.cpp \
    dis7/PointObjectStatePdu.cpp \
    dis7/PropulsionSystemData.cpp \
    dis7/RadioCommunicationsFamilyPdu.cpp \
    dis7/RadioIdentifier.cpp \
    dis7/RadioType.cpp \
    dis7/ReceiverPdu.cpp \
    dis7/RecordQueryReliablePdu.cpp \
    dis7/RecordQuerySpecification.cpp \
    dis7/RecordSpecification.cpp \
    dis7/RecordSpecificationElement.cpp \
    dis7/Relationship.cpp \
    dis7/RemoveEntityPdu.cpp \
    dis7/RemoveEntityReliablePdu.cpp \
    dis7/RepairCompletePdu.cpp \
    dis7/RepairResponsePdu.cpp \
    dis7/ResupplyOfferPdu.cpp \
    dis7/ResupplyReceivedPdu.cpp \
    dis7/SecondaryOperationalData.cpp \
    dis7/SeesPdu.cpp \
    dis7/Sensor.cpp \
    dis7/SeparationVP.cpp \
    dis7/ServiceRequestPdu.cpp \
    dis7/SetDataPdu.cpp \
    dis7/SetDataReliablePdu.cpp \
    dis7/SimulationAddress.cpp \
    dis7/SimulationIdentifier.cpp \
    dis7/SimulationManagementFamilyPdu.cpp \
    dis7/SimulationManagementPduHeader.cpp \
    dis7/SimulationManagementWithReliabilityFamilyPdu.cpp \
    dis7/StandardVariableSpecification.cpp \
    dis7/StartResumePdu.cpp \
    dis7/StartResumeReliablePdu.cpp \
    dis7/StopFreezePdu.cpp \
    dis7/StopFreezeReliablePdu.cpp \
    dis7/StorageFuel.cpp \
    dis7/StorageFuelReload.cpp \
    dis7/SupplyQuantity.cpp \
    dis7/SyntheticEnvironmentFamilyPdu.cpp \
    dis7/SystemIdentifier.cpp \
    dis7/TotalRecordSets.cpp \
    dis7/TrackJamData.cpp \
    dis7/TwoByteChunk.cpp \
    dis7/UAFundamentalParameter.cpp \
    dis7/UaPdu.cpp \
    dis7/UnattachedIdentifier.cpp \
    dis7/UnsignedDISInteger.cpp \
    dis7/VariableDatum.cpp \
    dis7/VariableParameter.cpp \
    dis7/VariableTransmitterParameters.cpp \
    dis7/Vector2Float.cpp \
    dis7/Vector3Double.cpp \
    dis7/Vector3Float.cpp \
    dis7/VectoringNozzleSystem.cpp \
    dis7/WarfareFamilyPdu.cpp \
    dis7/signalpdu.cpp \
    dis7/transferownershippdu.cpp \
    dis7/transmitterpdu.cpp \
    dis7/utils/ConversionUtils.cpp \
    dis7/utils/DataStream.cpp \
    main.cpp \
    mission/actions.cpp \
    mission/ifcondition.cpp \
    mission/inputs.cpp \
    mission/task.cpp \
    mission/taskgroup.cpp \
    # tests/applicationdialogtest/applicationdialog_test.cpp \
    # tests/canvaswidgettest/canvaswidget_test.cpp \
    # tests/consoleviewtest/consoleview_test.cpp \
    # tests/databaseeditortest/databaseeditor_test.cpp \
    # tests/databaseeditortest/test_databaseeditor.cpp \
    # tests/designtoolbartest/gui_designtoolbar_test.cpp \
    # tests/doctrineparameterstest/doctrineparameters_test.cpp \
    # tests/entityinfodialogtest/entityinfodialog_test.cpp \
    # tests/feedbacktest/feedback_test.cpp \
    # tests/graphwidgettest/graphwidget_test.cpp \
    # tests/gui_test_control.cpp \
    # tests/hierarchytreetest/addformationdialog_test.cpp \
    # tests/hierarchytreetest/additemdialog_test.cpp \
    # tests/hierarchytreetest/contextmenu_test.cpp \
    # tests/hierarchytreetest/gui_hierarchytree_test.cpp \
    # tests/inspectortest/inspectortest.cpp \
    # tests/layerpaneltest/layerpanel_test.cpp \
    # tests/measuredistancedialogtest/measuredistancedialog_test.cpp \
    # tests/menubartest/gui_menubar_test.cpp \
    # tests/menubartest/test_menubar1212.cpp \
    # tests/missioneditortest/gui_mission_test.cpp \
    # tests/navigationpagetest/navigationpage_test.cpp \
    # tests/panel/aesaradardisplay_test.cpp \
    # tests/panel/radardisplay_test.cpp \
    # tests/profileinfodialogtest/profileinfodialog_test.cpp \
    # tests/recentprojectsmanagertest/recentprojectsmanager_test.cpp \
    # tests/runtimeeditortest/runtimeeditor_test.cpp \
    # tests/runtimetoolbartest/gui_runtimetoolbar_test.cpp \
    # tests/scenarioeditor_test.cpp \
    # tests/sidebarwidgettest/sidebarwidget_test.cpp \
    # tests/statusbartest/statusbar_test.cpp \
    # tests/tacticaldisplaytest/tacticaldisplay_test.cpp \
    # tests/tacticalrulestest/tacticalrules_test.cpp \
    # tests/testscriptdialogtest/testscriptdialog_test.cpp \
    # tests/textscriptwidgettest/textscriptwidget_test.cpp \
 \    # tests/tooltiphelpertest/tooltiphelper_test.cpp
    sharedMemorylib/EntityHandler.cpp \
    sharedMemorylib/sharedMemory.cpp

HEADERS += \
    GIS_API/backend/api/airbase_api.h \
    GIS_API/backend/api/elevation_data.h \
    GIS_API/backend/api/layer_api.h \
    GIS_API/backend/api/map_api.h \
    GIS_API/backend/api/measure_api.h \
    GIS_API/backend/api/shape_api.h \
    GIS_API/backend/api/shape_history_api.h \
    GIS_API/backend/engine/gis_engine.h \
    GIS_API/backend/formats/builtin_layer_formats.h \
    GIS_API/backend/formats/geojson_simple.h \
    GIS_API/backend/formats/ilayer_format.h \
    GIS_API/backend/formats/layer_format_registry.h \
    GIS_API/backend/managers/layer_manager.h \
    GIS_API/backend/managers/shape_manager.h \
    GIS_API/backend/shapes/builtin_shape_types.h \
    GIS_API/backend/shapes/ishape_type.h \
    GIS_API/backend/shapes/shape_type_registry.h \
    GIS_API/common/constants.h \
    GIS_API/common/types.h \
    GIS_API/common/utils.h \
    GIS_API/frontend/dialogs/measure_distance_dialog.h \
    GIS_API/frontend/panels/api_layer_panel.h \
    GIS_API/frontend/panels/layer_widget.h \
    GIS_API/frontend/toolbars/api_map_toolbar.h \
    GIS_API/frontend/ui_action_api.h \
    GIS_API/frontend/widgets/map_widget.h \
    GUI/Canvas/canvas_action_api.h \
    GUI/Canvas/canvas_layer/dynamiccanvas.h \
    GUI/Canvas/canvas_layer/editorcanvas.h \
    GUI/Canvas/canvas_layer/staticcanvas.h \
    GUI/Canvas/context_menu/canvas_context_menu.h \
    GUI/Canvas/context_menu/entity_context_menu.h \
    GUI/Canvas/context_menu/entity_image_size_store.h \
    GUI/Canvas/context_menu/trajectory_color_store.h \
    GUI/Canvas/context_menu/trajectory_context_menu.h \
    GUI/Canvas/map_with_canvas.h \
    GUI/Canvas/overlay_canvas.h \
    GUI/Console/consoleview-styles.h \
    GUI/Console/consoleview.h \
    GUI/DOCTRINE/doctrine-styles.h \
    GUI/DOCTRINE/doctrineareadefinition.h \
    GUI/DOCTRINE/doctrineassumptionsnotes.h \
    GUI/DOCTRINE/doctrineparameters.h \
    GUI/DOCTRINE/tactical-styles.h \
    GUI/DOCTRINE/tacticalrules.h \
    GUI/Editors/RuntimeStatisticsData.h \
    GUI/Editors/RuntimeStatisticsDialog.h \
    GUI/Editors/StatisticsWidgets.h \
    GUI/Editors/analysiseditor.h \
    GUI/Editors/customresizableoverlaydock.h \
    GUI/Editors/database-styles.h \
    GUI/Editors/databaseeditor.h \
    GUI/Editors/missioneditor.h \
    GUI/Editors/recentprojectsmanager.h \
    GUI/Editors/reportseditor.h \
    GUI/Editors/runtimeeditor.h \
    GUI/Editors/scenarioeditor.h \
    GUI/Feedback/projectinformation-styles.h \
    GUI/Feedback/projectinformation.h \
    GUI/Hierarchytree/addformationdialog.h \
    GUI/Hierarchytree/additemdialog-styles.h \
    GUI/Hierarchytree/additemdialog/additemdialog.h \
    GUI/Hierarchytree/additemdialog/additemdialog_iff.h \
    GUI/Hierarchytree/additemdialog/additemdialog_radio.h \
    GUI/Hierarchytree/additemdialog/additemdialog_sensor.h \
    GUI/Hierarchytree/additemdialog/addweapondialog.h \
    GUI/Hierarchytree/contextmenu-styles.h \
    GUI/Hierarchytree/contextmenu.h \
    GUI/Hierarchytree/customtrajectorydialog.h \
    GUI/Hierarchytree/formationcanvaswidget.h \
    GUI/Hierarchytree/hierarchy-styles.h \
    GUI/Hierarchytree/hierarchyconnector.h \
    GUI/Hierarchytree/hierarchytree.h \
    GUI/Inspector/customparameterdialog.h \
    GUI/Inspector/customparameterdialog.h \
    GUI/Inspector/customparameterdialog.h \
    GUI/Inspector/customparameterdialog.h \
    GUI/Inspector/customparameterdialog.h \
    GUI/Inspector/inspector-styles.h \
    GUI/Inspector/inspector.h \
    GUI/Inspector/template/colortemplate.h \
    GUI/Inspector/template/geocordstemplate.h \
    GUI/Inspector/template/iconsdialog.h \
    GUI/Inspector/template/imagetemplate.h \
    GUI/Inspector/template/optiontemplate.h \
    GUI/Inspector/template/vectortemplate.h \
    GUI/Logger/loggerdialog.h \
    GUI/Menubars/menubar-styles.h \
    GUI/Menubars/menubar.h \
    GUI/Menubars/profileinfodialog-styles.h \
    GUI/Menubars/profileinfodialog.h \
    GUI/Navigation/navigationpage.h \
    GUI/Panel/EODisplayLib/eoland.h \
    GUI/Panel/EODisplayLib/eosky.h \
    GUI/Panel/EODisplayLib/eotargets.h \
    GUI/Panel/EODisplayLib/eovisionangle.h \
    GUI/Panel/EODisplayLib/eowether.h \
    GUI/Panel/IRDisplayLib/irland.h \
    GUI/Panel/IRDisplayLib/irsky.h \
    GUI/Panel/IRDisplayLib/irtargets.h \
    GUI/Panel/IRDisplayLib/irvisionangle.h \
    GUI/Panel/IRDisplayLib/irwether.h \
    GUI/Panel/adsbdisplay.h \
    GUI/Panel/aesaradardisplay.h \
    GUI/Panel/aisdisplay.h \
    GUI/Panel/csmdisplay.h \
    GUI/Panel/displayconstants.h \
    GUI/Panel/eodisplay.h \
    GUI/Panel/eoopengldisplay.h \
    GUI/Panel/esmdisplay.h \
    GUI/Panel/ewdisplay.h \
    GUI/Panel/iffdisplay.h \
    GUI/Panel/irdisplay.h \
    GUI/Panel/iropengldisplay.h \
    GUI/Panel/radardisplay.h \
    GUI/Panel/radiodisplay.h \
    GUI/Panel/sonardisplay.h \
    GUI/Panel/sonobuoypanel.h \
    GUI/Settings/applicationdialog-styles.h \
    GUI/Settings/applicationdialog.h \
    GUI/Sidebar/sidebar-styles.h \
    GUI/Sidebar/sidebarwidget.h \
    GUI/Tacticaldisplay/Airbase/airbase.h \
    GUI/Tacticaldisplay/Gis/custommapdialog.h \
    GUI/Tacticaldisplay/Gis/gislib.h \
    GUI/Tacticaldisplay/Gis/gisnetwork.h \
    GUI/Tacticaldisplay/Gis/layerpanel.h \
    GUI/Tacticaldisplay/Gis/transformmode.h \
    GUI/Tacticaldisplay/Layer/ILayerHost.h \
    GUI/Tacticaldisplay/Layer/csv.h \
    GUI/Tacticaldisplay/Layer/flatgeobuf.h \
    GUI/Tacticaldisplay/Layer/geojson.h \
    GUI/Tacticaldisplay/Layer/gml.h \
    GUI/Tacticaldisplay/Layer/kml.h \
    GUI/Tacticaldisplay/Layer/layerinformationdialog.h \
    GUI/Tacticaldisplay/Layer/layerpanel-styles.h \
    GUI/Tacticaldisplay/Layer/layerpanel.h \
    GUI/Tacticaldisplay/Layer/shapefile.h \
    GUI/Tacticaldisplay/Measurement/measurement.h \
    GUI/Tacticaldisplay/Shape/IShapeHost.h \
    GUI/Tacticaldisplay/Shape/mesh_entry.h \
    GUI/Tacticaldisplay/Shape/shapeeventhandler.h \
    GUI/Tacticaldisplay/Shape/shapes_feature.h \
    GUI/Tacticaldisplay/canvaswidget.h \
    GUI/Tacticaldisplay/contextmenu/contextmenu_handler.h \
    GUI/Tacticaldisplay/entityinfodialog.h \
    GUI/Tacticaldisplay/scalebar.h \
    GUI/Tacticaldisplay/tacticaldisplay.h \
    GUI/Tacticaldisplay/tooltiphelper.h \
    GUI/Tacticaldisplay/waypointeditdialog.h \
    GUI/Testscript/angelscripthighlighter.h \
    GUI/Testscript/testscriptdialog.h \
    GUI/Testscript/textscriptwidget.h \
    GUI/Theme/theme.h \
    GUI/Theme/theme_colors.h \
    GUI/Timing/graphwidget.h \
    GUI/Toolbars/designtoolbar-styles.h \
    GUI/Toolbars/designtoolbar.h \
    GUI/Toolbars/networktoolbar-styles.h \
    GUI/Toolbars/networktoolbar.h \
    GUI/Toolbars/runtimetoolbar-styles.h \
    GUI/Toolbars/runtimetoolbar.h \
    GUI/Toolbars/standardtoolbar.h \
    GUI/Toolbars/timetoolbar.h \
    GUI/dialogwindowutils.h \
    GUI/mainwindow-styles.h \
    GUI/mainwindow.h \
    GUI/measuredistance/measuredistancedialog-styles.h \
    GUI/measuredistance/measuredistancedialog.h \
    GUI/plugins/pluginmanagerdialog.h \
    GUI/scene3dwidget/scene3dwidget.h \
    GUI/splashwindow.h \
    GUI/statusbar.h \
    Setup.h \
    angelscript/add_on/scriptarray/scriptarray.h \
    angelscript/add_on/scriptstdstring/scriptstdstring.h \
    core/Components/attachedenitities.h \
    core/Components/collider.h \
    core/Components/component.h \
    core/Components/dynamicmodel.h \
    core/Components/mesh.h \
    core/Components/meshrenderer2d.h \
    core/Components/mission.h \
    core/Components/networkobject.h \
    core/Components/rigidbody.h \
    core/Components/trajectory.h \
    core/Components/transform.h \
    core/Config/scenarioconfig.h \
    core/Config/sessionmanager.h \
    core/DISPlugin/DISNetworkPlugin.h \
    core/DISPlugin/SimulationPlugin.h \
    core/DISPlugin/bridge/disnetworkbridge.h \
    core/DISPlugin/config/disconfigloader.h \
    core/DISPlugin/config/disreferencedataloader.h \
    core/DISPlugin/core/dismanager.h \
    core/DISPlugin/core/pdudispatcher.h \
    core/DISPlugin/core/pdusender.h \
    core/DISPlugin/core/pduserializer.h \
    core/DISPlugin/handlers/acknowledgehandler.h \
    core/DISPlugin/handlers/datahandler.h \
    core/DISPlugin/handlers/dataqueryhandler.h \
    core/DISPlugin/handlers/designatorhandler.h \
    core/DISPlugin/handlers/emissionhandler.h \
    core/DISPlugin/handlers/entitystatehandler.h \
    core/DISPlugin/handlers/exercisehandler.h \
    core/DISPlugin/handlers/iffhandler.h \
    core/DISPlugin/handlers/setdatahandler.h \
    core/DISPlugin/handlers/signalhandler.h \
    core/DISPlugin/handlers/transferownershiphandler.h \
    core/DISPlugin/handlers/transmitterhandler.h \
    core/DISPlugin/handlers/warfarehandlers.h \
    core/DISPlugin/interface/DISConfig.h \
    core/DISPlugin/interface/DISDatumSnapshot.h \
    core/DISPlugin/interface/DISDesignatorSnapshot.h \
    core/DISPlugin/interface/DISDetonationSnapshot.h \
    core/DISPlugin/interface/DISEmissionSnapshot.h \
    core/DISPlugin/interface/DISEntitySnapshot.h \
    core/DISPlugin/interface/DISExerciseControl.h \
    core/DISPlugin/interface/DISFireSnapshot.h \
    core/DISPlugin/interface/DISIFFSnapshot.h \
    core/DISPlugin/interface/DISIncomingDesignator.h \
    core/DISPlugin/interface/DISIncomingSignal.h \
    core/DISPlugin/interface/DISIncomingTransmitter.h \
    core/DISPlugin/interface/DISReferenceData.h \
    core/DISPlugin/interface/DISSignalSnapshot.h \
    core/DISPlugin/interface/DISTransmitterSnapshot.h \
    core/DISPlugin/interface/disincomingdata.h \
    core/DISPlugin/transport/DISTransport.h \
    core/DISPlugin/utils/coordconverter.h \
    core/DISPlugin/utils/deadreckoning.h \
    core/DISPlugin/utils/disdatumregistry.h \
    core/DISPlugin/utils/entityidmapper.h \
    core/DISPlugin/version/pduversionadapter.h \
    core/Debug/AppException.h \
    core/Debug/ExceptionMacros.h \
    core/Debug/console.h \
    core/Debug/frame.h \
    core/Debug/profiler.h \
    core/Dynamics/Model/aircraft.h \
    core/Dynamics/Model/seamodel.h \
    core/Dynamics/dynamics.h \
    core/GlobalRegistry.h \
    core/Hierarchy/Components/attachedenitities.h \
    core/Hierarchy/Components/collider.h \
    core/Hierarchy/Components/component.h \
    core/Hierarchy/Components/crosssection.h \
    core/Hierarchy/Components/distypeprofile.h \
    core/Hierarchy/Components/dynamicmodel.h \
    core/Hierarchy/Components/iffprofile.h \
    core/Hierarchy/Components/mesh.h \
    core/Hierarchy/Components/meshrenderer2d.h \
    core/Hierarchy/Components/mission.h \
    core/Hierarchy/Components/networkobject.h \
    core/Hierarchy/Components/radioprofile.h \
    core/Hierarchy/Components/rigidbody.h \
    core/Hierarchy/Components/sensorprofile.h \
    core/Hierarchy/Components/trajectory.h \
    core/Hierarchy/Components/transform.h \
    core/Hierarchy/Components/weaponprofile.h \
    core/Hierarchy/ECS/Struct/Meta_Data_Component.h \
    core/Hierarchy/ECS/Struct/collider_component.h \
    core/Hierarchy/ECS/Struct/dynamic_component.h \
    core/Hierarchy/ECS/Struct/environment_component.h \
    core/Hierarchy/ECS/Struct/mesh_component.h \
    core/Hierarchy/ECS/Struct/radar_component.h \
    core/Hierarchy/ECS/Struct/radio_component.h \
    core/Hierarchy/ECS/Struct/sensor_component.h \
    core/Hierarchy/ECS/Struct/trajectory_component.h \
    core/Hierarchy/ECS/Struct/transform_component.h \
    core/Hierarchy/ECS/System/DynamicSystem.h \
    core/Hierarchy/ECS/World/world.h \
    core/Hierarchy/EntityProfiles/IFFs/customiff.h \
    core/Hierarchy/EntityProfiles/Radio/customradio.h \
    core/Hierarchy/EntityProfiles/Radio/include/radio/propagation_model_config.h \
    core/Hierarchy/EntityProfiles/Radio/include/radio/radio_config.h \
    core/Hierarchy/EntityProfiles/Radio/include/radio/radio_encoders.h \
    core/Hierarchy/EntityProfiles/Radio/include/radio/radio_interface.h \
    core/Hierarchy/EntityProfiles/Radio/include/radio/radio_types.h \
    core/Hierarchy/EntityProfiles/Radio/src/propagation_model_impl.h \
    core/Hierarchy/EntityProfiles/Radio/src/radio_impl.h \
    core/Hierarchy/EntityProfiles/Radio/src/utils/math_utils.h \
    core/Hierarchy/EntityProfiles/SensorProfiles/AIS_ADSB/ADSB/ADSB.h \
    core/Hierarchy/EntityProfiles/SensorProfiles/AIS_ADSB/AIS/AIS.h \
    core/Hierarchy/EntityProfiles/SensorProfiles/AIS_ADSB/RF/RF.h \
    core/Hierarchy/EntityProfiles/SensorProfiles/CSM/csmdetector.h \
    core/Hierarchy/EntityProfiles/SensorProfiles/CSM/csmfrequency.h \
    core/Hierarchy/EntityProfiles/SensorProfiles/CSM/csmmonitor.h \
    core/Hierarchy/EntityProfiles/SensorProfiles/CSM/csmsignalmodel.h \
    core/Hierarchy/EntityProfiles/SensorProfiles/CSM/csmspectrum.h \
    core/Hierarchy/EntityProfiles/SensorProfiles/CSM/csmtypes.h \
    core/Hierarchy/EntityProfiles/SensorProfiles/EOIRSensorLib/EOIRCommonLib/environmental_conditions.h \
    core/Hierarchy/EntityProfiles/SensorProfiles/EOIRSensorLib/EOSensorLib/eosensorlib.h \
    core/Hierarchy/EntityProfiles/SensorProfiles/EOIRSensorLib/IRSensorLib/lwir.h \
    core/Hierarchy/EntityProfiles/SensorProfiles/EOIRSensorLib/IRSensorLib/mwir.h \
    core/Hierarchy/EntityProfiles/SensorProfiles/EOIRSensorLib/IRSensorLib/stanag_4347.h \
    core/Hierarchy/EntityProfiles/SensorProfiles/ESM/esmemitters.h \
    core/Hierarchy/EntityProfiles/SensorProfiles/ESM/esmobservationmodel.h \
    core/Hierarchy/EntityProfiles/SensorProfiles/ESM/esmppiwidget.h \
    core/Hierarchy/EntityProfiles/SensorProfiles/ESM/esmspectrumwidget.h \
    core/Hierarchy/EntityProfiles/SensorProfiles/ESM/esmrf.h \
    core/Hierarchy/EntityProfiles/SensorProfiles/adsbsensor.h \
    core/Hierarchy/EntityProfiles/SensorProfiles/aesaradar.h \
    core/Hierarchy/EntityProfiles/SensorProfiles/aisaRadarLib/radarantenna_aesa.h \
    core/Hierarchy/EntityProfiles/SensorProfiles/aisaRadarLib/radarmodel_aesa.h \
    core/Hierarchy/EntityProfiles/SensorProfiles/aisaRadarLib/radarscheduler.h \
    core/Hierarchy/EntityProfiles/SensorProfiles/aisaRadarLib/radarsignallibrary_aesa.h \
    core/Hierarchy/EntityProfiles/SensorProfiles/aisaRadarLib/radarsignalprocessor_aesa.h \
    core/Hierarchy/EntityProfiles/SensorProfiles/aisaRadarLib/radartracker_aesa.h \
    core/Hierarchy/EntityProfiles/SensorProfiles/aisaRadarLib/test-aesa/issst_test_framework.h \
    core/Hierarchy/EntityProfiles/SensorProfiles/aissensor.h \
    core/Hierarchy/EntityProfiles/SensorProfiles/csm.h \
    core/Hierarchy/EntityProfiles/SensorProfiles/customsensor.h \
    core/Hierarchy/EntityProfiles/SensorProfiles/eoSensorLib/eo.h \
    core/Hierarchy/EntityProfiles/SensorProfiles/eoSensorLib/eo_ir.h \
    core/Hierarchy/EntityProfiles/SensorProfiles/eoSensorLib/eovision.h \
    core/Hierarchy/EntityProfiles/SensorProfiles/eoSensorLib/georelativeangle.h \
    core/Hierarchy/EntityProfiles/SensorProfiles/eoSensorLib/georelativeelevation.h \
    core/Hierarchy/EntityProfiles/SensorProfiles/eoSensorLib/payload.h \
    core/Hierarchy/EntityProfiles/SensorProfiles/EOIRSensorLib/EOIRCommonLib/eoircommonlib.h \
    core/Hierarchy/EntityProfiles/SensorProfiles/EOIRSensorLib/IRSensorLib/irsensorlib.h \
    core/Hierarchy/EntityProfiles/SensorProfiles/eosensor.h \
    core/Hierarchy/EntityProfiles/SensorProfiles/esm.h \
    core/Hierarchy/EntityProfiles/SensorProfiles/irsensor.h \
    core/Hierarchy/EntityProfiles/SensorProfiles/radar.h \
    # core/Hierarchy/EntityProfiles/SensorProfiles/radarmodel.h \
    core/Hierarchy/EntityProfiles/SensorProfiles/radarlib/jammingconfig.h \
    core/Hierarchy/EntityProfiles/SensorProfiles/radarlib/radarantenna.h \
    core/Hierarchy/EntityProfiles/SensorProfiles/radarlib/radarmodel.h \
    core/Hierarchy/EntityProfiles/SensorProfiles/radarlib/radarsignallibrary.h \
    core/Hierarchy/EntityProfiles/SensorProfiles/radarlib/radarsignalprocessor.h \
    core/Hierarchy/EntityProfiles/SensorProfiles/radarlib/radartracker.h \
    core/Hierarchy/EntityProfiles/SensorProfiles/sonar.h \
    core/Hierarchy/EntityProfiles/SensorProfiles/sonar/active_sonar.h \
    core/Hierarchy/EntityProfiles/SensorProfiles/sonar/sonar_model.h \
    core/Hierarchy/EntityProfiles/fixedpoints.h \
    core/Hierarchy/EntityProfiles/formation.h \
    core/Hierarchy/EntityProfiles/iff.h \
    core/Hierarchy/EntityProfiles/platform.h \
    core/Hierarchy/EntityProfiles/radio.h \
    core/Hierarchy/EntityProfiles/sensor.h \
    core/Hierarchy/EntityProfiles/specialzone.h \
    core/Hierarchy/EntityProfiles/weapon.h \
    core/Hierarchy/EntityProfiles/weapons/artillery.h \
    core/Hierarchy/EntityProfiles/weapons/bomb.h \
    core/Hierarchy/EntityProfiles/weapons/chaff.h \
    core/Hierarchy/EntityProfiles/weapons/flare.h \
    core/Hierarchy/EntityProfiles/weapons/missile.h \
    core/Hierarchy/EntityProfiles/weapons/rocket.h \
    core/Hierarchy/EntityProfiles/weapons/sonobuoy.h \
    core/Hierarchy/EntityProfiles/weapons/torpedo.h \
    core/Hierarchy/Struct/action.h \
    core/Hierarchy/Struct/color.h \
    core/Hierarchy/Struct/condition.h \
    core/Hierarchy/Struct/constants.h \
    core/Hierarchy/Struct/formationposition.h \
    core/Hierarchy/Struct/geocords.h \
    core/Hierarchy/Struct/parameter.h \
    core/Hierarchy/Struct/vector.h \
    core/Hierarchy/Struct/waypoints.h \
    core/Hierarchy/Utils/entityutils.h \
    core/Hierarchy/entity.h \
    core/Hierarchy/folder.h \
    core/Hierarchy/hierarchy.h \
    core/Hierarchy/profilecategaory.h \
    core/InputSystem/inputmanager.h \
    core/Network/entitypdu.h \
    core/Network/folderpdu.h \
    core/Network/libs/MessageBus.h \
    core/Network/libs/TransformUpdate.h \
    core/Network/libs/lz4.h \
    core/Network/networkmanager.h \
    core/Network/networktransport.h \
    core/Network/profilepdu.h \
    core/Network/transformpdu.h \
    core/Plugins/Interfaces/DynamicInterface.h \
    core/Plugins/Interfaces/HierarchyInterface.h \
    core/Plugins/Interfaces/aesaRadarInterface.h \
    core/Plugins/Interfaces/customIFFInterface.h \
    core/Plugins/Interfaces/customRadioInterface.h \
    core/Plugins/Interfaces/customSensorInterface.h \
    core/Plugins/Interfaces/uigisinterface.h \
    core/Plugins/Interfaces/uitoolbarinterface.h \
    core/Plugins/aesalogicregistry.h \
    core/Plugins/pluginmanager.h \
    core/Plugins/uipluginmanager.h \
    core/Recorder/payload.h \
    core/Recorder/recorder.h \
    core/Render/scenerenderer.h \
    core/SQLite/sqlite.h \
    core/ScriptEngine/AutoScript/autoscriptconfig.h \
    core/ScriptEngine/AutoScript/autoscriptcontroller.h \
    core/ScriptEngine/AutoScript/autoscriptmanager.h \
    core/ScriptEngine/AutoScript/autoscriptsource.h \
    core/ScriptEngine/AutoScript/tcpautoscriptsource.h \
    core/ScriptEngine/scriptengine.h \
    core/ScriptEngine/scriptenginegis.h \
    core/SharedMemory/sharedmemorywrapper.h \
    core/Simulation/simulation.h \
    core/Simulation/simulation_state.h \
    core/Utility/property.h \
    core/Utility/uuid.h \
    core/core_test.h \
    core/structure/database.h \
    core/structure/entity.h \
    core/structure/folder.h \
    core/structure/hierarchy.h \
    core/structure/missioncreator.h \
    core/structure/missionexcuter.h \
    core/structure/runtime.h \
    core/structure/scenario.h \
    dis7/AcknowledgePdu.h \
    dis7/AcknowledgeReliablePdu.h \
    dis7/AcousticEmitter.h \
    dis7/ActionRequestPdu.h \
    dis7/ActionRequestReliablePdu.h \
    dis7/ActionResponsePdu.h \
    dis7/ActionResponseReliablePdu.h \
    dis7/AggregateIdentifier.h \
    dis7/AggregateMarking.h \
    dis7/AggregateType.h \
    dis7/AngleDeception.h \
    dis7/AngularVelocityVector.h \
    dis7/AntennaLocation.h \
    dis7/ArealObjectStatePdu.h \
    dis7/ArticulatedParts.h \
    dis7/Association.h \
    dis7/AttachedParts.h \
    dis7/Attribute.h \
    dis7/AttributePdu.h \
    dis7/BeamAntennaPattern.h \
    dis7/BeamData.h \
    dis7/BeamStatus.h \
    dis7/BlankingSector.h \
    dis7/ClockTime.h \
    dis7/CollisionElasticPdu.h \
    dis7/CollisionPdu.h \
    dis7/CommentPdu.h \
    dis7/CommentReliablePdu.h \
    dis7/CommunicationsNodeID.h \
    dis7/CreateEntityPdu.h \
    dis7/CreateEntityReliablePdu.h \
    dis7/DataPdu.h \
    dis7/DataQueryDatumSpecification.h \
    dis7/DataQueryPdu.h \
    dis7/DataQueryReliablePdu.h \
    dis7/DataReliablePdu.h \
    dis7/DatumSpecification.h \
    dis7/DeadReckoningParameters.h \
    dis7/DesignatorPdu.h \
    dis7/DetonationPdu.h \
    dis7/DirectedEnergyAreaAimpoint.h \
    dis7/DirectedEnergyDamage.h \
    dis7/DirectedEnergyFirePdu.h \
    dis7/DirectedEnergyPrecisionAimpoint.h \
    dis7/DirectedEnergyTargetEnergyDeposition.h \
    dis7/DistributedEmissionsFamilyPdu.h \
    dis7/EEFundamentalParameterData.h \
    dis7/EightByteChunk.h \
    dis7/ElectromagneticEmissionBeamData.h \
    dis7/ElectromagneticEmissionSystemData.h \
    dis7/ElectromagneticEmissionsPdu.h \
    dis7/EmitterSystem.h \
    dis7/EngineFuel.h \
    dis7/EngineFuelReload.h \
    dis7/EntityAssociation.h \
    dis7/EntityDamageStatusPdu.h \
    dis7/EntityID.h \
    dis7/EntityInformationFamilyPdu.h \
    dis7/EntityManagementFamilyPdu.h \
    dis7/EntityMarking.h \
    dis7/EntityStatePdu.h \
    dis7/EntityStateUpdatePdu.h \
    dis7/EntityType.h \
    dis7/EntityTypeVP.h \
    dis7/Environment.h \
    dis7/EnvironmentGeneral.h \
    dis7/EnvironmentType.h \
    dis7/EulerAngles.h \
    dis7/EventIdentifier.h \
    dis7/EventIdentifierLiveEntity.h \
    dis7/EventReportPdu.h \
    dis7/EventReportReliablePdu.h \
    dis7/Expendable.h \
    dis7/ExpendableDescriptor.h \
    dis7/ExpendableReload.h \
    dis7/ExplosionDescriptor.h \
    dis7/FalseTargetsAttribute.h \
    dis7/FastEntityStatePdu.h \
    dis7/FirePdu.h \
    dis7/FixedDatum.h \
    dis7/FourByteChunk.h \
    dis7/FundamentalOperationalData.h \
    dis7/GridAxis.h \
    dis7/GroupIdentifier.h \
    dis7/IFFFundamentalParameterData.h \
    dis7/IFFPdu.h \
    dis7/IOCommunicationsNode.h \
    dis7/IOEffect.h \
    dis7/IffDataSpecification.h \
    dis7/IntercomCommunicationsParameters.h \
    dis7/IntercomIdentifier.h \
    dis7/IntercomSignalPdu.h \
    dis7/IsPartOfPdu.h \
    dis7/JammingTechnique.h \
    dis7/LaunchedMunitionRecord.h \
    dis7/LayerHeader.h \
    dis7/LinearObjectStatePdu.h \
    dis7/LinearSegmentParameter.h \
    dis7/LiveEntityIdentifier.h \
    dis7/LiveEntityPdu.h \
    dis7/LiveSimulationAddress.h \
    dis7/LogisticsFamilyPdu.h \
    dis7/MineEntityIdentifier.h \
    dis7/MinefieldFamilyPdu.h \
    dis7/MinefieldIdentifier.h \
    dis7/MinefieldResponseNackPdu.h \
    dis7/MinefieldStatePdu.h \
    dis7/ModulationParameters.h \
    dis7/ModulationType.h \
    dis7/Munition.h \
    dis7/MunitionDescriptor.h \
    dis7/MunitionReload.h \
    dis7/NamedLocationIdentification.h \
    dis7/ObjectIdentifier.h \
    dis7/ObjectType.h \
    dis7/OwnershipStatus.h \
    dis7/Pdu.h \
    dis7/PduContainer.h \
    dis7/PduHeader.h \
    dis7/PduStatus.h \
    dis7/PduSuperclass.h \
    dis7/PointObjectStatePdu.h \
    dis7/PropulsionSystemData.h \
    dis7/RadioCommunicationsFamilyPdu.h \
    dis7/RadioIdentifier.h \
    dis7/RadioType.h \
    dis7/ReceiverPdu.h \
    dis7/RecordQueryReliablePdu.h \
    dis7/RecordQuerySpecification.h \
    dis7/RecordSpecification.h \
    dis7/RecordSpecificationElement.h \
    dis7/Relationship.h \
    dis7/RemoveEntityPdu.h \
    dis7/RemoveEntityReliablePdu.h \
    dis7/RepairCompletePdu.h \
    dis7/RepairResponsePdu.h \
    dis7/ResupplyOfferPdu.h \
    dis7/ResupplyReceivedPdu.h \
    dis7/SecondaryOperationalData.h \
    dis7/SeesPdu.h \
    dis7/Sensor.h \
    dis7/SeparationVP.h \
    dis7/ServiceRequestPdu.h \
    dis7/SetDataPdu.h \
    dis7/SetDataReliablePdu.h \
    dis7/SimulationAddress.h \
    dis7/SimulationIdentifier.h \
    dis7/SimulationManagementFamilyPdu.h \
    dis7/SimulationManagementPduHeader.h \
    dis7/SimulationManagementWithReliabilityFamilyPdu.h \
    dis7/StandardVariableSpecification.h \
    dis7/StartResumePdu.h \
    dis7/StartResumeReliablePdu.h \
    dis7/StopFreezePdu.h \
    dis7/StopFreezeReliablePdu.h \
    dis7/StorageFuel.h \
    dis7/StorageFuelReload.h \
    dis7/SupplyQuantity.h \
    dis7/SyntheticEnvironmentFamilyPdu.h \
    dis7/SystemIdentifier.h \
    dis7/TotalRecordSets.h \
    dis7/TrackJamData.h \
    dis7/TwoByteChunk.h \
    dis7/UAFundamentalParameter.h \
    dis7/UaPdu.h \
    dis7/UnattachedIdentifier.h \
    dis7/UnsignedDISInteger.h \
    dis7/VariableDatum.h \
    dis7/VariableParameter.h \
    dis7/VariableTransmitterParameters.h \
    dis7/Vector2Float.h \
    dis7/Vector3Double.h \
    dis7/Vector3Float.h \
    dis7/VectoringNozzleSystem.h \
    dis7/WarfareFamilyPdu.h \
    dis7/opendis7_export.h \
    dis7/signalpdu.h \
    dis7/transferownershippdu.h \
    dis7/transmitterpdu.h \
    dis7/utils/ConversionUtils.h \
    dis7/utils/DataStream.h \
    dis7/utils/Endian.h \
    dis7/utils/PDUType.h \
    mission/actions.h \
    mission/ifcondition.h \
    mission/inputs.h \
    mission/task.h \
    mission/taskgroup.h \
    # tests/applicationdialogtest/applicationdialog_test.h \
    # tests/consoleviewtest/consoleview_test.h \
    # tests/databaseeditortest/databaseeditor_test.h \
    # tests/databaseeditortest/test_databaseeditor.h \
    # tests/designtoolbartest/gui_designtoolbar_test.h \
    # tests/doctrineparameterstest/doctrineparameters_test.h \
    # tests/entityinfodialogtest/entityinfodialog_test.h \
    # tests/feedbacktest/feedback_test.h \
    # tests/graphwidgettest/graphwidget_test.h \
    # tests/gui_test_control.h \
    # tests/hierarchytreetest/addformationdialog_test.h \
    # tests/hierarchytreetest/additemdialog_test.h \
    # tests/hierarchytreetest/contextmenu_test.h \
    # tests/hierarchytreetest/gui_hierarchytree_test.h \
    # tests/inspectortest/inspectortest.h \
    # tests/layerpaneltest/layerpanel_test.h \
    # tests/measuredistancedialogtest/measuredistancedialog_test.h \
    # tests/menubartest/gui_menubar_test.h \
    # tests/menubartest/test_menubar1212.h \
    # tests/missioneditortest/gui_mission_test.h \
    # tests/navigationpagetest/navigationpage_test.h \
    # tests/profileinfodialogtest/profileinfodialog_test.h \
    # tests/runtimeeditortest/runtimeeditor_test.h \
    # tests/runtimetoolbartest/gui_runtimetoolbar_test.h \
    # tests/scenarioeditor_test.h \
    # tests/sidebarwidgettest/sidebarwidget_test.h \
    # tests/tacticalrulestest/tacticalrules_test.h \
    # tests/testscriptdialogtest/testscriptdialog_test.h \
 \    # tests/textscriptwidgettest/textscriptwidget_test.h
    sharedMemorylib/EntityHandler.h \
    sharedMemorylib/sharedMemory.h \
    sharedMemorylib/struct.h

win32: LIBS += -lws2_32
win32: LIBS += -lpsapi

FORMS += \
    mission/actions.ui \
    mission/ifcondition.ui \
    mission/inputs.ui \
    mission/task.ui \
    mission/taskgroup.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Resources.qrc \
    Weapon_Resource.qrc

DISTFILES += \
    GUI/GUI/Editors/runtimeeditor.zip \
    GUI/GUI/Panel.zip \
    Testscript/dghy.as \
    Testscript/er23re2.as \
    core/Hierarchy/EntityProfiles/SensorProfiles/EOIRSensorLib/EOIRTest/CMakeLists.txt \
    dis7/CMakeLists.txt \
    dis7/utils/CMakeLists.txt \
    tests/tst_ss.qml \
    tests/tst_testmenubar.qml

# win32:CONFIG(release, debug|release): LIBS += -L$$PWD/'../../Desktop/Cpp Projects/sharedMemory/release/' -lSharedMemoryLib
# else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/'../../Desktop/Cpp Projects/sharedMemory/debug/' -lSharedMemoryLib
# else:unix: LIBS += -L$$PWD/'../../Desktop/Cpp Projects/sharedMemory/' -lSharedMemoryLib

# INCLUDEPATH += $$PWD/'../../Desktop/Cpp Projects/sharedMemory'
# DEPENDPATH += $$PWD/'../../Desktop/Cpp Projects/sharedMemory'

# win32:CONFIG(release, debug|release): LIBS += -L$$PWD/sharedMemorylib/release/ -lSharedMemoryLib
# else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/sharedMemorylib/debug/ -lSharedMemoryLib
# else:unix: LIBS += -L$$PWD/sharedMemorylib/ -lSharedMemoryLib

unix: LIBS += -lcrypto -lssl
unix {
    QMAKE_LFLAGS += -rdynamic
    QMAKE_LFLAGS += -Wl,--export-dynamic
}
# INCLUDEPATH += $$PWD/sharedMemorylib
# DEPENDPATH += $$PWD/sharedMemorylib

SUBDIRS += \
    tests/TDF.pro

# MenuBar GUI tests
SOURCES +=
HEADERS +=
INCLUDEPATH += $$PWD/GUI
