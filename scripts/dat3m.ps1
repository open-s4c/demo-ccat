# 
if (docker images | Select-String dat3m | Select-String -NotMatch ghcr) {
} else {
    docker build -t dat3m -f verify/Dockerfile .
}

$pwd = Get-Location
$mount = "$pwd"+":/work"
docker run -v $mount -it dat3m