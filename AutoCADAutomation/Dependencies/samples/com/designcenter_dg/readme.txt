
This sample shows how to add custom content to the AutoCAD DesignCenter. In this example, TXT files are added as content. 

1. Build the application. Note that you will get a regsrv32 warning. This is an ObjectARX application and cannot be registered completely without AutoCAD present. This warning can be removed from the project settings by choosing the Custom Build tab in the Project Settings dialog. Remove the text in the Commands window and this operation will not be attempted in the future for that project and the warning will go away. The setting is generated by the ATL COM Wizard and has no negative affect on the application. Since it is automatically generated, you will likely see the same warning with projects that you create, especially following the instructions from the ObjectARX Developer's Guide. For this reason, it was left in for the sample to demonstrate that it has no affect on the application and can be safely ignored.
   
2. Load the application into AutoCAD. By loading the application, the code will be executed at runtime to register the server. Once registered, the application will automatically be loaded by the AutoCAD DesignCenter when necessary.
   
3. Start the AutoCAD DesignCenter (command: ADC). If the AutoCAD DesignCenter is already running, you should quit and start it again for the content to be recognized.
   
4. Make sure the description window is open in AutoCAD DesignCenter. This sample will read the contents of text files and display the text in the description window. 
   
5. Browse to a directory that contains a .TXT file. Any .TXT file should now show up in the explorer as valid content. Drag and Drop is NOT supported by this sample, but by double clicking the file, the text will be created as MTEXT in the AutoCAD editor at the center of the current view.
   Note : If the selected .TXT file contains more then 1000 characters, then only First 1000 characters are shown description window and also on double click only First 1000 characters are used to create MTEXT.
   

