$DEMO_HOME= Get-Location
$env:DEMO_HOME = "$DEMO_HOME"

if (Get-Command cmake 2> $null) {
} else {
    $env:Path += ";$DEMO_HOME\scripts"
    $env:Path += ";$DEMO_HOME\ohos\native\build-tools\cmake\bin"
}
$TFILE="$DEMO_HOME\ohos\native\build\cmake\ohos.toolchain.cmake"

cmake -S $DEMO_HOME\ohos -Bbuild -DCMAKE_TOOLCHAIN_FILE="$TFILE" -G "Ninja"