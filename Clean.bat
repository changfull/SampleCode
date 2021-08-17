@echo Off

RD /s /q .\Output .\Bin
FOR /R . %%d IN (.) DO rd /s /q "%%d\.vs" "%%d\ipch" 2>nul

FOR /R .\bin %%d IN (.) DO rd /s /q "%%d\log" 2>nul
FOR /R .\source %%d IN (.) DO rd /s /q "%%d\log" 2>nul

FOR /R . %%d IN (.) DO rd /s /q "%%d\obj" 2>nul

DEL /s /a *.suo *.ncb *.user *.pdb *.ilk *.log *.mem *.sdf *.opensdf 2>nul
DEL /a .\lib\RootNet_d.lib .\lib\RootNet.lib 2>nul