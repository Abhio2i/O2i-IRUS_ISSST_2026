void main(ScriptEngine@ e)
{
    // Enable Airbase Layer
    e.ToggleAirbases(True);

    // Generate Auto Report
    e.generatePDFReport("Airbase_India_Report.pdf");
}
