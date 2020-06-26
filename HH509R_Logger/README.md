## Data Logging Program for HH509R Thermocouple Reader

HH509R\_Logger.exe (in the Release\ directory) is a command line program that records temperature data from an Omega Engineering HH509R datalogging thermocouple reader and saves this data to a .xlsx workbook file. Temperature data is also printed live to the screen while the program is running.

To use this program, run

```PS > .\HH509_Logger.exe COM1 output_file.xlsx```

from PowerShell or the command line. COM1 stands in for the COM port to which the HH509R is connected.

The program ceases collecting data and saves its output .xlsx file under two conditions:

1. The user keys Ctrl+C in the PowerShell or command line window.
2. The program receives no data from the HH509R for 30 seconds.

See Release\example\_data.xlsx for an example output file.
