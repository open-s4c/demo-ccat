# assume we are at demo-ccat
docker build -t demo-ccat -f docker\Dockerfile docker

$pwd = Get-Location
$mount = "$pwd"+":/work"
docker run --rm -it -v $mount demo-ccat