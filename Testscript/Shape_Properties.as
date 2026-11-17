void main(ScriptEngine@ e)
{
    // Set the active city to Bhopal.
    e.useCity("Bhopal");

    // Create a rectangle named "Block" with a width of 10,000 units
    // and a height of 6,000 units.
    e.AddRectangle("Block", 10000, 6000);

    // Pause script execution for 500 milliseconds.
    e.sleep(500);

    // Set the rectangle's border color to green using RGB values (0, 255, 0).
    // Set the border thickness to 4 units.
    e.addShapeProperties(
        "Block",
        0,    // Red component
        255,  // Green component
        0,    // Blue component
        4     // Border thickness
    );

    // Generate a PDF report containing the GIS shape properties.
    e.generatePDFReport("GIS_Report_Properties.pdf");
}
