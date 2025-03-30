$DEMO_HOME = Get-Location
$TFILE="$DEMO_HOME\ohos\native\build\cmake\ohos.toolchain.cmake"
cmake -S $DEMO_HOME\ohos -Bbuild -DCMAKE_TOOLCHAIN_FILE="$TFILE" -G "Ninja"