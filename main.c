
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>
#include "lame.h"

char * changeStringAppendix( char*);


char * changeStringAppendix(char *fileName)
{
    char oldAppendix[] = "wav";
    char newAppendix[] = "mp3";

	char *tempString, *searchStart;
	int len=0;

	searchStart = strstr(fileName, oldAppendix);
	if(searchStart == NULL)
	{
		return fileName;
	}

	tempString = (char*) malloc(strlen(fileName) * sizeof(char));
	if(tempString == NULL)
	{
		return NULL;
	}

	strcpy(tempString, fileName);

	len = searchStart - fileName;
	fileName[len] = '\0';

	strcat(fileName, newAppendix);

	len += strlen(oldAppendix);
	strcat(fileName, (char*)tempString+len);

	free(tempString);

	return fileName;
}

bool isWav(const char* testString)
{
    bool ret = false;
    if (testString)
    {
        size_t size = strlen(testString);
        if (size >= 4 &&
            testString[size-4] == '.' &&
            testString[size-3] == 'w' &&
            testString[size-2] == 'a' &&
            testString[size-1] == 'v')
        {
          ret = true;
        }
    }
  return ret;
}


int main( int argc, char const *argv[] )
{

    if( argc < 2 )
    {
        printf( "Please set a directory when starting the app  \n" );
        return 0;
    }

    printf( "Scan directory for wav files: \n" );

    DIR *d;
    struct dirent *dir;

    d = opendir( argv[1] );
    if (d)
    {
        while ((dir = readdir(d)) != NULL)
        {
            if (isWav(dir->d_name) )
            {
                char wavFileNameStr[80] = "";
                char mp3FileNameStr[80] = "";

                strcat(wavFileNameStr, argv[1]);
                strcat(wavFileNameStr, "/");
                strcat(wavFileNameStr, dir->d_name);

                strcpy(mp3FileNameStr, wavFileNameStr);
                changeStringAppendix( mp3FileNameStr );

                //printf("Using Version %s \n", get_lame_version());

                int read, write;
                printf("Open: %s\n", wavFileNameStr );
                printf("Open: %s\n", mp3FileNameStr );


                FILE *pcm = fopen(wavFileNameStr, "rb");
                FILE *mp3 = fopen(mp3FileNameStr, "wb");

                const int PCM_SIZE = 8192;
                const int MP3_SIZE = 8192;

                short int pcm_buffer[PCM_SIZE*2];
                unsigned char mp3_buffer[MP3_SIZE];

                lame_t lame = lame_init();
                lame_set_in_samplerate(lame, 44100);
                lame_set_VBR(lame, vbr_default);
               // lame_init_params(lame);

                if (lame_init_params(lame) < 0)
                {
                    fclose(mp3);
                    fclose(pcm);

                }
                else
                {
                printf("Encoding: " );
                    do
                    {
                        printf("." );
                        read = fread(pcm_buffer, 2*sizeof(short int), PCM_SIZE, pcm);
                        if (read == 0)
                            write = lame_encode_flush(lame, mp3_buffer, MP3_SIZE);
                        else
                            write = lame_encode_buffer_interleaved(lame, pcm_buffer, read, mp3_buffer, MP3_SIZE);
                        fwrite(mp3_buffer, write, 1, mp3);
                    } while (read != 0);

                    printf("\n Encoding succeeded, close files. \n" );
                    fclose(mp3);
                    fclose(pcm);

                    lame_close(lame);



                }
            }

        }
        closedir(d);


    }


    return 0;
}
