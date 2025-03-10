# OpenHarmony OS

This directory contains scripts to configure, compile and upload ccat to a development board with OHOS.  We tested on a Hihope scdayu200 board running with OHOS 5.1.

## Configure toolchain

Download a precompiled toolchain from [ohos-sdk-windows_linux-public.tar.gz](
https://gitee.com/openharmony/docs/blob/master/en/release-notes/OpenHarmony-v5.0.1-release) (see Table 2).

Untar the release. You'll find several zipped files. Unzip `native-*.zip`. After that move the generated `native` directory **inside** this directory, ie, `demo-ccat/ohos`. 

Open powershell in `demo-ccat` and run

    ohos/configure.ps1

That should create the `demo-ccat/build` directory.

## Compile `ccat` for OHOS

You can build the file using the provided `ninja` build system distributed with the toolchain:

    ninja -C build

## Upload `ccat` to the board

Assuming you have the board properly connected, you can access the board with `hdc shell`. This program is also distributed in the toolchain.

Run `ohos/upload.ps1` to upload `ccat`, a run script and an image into the `/tmp/demo` directory on the board.

Now, connect with `hdc shell` and call `run.sh`.