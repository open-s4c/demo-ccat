Push-Location
cd $env:DEMO_HOME
Push-Location
cd ohos
hdc file send run.sh /data/demo/
Pop-Location
hdc file send assets/monalisa.jpg /data/demo/
cd build
hdc file send ccat /data/demo/
Pop-Location