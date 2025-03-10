# assume we are at demo-ccat
docker build -t demo-ccat -f docker\Dockerfile docker

$pwd = Get-Location
$mount = "$pwd"+":/work"
docker run -v $mount -u 1000:1000 -it demo-ccat