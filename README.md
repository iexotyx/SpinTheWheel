Simple program written to spin a physics-based wheel and randomly select a value*. Select latest release to download the compiled .exe, tested on Windows only.

Please raise an issue at https://github.com/iexotyx/SpinTheWheel/ for fixes/improvements.

*While the randomness of the program has been tested, it is not suggested that this program be used to choose a name for a child, make important life decisions, etc...

Instructions:
-------------
1. (optional) Move the WheelApp.exe file somewhere. The save/load wheel functionality will create a "Wheels" and a "Trees" folder in whichever directory the [.exe] sits in to store saved files. Create a shortcut to the [.exe] if you prefer.

   1.1. I suggest:
   
           "Wheel Spinner"
             └─ WheelApp.exe
   
   1.2. The program will then create the required folders and files as you save wheels:
   
           "Wheel Spinner"
             ├─ WheelApp.exe
             ├─ Wheels
             |    ├─ "Child Names".csv
             |    ├─ "Job Wheel".csv
             |    └─ "etc".csv
             └─ Trees
                  └─ "Coming Soon".csv
   
2. Open the [.exe].
3. Click the menu button and select create/edit wheel.
4. Edit the segment labels, colours, and weights as desired.

    4.1. Don't worry too much about calculating the weights so they all add up to 1 if you don't want to; the program will normalise them when the wheel is saved so you only need to keep them proportionally correct.
5. Press [S] to save the wheel to the main view and [SPACE] to spin the wheel.
   or
6. Press [CTRL+S] to save the wheel to a file for later use, the program will save the wheel files to the Wheels folder as shown in 1.2.
7. Click the menu button and select Load Wheel.
8. Select a saved wheel and you can load it to the main wheel or delete it.
9. Once a loaded wheel is in the main wheel, you can go to create/edit wheel and further edit the wheel.

    9.1 After loading a wheel, if you press [CTRL+S] without renaming the wheel, it will save over the top of the loaded wheel.
    
    9.2 If you press [CTRL+S] after renaming the wheel, it will save as a new wheel.
