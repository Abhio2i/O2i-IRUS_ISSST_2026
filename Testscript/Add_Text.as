void main(ScriptEngine@ e)
{
    // Set the active city to Bhopal.
    e.useCity("Bhopal");

    // Add a text label named "Bhopal HQ" at the specified latitude and longitude.
    e.addText("Bhopal HQ", 23.278665, 77.369882);

    // Pause script execution for 1000 milliseconds (1 second).
    e.sleep(1000);

    // Generate a PDF report containing the GIS text information.
    e.generatePDFReport("GIS_Report_Text.pdf");
}
