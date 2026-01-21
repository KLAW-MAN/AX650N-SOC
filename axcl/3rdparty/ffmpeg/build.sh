#!/bin/sh

BASE=`pwd`
CROSS=
OUTPUT_PATH=
FFMPEG_SRC=
AXCL_INC_PATH=
DBG=
HOST=
CLEAN_BUILD=no


get_opt() {
    while [ $# -gt 0 ]; do
        case $1 in
            cross=*)
                CROSS="${1#*=}"
                echo "CROSS: ${CROSS}"
                ;;
            dbg=*)
                DBG="${1#*=}"
                echo "DEBUG: ${DBG}"
                ;;
            axcl_inc=*)
                AXCL_INC_PATH="${1#*=}"
                echo "AXCL_INC_PATH: ${DBG}"
                ;;
            out=*)
                OUTPUT_PATH="${1#*=}"
                echo "OUTPUT_PATH: ${OUTPUT_PATH}"
                ;;
            host=*)
                HOST="${1#*=}"
                echo "HOST: ${HOST}"
                ;;
            src=*)
                FFMPEG_SRC="${1#*=}"
                echo "SRC PATH: ${FFMPEG_SRC}"
                ;;
            clean)
                CLEAN_BUILD="yes"
                ;;
            *)
                echo "Unknown parameter: $1"
                ;;
        esac
        shift
    done
}

make_arm64() {
    cd ${FFMPEG_SRC}

    if [ "$DBG" = "yes" ] ; then
        echo "configure ffmpeg with debug info"
        ./configure \
        --prefix=${OUTPUT_PATH} \
        --enable-cross-compile \
        --cross-prefix=${CROSS} \
        --arch=aarch64 \
        --host-os=linux \
        --target-os=linux \
        --cc=${CROSS}gcc \
        --enable-shared \
        --disable-static \
        --enable-gpl \
        --enable-muxer=mp4 \
        --enable-muxer=flv \
        --enable-muxer=h264 \
        --enable-muxer=hevc \
        --enable-demuxer=flv \
        --enable-demuxer=h264 \
        --enable-demuxer=hevc \
        --enable-demuxer=rtsp \
        --enable-parser=hevc \
        --enable-ffmpeg \
        --enable-swscale  \
        --disable-avdevice \
        --enable-encoders \
        --enable-filters \
        --enable-axmm \
        --enable-cross-compile \
        --disable-large-tests \
        --disable-doc \
        --disable-asm \
        --disable-stripping \
        --enable-debug \
        --extra-cflags="-I$AXCL_INC_PATH" \
        --extra-ldflags="-ldl"
    else
        echo "configure ffmpeg"
        ./configure \
        --prefix=${OUTPUT_PATH} \
        --enable-cross-compile \
        --cross-prefix=${CROSS} \
        --arch=aarch64 \
        --host-os=linux \
        --target-os=linux \
        --cc=${CROSS}gcc \
        --enable-shared \
        --disable-static \
        --enable-gpl \
        --enable-muxer=mp4 \
        --enable-muxer=flv \
        --enable-muxer=h264 \
        --enable-muxer=hevc \
        --enable-demuxer=flv \
        --enable-demuxer=h264 \
        --enable-demuxer=hevc \
        --enable-demuxer=rtsp \
        --enable-parser=hevc \
        --enable-ffmpeg \
        --enable-swscale  \
        --disable-avdevice \
        --enable-encoders \
        --enable-filters \
        --enable-axmm \
        --enable-cross-compile \
        --disable-large-tests \
        --disable-doc \
        --disable-logging \
        --enable-pic \
        --extra-cflags="-I$AXCL_INC_PATH" \
        --extra-ldflags="-ldl -Wl,--build-id"
    fi

    make -j32 && make install
}

make_riscv() {
    cd ${FFMPEG_SRC}

    if [ "$DBG" = "yes" ] ; then
        echo "configure ffmpeg with debug info"
        ./configure \
        --prefix=${OUTPUT_PATH} \
        --enable-cross-compile \
        --cross-prefix=${CROSS} \
        --arch=riscv64 \
        --host-os=linux \
        --target-os=linux \
        --cc=${CROSS}gcc \
        --enable-shared \
        --disable-static \
        --enable-gpl \
        --enable-muxer=mp4 \
        --enable-muxer=flv \
        --enable-muxer=h264 \
        --enable-muxer=hevc \
        --enable-demuxer=flv \
        --enable-demuxer=h264 \
        --enable-demuxer=hevc \
        --enable-demuxer=rtsp \
        --enable-parser=hevc \
        --enable-ffmpeg \
        --enable-swscale  \
        --disable-avdevice \
        --enable-encoders \
        --enable-filters \
        --enable-axmm \
        --enable-cross-compile \
        --disable-large-tests \
        --disable-doc \
        --disable-asm \
        --disable-stripping \
        --enable-debug \
        --extra-cflags="-I$AXCL_INC_PATH" \
        --extra-ldflags="-ldl"
    else
        echo "configure ffmpeg"
        ./configure \
        --prefix=${OUTPUT_PATH} \
        --enable-cross-compile \
        --cross-prefix=${CROSS} \
        --arch=riscv64 \
        --host-os=linux \
        --target-os=linux \
        --cc=${CROSS}gcc \
        --enable-shared \
        --disable-static \
        --enable-gpl \
        --enable-muxer=mp4 \
        --enable-muxer=flv \
        --enable-muxer=h264 \
        --enable-muxer=hevc \
        --enable-demuxer=flv \
        --enable-demuxer=h264 \
        --enable-demuxer=hevc \
        --enable-demuxer=rtsp \
        --enable-parser=hevc \
        --enable-ffmpeg \
        --enable-swscale  \
        --disable-avdevice \
        --enable-encoders \
        --enable-filters \
        --enable-axmm \
        --enable-cross-compile \
        --disable-large-tests \
        --disable-doc \
        --disable-logging \
        --enable-pic \
        --extra-cflags="-I$AXCL_INC_PATH" \
        --extra-ldflags="-ldl"
    fi

    make -j32 && make install
}

make_x86() {
    cd ${FFMPEG_SRC}

    if [ "$DBG" = "yes" ] ; then
        echo "configure ffmpeg with debug info"
        ./configure \
        --prefix=${OUTPUT_PATH} \
        --enable-cross-compile \
        --cross-prefix=${CROSS} \
        --arch=x86_64 \
        --host-os=linux \
        --target-os=linux \
        --cc=${CROSS}gcc \
        --enable-shared \
        --disable-static \
        --enable-gpl \
        --enable-muxer=mp4 \
        --enable-muxer=flv \
        --enable-muxer=h264 \
        --enable-muxer=hevc \
        --enable-demuxer=flv \
        --enable-demuxer=h264 \
        --enable-demuxer=hevc \
        --enable-demuxer=rtsp \
        --enable-parser=hevc \
        --enable-ffmpeg \
        --enable-swscale  \
        --disable-avdevice \
        --enable-encoders \
        --enable-filters \
        --enable-axmm \
        --enable-cross-compile \
        --disable-large-tests \
        --disable-doc \
        --disable-asm \
        --disable-stripping \
        --enable-debug \
        --extra-cflags="-I$AXCL_INC_PATH" \
        --extra-ldflags="-ldl"
    else
        echo "configure ffmpeg"
        ./configure \
        --prefix=${OUTPUT_PATH} \
        --enable-cross-compile \
        --cross-prefix=${CROSS} \
        --arch=x86_64 \
        --host-os=linux \
        --target-os=linux \
        --cc=${CROSS}gcc \
        --enable-shared \
        --disable-static \
        --enable-gpl \
        --enable-muxer=mp4 \
        --enable-muxer=flv \
        --enable-muxer=h264 \
        --enable-muxer=hevc \
        --enable-demuxer=flv \
        --enable-demuxer=h264 \
        --enable-demuxer=hevc \
        --enable-demuxer=rtsp \
        --enable-parser=hevc \
        --enable-ffmpeg \
        --enable-swscale  \
        --disable-avdevice \
        --enable-encoders \
        --enable-filters \
        --enable-axmm \
        --enable-cross-compile \
        --disable-large-tests \
        --disable-doc \
        --enable-pic \
        --disable-x86asm \
        --disable-libxcb \
        --disable-zlib \
        --disable-bzlib \
        --disable-libxcb_shm \
        --disable-lzma \
        --disable-logging \
        --extra-cflags="-I$AXCL_INC_PATH" \
        --extra-ldflags="-ldl" \
        --extra-libs="-ldl"
    fi

    make -j32 && make install
}


clean() {
    cd ${FFMPEG_SRC}
    if [ -e "./ffbuild/config.mak" ]; then
        make distclean
    fi
}

get_opt "$@"

if [ "$CLEAN_BUILD" = "yes" ]; then
    clean
else
    if [ "$HOST" = "x86" ] ; then
        make_x86
    elif [ "$HOST" = "riscv" ] ; then
        make_riscv
    else
        make_arm64
    fi
fi
