function Component() {
    if (installer.isInstaller()) {
        
        ComponentSelectionPage = gui.pageById(QInstaller.ComponentSelection);

        if (systemInfo.productType === "windows")
            installer.setDefaultPageVisible(QInstaller.StartMenuSelection, true);
    }
}

Component.prototype.createOperations = function()
{
    component.createOperations();
    if (systemInfo.productType === "windows") {
        component.addOperation("CreateShortcut", 
                                "@TargetDir@/Perticon.exe", 
                                "@DesktopDir@/Perticon.lnk",
                                "workingDirectory=@TargetDir@", 
                                "iconPath=@TargetDir@/logo.png",
                                "description=Perticon");
    } else {
        component.addOperation("CreateDesktopEntry", 
            "Perticon.desktop", 
            "Type=Application\nTerminal=false\nExec=@TargetDir@/Perticon.sh\nName=Perticon\nIcon=@TargetDir@/Logo.png");
    }
}
