void main(ScriptEngine@ e)
{
    // Set the measurement unit to kilometers.
    e.SetMeasurementUnit("kilometers");

    // Start a new distance measurement session.
    e.StartDistanceMeasurement();

    // Add the first measurement point using latitude and longitude.
    e.AddMeasurePoint(24.6639, 77.2090);

    // Add the second measurement point using latitude and longitude.
    e.AddMeasurePoint(23.2599, 77.4126);

    // Calculate and retrieve the total distance between the measurement points.
    double d = e.GetTotalDistance();

    // Generate a PDF report containing the measurement results.
    e.generatePDFReport("measurement_report.pdf");
}
