## Data Logging Program for HH509R Thermocouple Reader

HH509R\_Logger.exe (in the Release\ directory) is a command line program that records temperature data from an Omega Engineering HH509R datalogging thermocouple reader and saves this data to a .xlsx workbook file. Temperature data is also printed live to the screen while the program is running.

To use this program, run

```PS > .\HH509_Logger.exe COM1 output_file.xlsx```

from PowerShell or the command line. COM1 stands in for the COM port to which the HH509R is connected. The thermocouple reader should be powered on before running HH509\_Logger.exe.

The program ceases collecting data and saves its output .xlsx file under four conditions:

1. The user keys Ctrl+C in the PowerShell or command line window.
2. The program receives no data of any kind from the HH509R for 60 seconds.
3. The program receives no valid data from the HH509R for 300 seconds.
4. The amount of collected data has reached the row limit of a .xlsx workbook file.

See Release\example\_data.xlsx for an example output file.
