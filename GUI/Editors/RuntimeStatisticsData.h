#pragma once
#include <QString>
#include <QMetaType>
#include <QVector>

struct EntityDistribution {
    int air = 0, ground = 0, sea = 0, staticEntities = 0, other = 0;
};
struct OperationalStatus {
    int moving = 0, stationary = 0, airborne = 0, submerged = 0;
    int damaged = 0, healthy = 0;
};
struct SensorStatistics {
    int total = 0, active = 0, radar = 0, sonar = 0;
    int activeTracks = 0, totalDetections = 0;
};
struct RuntimeStatisticsData {
    QString scenario = "-";
    QString status = "Stopped";
    QString simulationTime = "00:00:00.000";
    double simulationRate = 1.0;
    int totalEntities = 0, activeEntities = 0, destroyedEntities = 0, inactiveEntities = 0;
    EntityDistribution distribution;
    OperationalStatus operational;
    SensorStatistics sensors;
    // Set sampleTimeSeconds to elapsed SIMULATION time for history graphs.
    double sampleTimeSeconds = 0.0;
    // Tick duration is optional; negative means unavailable.
    double tickDurationMs = -1.0;
};
Q_DECLARE_METATYPE(RuntimeStatisticsData)
