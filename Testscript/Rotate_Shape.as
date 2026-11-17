void main(ScriptEngine@ e)
{
    // Set the active city to Bhopal.
    e.useCity("Bhopal");

    // Create a rectangle named "Grid_Block" with a width of 40,000 units
    // and a height of 30,000 units.
    e.AddRectangle(
        "Grid_Block",
        40000.0,
        30000.0
    );

    // Pause script execution for 3 seconds.
    e.sleep(3000);

    // Rotate the rectangle by 45 degrees.
    e.rotateShape(
        "Grid_Block",
        45.0
    );

    // Generate a PDF report containing the GIS shape rotation information.
    e.generatePDFReport(
        "GIS_Report_rotate.pdf"
    );
}
