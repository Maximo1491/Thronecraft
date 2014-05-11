#include <al.h>
#include <alc.h>
#include <stdio.h>

//A sound class that will handle all the sounds and effects within Volumetric.
//All sounds will be wav sounds.
class Sound
{
	//Add music and sound buffers as needed here.
	ALuint sound[2];
	ALuint music[3];

	//Keep track of the device and context used in the Sound engine.
	//Keep track of the background gain in order to dynamically change it.
	ALCdevice* device;
	ALCcontext* context;
	float backgroundGain;

	//Track if the time is during the day or night.
	bool dayPlaying;

	//Setup structures to capture the sound data information from the wav files.
	//RIFF data within the wav file.
	struct RIFF_Header
	{
		char chunkID[4];
		long chunkSize;
		char format[4];
	};

	//Wave format within the wav file.
	struct WAVE_Format
	{
		char subChunkID[4];
		long subChunkSize;
		short audioFormat;
		short numOfChannels;
		long sampleRate;
		long byteRate;
		short blockAlign;
		short bitsPerSample;
	};

	//Wave data within the wav file.
	struct WAVE_Data
	{
		char subChunkID[4];
		long subChunk2Size;
	};

public:

	//Constructor
	//The constructor will initialize all the files including all the sound and music files.
	//The sound device will be identified and saved as well as the sound context created.
	Sound()
	{
		backgroundGain = 0.1f;
		dayPlaying = true;

		device = alcOpenDevice(0);
		context = alcCreateContext(device, 0);

		alcMakeContextCurrent (context);

		sound[0] = LoadWavFile ("../../assets/volumetric/audio/effects/sound11a.wav", 1.0f, 1.0f, AL_FALSE);
		sound[1] = LoadWavFile ("../../assets/volumetric/audio/effects/sound7.wav", 1.0f, 1.0f, AL_FALSE);

		music[0] = LoadWavFile ("../../assets/volumetric/audio/music/day1.wav", 0.0f, 1.0f, AL_TRUE);
		music[1] = LoadWavFile ("../../assets/volumetric/audio/music/night1.wav", 0.0f, 1.0f, AL_TRUE);
		music[2] = LoadWavFile ("../../assets/volumetric/audio/music/menu music1.wav", 0.1f, 1.0f, AL_TRUE);
	}

	//Destructor
	~Sound()
	{
		CleanUpSoundEngine();
	}

	//This function will play the sound effect whose index is passed.
	void PlaySound (const int index)
	{
		alSourcePlay(sound[index]);
	}

	//This function shall start to play both the day and night music effects.
	void PlayMusic()
	{
		alSourcePlay (music[0]);
		alSourcePlay (music[1]);
	}

	//This function will stop playing both the day and night music effects.
	void StopMusic()
	{
		alSourceStop (music[0]);
		alSourceStop (music[1]);
	}

	//This function will play the music effect for the start menu.
	void PlayMenuMusic()
	{
		alSourcePlay (music[2]);
	}

	//This function will stop playing the music effect for the start menu.
	void StopMenuMusic()
	{
		alSourceStop (music[2]);
	}

	//This function will adjust the background gain of the day and night music files.
	//The day and night music will fade in and out during the sunset and sunrise in the game.
	//When the day and night are in full swing, the gain of the appropriate music will be 1.0f.
	void AdjustBackground(const float &value)
	{

		//Check first to see if they background gain has changed at all.
		if (backgroundGain != value)
		{
			//Change the background gain.
			backgroundGain = value;

			//Check to see if the background gain is below 0 or above 1.
			//If so change the gain to 0 or 1 appropriately.
			if (backgroundGain < 0.0f)
				backgroundGain = 0.0f;
			else if (backgroundGain > 1.0f)
				backgroundGain = 1.0f;

			//Check to see if the game is during the day.  If so, adjust the gain
			//of the day music.  If not, adjust the gain of the night music.
			if (dayPlaying)
				alSourcef (music[0], AL_GAIN, backgroundGain);
			else
				alSourcef (music[1], AL_GAIN, backgroundGain);
		}
	}

	//This function will change the boolean keeping track of if day is playing.
	void setDayPlaying (const bool value)
	{
		dayPlaying = value;
	}

	//This function will return the background gain setting.
	float GetBackgroundGain ()
	{
		return backgroundGain;
	}

private:

	//This function will destroy the context used and stop using the sound device.
	void CleanUpSoundEngine()
	{
		alcMakeContextCurrent(0);
		alcDestroyContext(context);
		alcCloseDevice(device);
	}

	ALuint LoadWavFile(char* _fileName, const float _gain, const float _pitch, const int _loop)
	{
		//Declare a file, data, wave format, riff header, and wave data objects.  This will be used
		//to break down the wave file for playing.
		FILE* soundFile = NULL;
		WAVE_Format waveFormat;
		RIFF_Header riffHeader;
		WAVE_Data waveData;
		unsigned char* data;

		//Create a format, buffer, and source object for playing the wav file.
		ALuint buffer;
		ALuint source;
		ALenum format;

		//Create arrays to hold the source position, source velocity, listener position,
		//listener velocity, and listener orientation. Velocity for both will be 0.
		//Position will be set to the same area and turn the listener so it is facing
		//the source.
		ALfloat SourcePos[] = { 0.0, 0.0, 0.0 };
		ALfloat SourceVel[] = { 0.0, 0.0, 0.0 };
		ALfloat ListenerPos[] = { 0.0, 0.0, 0.0 };
		ALfloat ListenerVel[] = { 0.0, 0.0, 0.0 };
		ALfloat ListenerOri[] = { 0.0, 0.0, -1.0, 0.0, 1.0, 0.0 };  
		
		//Open the sound file prepping it for reading.
		soundFile = fopen(_fileName, "rb");

		//Read the riff header, wwave format, and wave data respectively.
		fread(&riffHeader, sizeof(RIFF_Header), 1, soundFile);
		fread(&waveFormat, sizeof(WAVE_Format), 1, soundFile);
		fread(&waveData, sizeof(WAVE_Data), 1, soundFile);

		//Initialize and load the data from the wave file.
		data = new unsigned char[waveData.subChunk2Size];
		fread(data, waveData.subChunk2Size, 1, soundFile);

		//Check to see how many bitsPerSample and number of channels the wav
		//file uses.  Set the format for the wave file based on this information.
		if (waveFormat.bitsPerSample == 8)
		{
			if (waveFormat.numOfChannels == 1)
			{
				format = AL_FORMAT_MONO8;
			}

			else if (waveFormat.numOfChannels == 2)
			{
				format = AL_FORMAT_STEREO8;
			}
		}

		else if (waveFormat.bitsPerSample == 16)
		{
			if (waveFormat.numOfChannels == 1)
			{
				format = AL_FORMAT_MONO16;
			}

			else if (waveFormat.numOfChannels == 2)
			{
				format = AL_FORMAT_STEREO16;
			}
		}
		
		//Generate a source and buffer in openAL.
		alGenBuffers(1, &buffer);
		alGenSources(1, &source);

		//Add the wave data to the buffer using the format and sampleRate of the wav file.
		alBufferData(buffer, format, data, waveData.subChunk2Size, waveFormat.sampleRate);

		//Set the listener position, velocity, and orientation in openAL.
		alListenerfv(AL_POSITION, ListenerPos);
		alListenerfv(AL_VELOCITY, ListenerVel);
		alListenerfv(AL_ORIENTATION, ListenerOri);

		//Set the source buffer, gain, pitch, position, velocity, and if the sound loops in
		//openAL.
		alSourcei(source, AL_BUFFER, buffer);
		alSourcef(source, AL_PITCH, _pitch);
		alSourcef(source, AL_GAIN, _gain);
		alSourcefv(source, AL_POSITION, SourcePos);
		alSourcefv(source, AL_VELOCITY, SourceVel);
		alSourcei(source, AL_LOOPING, _loop);

		//Return the source to store in the element structure.
		return source;
	}
};