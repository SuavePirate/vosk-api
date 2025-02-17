// Copyright 2020 Alpha Cephei Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "vosk_api.h"

#include "recognizer.h"
#include "model.h"
#include "spk_model.h"

#if HAVE_CUDA
#include "cudamatrix/cu-device.h"
#include "batch_recognizer.h"
#endif

#include <string.h>

using namespace kaldi;

VoskModel *vosk_model_new(const char *model_path)
{
    try {
        return (VoskModel *)new Model(model_path);
    } catch (...) {
        return nullptr;
    }
}

void vosk_model_free(VoskModel *model)
{
    if (model == nullptr) {
       return;
    }
    ((Model *)model)->Unref();
}

int vosk_model_find_word(VoskModel *model, const char *word)
{
    return (int) ((Model *)model)->FindWord(word);
}

VoskSpkModel *vosk_spk_model_new(const char *model_path)
{
    try {
        return (VoskSpkModel *)new SpkModel(model_path);
    } catch (...) {
        return nullptr;
    }
}

void vosk_spk_model_free(VoskSpkModel *model)
{
    if (model == nullptr) {
       return;
    }
    ((SpkModel *)model)->Unref();
}

VoskRecognizer *vosk_recognizer_new(VoskModel *model, float sample_rate)
{
    try {
        return (VoskRecognizer *)new Recognizer((Model *)model, sample_rate);
    } catch (...) {
        return nullptr;
    }
}

VoskRecognizer *vosk_recognizer_new_spk(VoskModel *model, float sample_rate, VoskSpkModel *spk_model)
{
    try {
        return (VoskRecognizer *)new Recognizer((Model *)model, sample_rate, (SpkModel *)spk_model);
    } catch (...) {
        return nullptr;
    }
}

VoskRecognizer *vosk_recognizer_new_grm(VoskModel *model, float sample_rate, const char *grammar)
{
    try {
        return (VoskRecognizer *)new Recognizer((Model *)model, sample_rate, grammar);
    } catch (...) {
        return nullptr;
    }
}

void vosk_recognizer_set_max_alternatives(VoskRecognizer *recognizer, int max_alternatives)
{
    ((Recognizer *)recognizer)->SetMaxAlternatives(max_alternatives);
}

void vosk_recognizer_set_words(VoskRecognizer *recognizer, int words)
{
    ((Recognizer *)recognizer)->SetWords((bool)words);
}

void vosk_recognizer_set_nlsml(VoskRecognizer *recognizer, int nlsml)
{
    ((Recognizer *)recognizer)->SetNLSML((bool)nlsml);
}

void vosk_recognizer_set_spk_model(VoskRecognizer *recognizer, VoskSpkModel *spk_model)
{
    if (recognizer == nullptr || spk_model == nullptr) {
       return;
    }
    ((Recognizer *)recognizer)->SetSpkModel((SpkModel *)spk_model);
}

int vosk_recognizer_accept_waveform(VoskRecognizer *recognizer, const char *data, int length)
{
    try {
        return ((Recognizer *)(recognizer))->AcceptWaveform(data, length);
    } catch (...) {
        return -1;
    }
}

int vosk_recognizer_accept_waveform_s(VoskRecognizer *recognizer, const short *data, int length)
{
    try {
        return ((Recognizer *)(recognizer))->AcceptWaveform(data, length);
    } catch (...) {
        return -1;
    }
}

int vosk_recognizer_accept_waveform_f(VoskRecognizer *recognizer, const float *data, int length)
{
    try {
        return ((Recognizer *)(recognizer))->AcceptWaveform(data, length);
    } catch (...) {
        return -1;
    }
}

const char *vosk_recognizer_result(VoskRecognizer *recognizer)
{
    return ((Recognizer *)recognizer)->Result();
}

const char *vosk_recognizer_partial_result(VoskRecognizer *recognizer)
{
    return ((Recognizer *)recognizer)->PartialResult();
}

const char *vosk_recognizer_final_result(VoskRecognizer *recognizer)
{
    return ((Recognizer *)recognizer)->FinalResult();
}

void vosk_recognizer_reset(VoskRecognizer *recognizer)
{
    ((Recognizer *)recognizer)->Reset();
}

void vosk_recognizer_free(VoskRecognizer *recognizer)
{
    delete (Recognizer *)(recognizer);
}

void vosk_set_log_level(int log_level)
{
    SetVerboseLevel(log_level);
}

void vosk_gpu_init()
{
#if HAVE_CUDA
//    kaldi::CuDevice::EnableTensorCores(true);
//    kaldi::CuDevice::EnableTf32Compute(true);
    kaldi::CuDevice::Instantiate().SelectGpuId("yes");
    kaldi::CuDevice::Instantiate().AllowMultithreading();
#endif
}

void vosk_gpu_thread_init()
{
#if HAVE_CUDA
    kaldi::CuDevice::Instantiate();
#endif
}

VoskBatchRecognizer *vosk_batch_recognizer_new()
{
#if HAVE_CUDA
    return (VoskBatchRecognizer *)(new BatchRecognizer());
#else
    return NULL;
#endif
}

void vosk_batch_recognizer_free(VoskBatchRecognizer *recognizer)
{
#if HAVE_CUDA
    delete ((BatchRecognizer *)recognizer);
#endif
}

void vosk_batch_recognizer_accept_waveform(VoskBatchRecognizer *recognizer, int id, const char *data, int length)
{
#if HAVE_CUDA
    ((BatchRecognizer *)recognizer)->AcceptWaveform(id, data, length);
#endif
}

void vosk_batch_recognizer_finish_stream(VoskBatchRecognizer *recognizer, int id)
{
#if HAVE_CUDA
    ((BatchRecognizer *)recognizer)->FinishStream(id);
#endif
}

const char *vosk_batch_recognizer_front_result(VoskBatchRecognizer *recognizer, int id)
{
#if HAVE_CUDA
    return ((BatchRecognizer *)recognizer)->FrontResult(id);
#else
    return NULL;
#endif
}

void vosk_batch_recognizer_pop(VoskBatchRecognizer *recognizer, int id)
{
#if HAVE_CUDA
    ((BatchRecognizer *)recognizer)->Pop(id);
#endif
}

void vosk_batch_recognizer_wait(VoskBatchRecognizer *recognizer)
{
#if HAVE_CUDA
    ((BatchRecognizer *)recognizer)->WaitForCompletion();
#endif
}

int vosk_batch_recognizer_get_pending_chunks(VoskBatchRecognizer *recognizer, int id)
{
#if HAVE_CUDA
    return ((BatchRecognizer *)recognizer)->GetNumPendingChunks(id);
#else
    return 0;
#endif
}
