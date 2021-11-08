import struct
import pyaudio
import pvporcupine
import serial

porcupine = None
pa = None
audio_stream = None

ser = serial.Serial('\\.\COM3', 9600) # change COM port here if necessary
# ser = serial.Serial('/dev/ttyACM0', 9600) # use this one if running on raspi

ser.flush()

try: 
    porcupine = pvporcupine.create(keyword_paths=['trick-or-treat__en_windows_2021-11-17-utc_v1_9_0.ppn']) # use this one for windows testing
    # porcupine = pvporcupine.create(keyword_paths=['trick-or-treat__en_raspberry-pi_2021-11-17-utc_v1_9_0.ppn']) # use this one if on raspi

    pa = pyaudio.PyAudio()
    audio_stream = pa.open(
        rate=porcupine.sample_rate,
        channels=1,
        format=pyaudio.paInt16,
        input=True,
        frames_per_buffer=porcupine.frame_length
    )
    while True:
        pcm = audio_stream.read(porcupine.frame_length)
        pcm = struct.unpack_from("h" * porcupine.frame_length, pcm)

        keyword_index = porcupine.process(pcm)

        if keyword_index >= 0:
            print("detected!")
            ser.write(b"detected from raspi!")
            ser.flush()


finally:
    if porcupine is not None:
        porcupine.delete()
    if audio_stream is not None:
        audio_stream.close()
    if pa is not None:
        pa.terminate()



