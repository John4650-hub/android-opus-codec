//
// Created by Loboda Alexey on 21.05.2020.
// Updated by Kafuuma John Delvin on 05.06.2026
//

#include <string>
#include <jni.h>
#include <vector>
#include <cmath>
#include "opusenc.h"
#include "opusfile.h"
#include "codec/CodecOpus.h"
#include "utils/SamplesConverter.h"

CodecOpus codec;
// Global encoder instance
static OggOpusEnc* g_enc = nullptr;
static OggOpusComments* g_comments = nullptr;
static OggOpusFile* opusFile = nullptr;
//
// Encoding
//

extern "C"
JNIEXPORT jint JNICALL Java_com_theeasiestway_opus_Opus_encoderInit(JNIEnv *env, jobject thiz, jint sample_rate, jint num_channels, jint application) {
    return codec.encoderInit(sample_rate, num_channels, application);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_theeasiestway_opus_Opus_encoderSetBitrate(JNIEnv *env, jobject thiz, jint bitrate) {
    return codec.encoderSetBitrate(bitrate);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_theeasiestway_opus_Opus_encoderSetComplexity(JNIEnv *env, jobject thiz, jint complexity) {
    return codec.encoderSetComplexity(complexity);
}

extern "C"
JNIEXPORT jbyteArray JNICALL
Java_com_theeasiestway_opus_Opus_encode___3BI(JNIEnv *env, jobject thiz, jbyteArray bytes, jint frame_size) {
    jbyte *nativeBytes = env->GetByteArrayElements(bytes, 0);
    std::vector<uint8_t> encodedData = codec.encode((uint8_t *) nativeBytes, frame_size);
    int encodedSize = encodedData.size();
    if (encodedSize <= 0) return nullptr;

    jbyteArray result = env->NewByteArray(encodedSize);
    env->SetByteArrayRegion(result, 0, encodedSize, (jbyte *) encodedData.data());
    env->ReleaseByteArrayElements(bytes, nativeBytes, 0);

    return result;
}

extern "C"
JNIEXPORT jshortArray JNICALL
Java_com_theeasiestway_opus_Opus_encode___3SI(JNIEnv *env, jobject thiz, jshortArray shorts, jint frame_size) {
    jshort *nativeShorts = env->GetShortArrayElements(shorts, 0);
    jint length = env->GetArrayLength(shorts);

    std::vector<short> encodedData = codec.encode(nativeShorts, length, frame_size);
    int encodedSize = encodedData.size();
    if (encodedSize <= 0) return nullptr;

    jshortArray result = env->NewShortArray(encodedSize);
    env->SetShortArrayRegion(result, 0, encodedSize, encodedData.data());
    env->ReleaseShortArrayElements(shorts, nativeShorts, 0);

    return result;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_theeasiestway_opus_Opus_encoderRelease(JNIEnv *env, jobject thiz) {
    codec.encoderRelease();
}

//
// Decoding
//

extern "C"
JNIEXPORT jint JNICALL
Java_com_theeasiestway_opus_Opus_decoderInit(JNIEnv *env, jobject thiz, jint sample_rate, jint num_channels) {
    return codec.decoderInit(sample_rate, num_channels);
}

extern "C"
JNIEXPORT jbyteArray JNICALL
Java_com_theeasiestway_opus_Opus_decode___3BII(JNIEnv *env, jobject thiz, jbyteArray bytes, jint frame_size, jint fec) {
    jbyte *nativeBytes = env->GetByteArrayElements(bytes, 0);
    jint length = env->GetArrayLength(bytes);

    std::vector<uint8_t> encodedData = codec.decode((uint8_t *) nativeBytes, length, frame_size, fec);
    int encodedSize = encodedData.size();
    if (encodedSize <= 0) return nullptr;

    jbyteArray result = env->NewByteArray(encodedSize);
    env->SetByteArrayRegion(result, 0, encodedSize, (jbyte *) encodedData.data());
    env->ReleaseByteArrayElements(bytes, nativeBytes, 0);

    return result;
}

extern "C"
JNIEXPORT jshortArray JNICALL
Java_com_theeasiestway_opus_Opus_decode___3SII(JNIEnv *env, jobject thiz, jshortArray shorts, jint frame_size, jint fec) {
    jshort *nativeShorts = env->GetShortArrayElements(shorts, 0);
    jint length = env->GetArrayLength(shorts);

    std::vector<short> encodedData = codec.decode(nativeShorts, length, frame_size, fec);
    int encodedSize = encodedData.size();
    if (encodedSize <= 0) return nullptr;

    jshortArray result = env->NewShortArray(encodedSize);
    env->SetShortArrayRegion(result, 0, encodedSize, encodedData.data());
    env->ReleaseShortArrayElements(shorts, nativeShorts, 0);

    return result;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_theeasiestway_opus_Opus_decoderRelease(JNIEnv *env, jobject thiz) {
    codec.decoderRelease();
}

//
// Utils
//

extern "C"
JNIEXPORT jshortArray JNICALL
Java_com_theeasiestway_opus_Opus_convert___3B(JNIEnv *env, jobject thiz, jbyteArray bytes) {
    uint8_t *nativeBytes = (uint8_t *) env->GetByteArrayElements(bytes, 0);
    jint length = env->GetArrayLength(bytes);

    std::vector<short> shorts = SamplesConverter::convert(&nativeBytes, length);
    int size = shorts.size();
    if (!size) return nullptr;

    jshortArray result = env->NewShortArray(size);
    env->SetShortArrayRegion(result, 0, size, shorts.data());
    env->ReleaseByteArrayElements(bytes, (jbyte *) nativeBytes, 0);

    return result;
}

extern "C"
JNIEXPORT jbyteArray JNICALL
Java_com_theeasiestway_opus_Opus_convert___3S(JNIEnv *env, jobject thiz, jshortArray shorts) {
    short *nativeShorts = env->GetShortArrayElements(shorts, 0);
    jint length = env->GetArrayLength(shorts);

    std::vector<uint8_t> bytes = SamplesConverter::convert(&nativeShorts, length);
    int size = bytes.size();
    if (!size) return nullptr;

    jbyteArray result = env->NewByteArray(size);
    env->SetByteArrayRegion(result, 0, size, (jbyte *) bytes.data());
    env->ReleaseShortArrayElements(shorts, nativeShorts, 0);

    return result;
}

// save
extern "C"
JNIEXPORT jint JNICALL
Java_com_theeasiestway_opus_Opus_oggEncoderInit(JNIEnv *env, jobject thiz,
                                                jstring path,
                                                jint sampleRate,
                                                jint channels,
                                                jint family) {
    const char *cpath = env->GetStringUTFChars(path, nullptr);

    int error;
    g_comments = ope_comments_create();
    g_enc = ope_encoder_create_file(cpath, g_comments,
                                    sampleRate, channels, family, &error);

    env->ReleaseStringUTFChars(path, cpath);

    return error; // return OPE_OK (0) if success, else error code
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_theeasiestway_opus_Opus_writeChunk(JNIEnv *env, jobject thiz,
                                               jshortArray pcmData,
                                               jint channels) {
    if (!g_enc) return OPE_INTERNAL_ERROR;

    jshort *pcm = env->GetShortArrayElements(pcmData, nullptr);
    jsize length = env->GetArrayLength(pcmData);

    int err = ope_encoder_write(g_enc, (const opus_int16*)pcm, length / channels);

    env->ReleaseShortArrayElements(pcmData, pcm, 0);
    return err;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_theeasiestway_opus_Opus_closeOggEncoder(JNIEnv *env, jobject thiz) {
    if (g_enc) {
        ope_encoder_drain(g_enc);
        ope_encoder_destroy(g_enc);
        g_enc = nullptr;
    }
    if (g_comments) {
        ope_comments_destroy(g_comments);
        g_comments = nullptr;
    }
}

/**
 * Open Opus file.
 */
extern "C"
JNIEXPORT jint JNICALL
Java_com_theeasiestway_opus_Opus_openFile(JNIEnv* env, jobject thiz, jstring path) {
    const char* filePath = env->GetStringUTFChars(path, nullptr);
    int error;
    opusFile = op_open_file(filePath, &error);
    env->ReleaseStringUTFChars(path, filePath);
    return (error == 0 && opusFile) ? 0 : error;
}

/**
 * Seek to given time (milliseconds).
 */
extern "C"
JNIEXPORT jint JNICALL
Java_com_theeasiestway_opus_Opus_seekMs(JNIEnv* env, jobject thiz, jlong ms) {
    if (!opusFile) return OP_EFAULT;
    ogg_int64_t pcmOffset = (ms * 48); // 48 samples per ms at 48kHz
    return op_pcm_seek(opusFile, pcmOffset);
}

/**
 * Decode next chunk and return PCM as byte[].
 */
extern "C"
JNIEXPORT jbyteArray JNICALL
Java_com_theeasiestway_opus_Opus_decodeChunk(JNIEnv* env, jobject thiz, jint maxSamples) {
    if (!opusFile) return nullptr;

    std::vector<opus_int16> pcm(maxSamples); // mono
    int samplesDecoded = op_read(opusFile, pcm.data(), pcm.size(), nullptr);

    if (samplesDecoded <= 0) return nullptr;

    jbyteArray output = env->NewByteArray(samplesDecoded * sizeof(opus_int16));
    env->SetByteArrayRegion(output, 0, samplesDecoded * sizeof(opus_int16),
                            reinterpret_cast<jbyte*>(pcm.data()));
    return output;
}

/**
 * Compute peak amplitude of last decoded chunk.
 * Trying to replicate MediaRecorder.getMaxAmplitude()
 */
extern "C"
JNIEXPORT jint JNICALL
Java_com_theeasiestway_opus_Opus_getAmplitude(JNIEnv* env, jobject thiz, jbyteArray pcmData) {
    jsize len = env->GetArrayLength(pcmData);
    jbyte* buf = env->GetByteArrayElements(pcmData, nullptr);

    int16_t* samples = reinterpret_cast<int16_t*>(buf);
    int sampleCount = len / sizeof(int16_t);

    int maxAmp = 0;
    for (int i = 0; i < sampleCount; i++) {
        int amp = std::abs(samples[i]); // absolute value
        if (amp > maxAmp) maxAmp = amp;
    }

    env->ReleaseByteArrayElements(pcmData, buf, JNI_ABORT);
    return maxAmp; // 0–32767
}

/**
 * Get current playback position in milliseconds.
 */
extern "C"
JNIEXPORT jlong JNICALL
Java_com_theeasiestway_opus_Opus_getPosition(JNIEnv* env, jobject thiz) {
    if (!opusFile) return -1;
    ogg_int64_t posSamples = op_pcm_tell(opusFile);
    if (posSamples < 0) return -1;
    return posSamples / 48; // convert samples â†’ ms (48 samples per ms at 48kHz)
}

/**
 * Get total duration of the file in milliseconds.
 */
extern "C"
JNIEXPORT jlong JNICALL
Java_com_theeasiestway_opus_Opus_getDuration(JNIEnv* env, jobject thiz) {
    if (!opusFile) return -1;
    ogg_int64_t totalSamples = op_pcm_total(opusFile, -1); 
    if (totalSamples < 0) return -1;
    return totalSamples / 48; // convert samples â†’ ms
}


/**
 * Close file.
 */
extern "C"
JNIEXPORT void JNICALL
Java_com_theeasiestway_opus_Opus_closeFile(JNIEnv* env, jobject thiz) {
    if (opusFile) {
        op_free(opusFile);
        opusFile = nullptr;
    }
}

