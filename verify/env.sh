CFLAGS=""
CFLAGS="${CFLAGS} -I$(pwd)/src"
CFLAGS="${CFLAGS} -I$(pwd)/verify"
CFLAGS="${CFLAGS} -DVSYNC_VERIFICATION -DVSYNC_VERIFICATION_GENERIC"
CFLAGS="${CFLAGS}"

export CFLAGS
