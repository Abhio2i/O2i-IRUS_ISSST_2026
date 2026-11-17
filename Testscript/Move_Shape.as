void main(ScriptEngine@ e)
{
    // Set the active city to Bhopal.
    e.useCity("Bhopal");

    // Create a circle named "Circle" with a radius of 10,000 units.
    e.AddCircle("Circle", 10000.0);

    // Move the circle to the specified latitude and longitude.
    e.moveShape("Circle", 22.7375, 75.8481);

    // Generate a PDF report containing the shape movement information.
    e.generatePDFReport("shape_move_report.pdf");
}
