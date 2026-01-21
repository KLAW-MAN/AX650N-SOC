sample_audio

1）Functional description:
The code under the audio folder is the sample reference code provided by the SDK package, which is convenient for
customers to quickly understand the configuration process of the entire audio module.
The sample code demonstrates the following functions: ai recording, ao playback, ai_aenc encoding, adec_ao decoding.
Our company provides codec drivers for everest manufacturers: es8388, es7210, es8311, es7243l and es8156


2）Usage examples:
Example 1: View help information
sample_audio -h

Example 2: Recording 16kHz audio
sample_audio ai -D 0 -d 2 -r 16000 -p 160 -w 1

Example 3: Enable FIXED mode echo cancellation to record 16kHz audio
sample_audio ai -D 0 -d 2 -r 16000 -p 160 --aec-mode 2 --routing-mode 0 --layout 1 -w 1

Example 4: Playing 16kHz audio
sample_audio ao -D 0 -d 3 -r 16000

Example 5: Playing 48kHz audio using HDMI
sample_audio ao -D 0 -d 0 -r 48000 -p 1024

Example 6: Recording 16kHz audio and encoding
sample_audio ai_aenc -D 0 -d 2 -r 16000 -p 160 -w 1

Example 7: Recording 16kHz audio and encoding in mono
sample_audio ai_aenc -D 0 -d 2 -r 16000 -p 160 --layout 1 --aenc-chns 1 -w 1

Example 8: Decode and play 16kHz audio
sample_audio adec_ao -D 0 -d 3 -r 16000

3）Example running results:
After successful running, press Ctrl+C to exit, and a wave file should be generated in the current directory. Users can open it to see the actual effect.

4）Notes:
 (1) For the sound card number and device number, please refer to /dev/snd/. For example:
   pcmC0D0p: card 0, device 0, playback device
   pcmC0D1c: card 0, device 1, capture device
（2）HDMI only supports 48kHz sampling rate
（3）To play HDMI audio, you need to run sample_vo before running sample_audio. For details, refer to sample_vo's README.