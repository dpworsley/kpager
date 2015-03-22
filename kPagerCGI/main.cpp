/*
 * main.cpp
 *
 *  Created on: Oct 21, 2014
 *      Author: dworsle
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <windows.h>

#define LOG_DEBUG(...) printf(__VA_ARGS__)
#define LOG_ERROR(...) fprintf(stderr, __VA_ARGS__)


// Helper macro to convert two-character hex strings to character value
#define ToHex(Y) (Y>='0'&&Y<='9'?Y-'0':Y-'A'+10)

char InputData[4096];

void getAllParams()
{
	// Determining if it is a POST or GET method
	if( getenv( "REQUEST_METHOD" ) == 0 )
	{
		printf("No REQUEST_METHOD, must be running in DOS mode");
		return;
	}
	else if (strcmp( getenv("REQUEST_METHOD"), "POST") == 0)
	{
		// If POST
		char *endptr; // quite useless, but required
		char *len1 = getenv("CONTENT_LENGTH");
		int contentlength = strtol(len1, &endptr, 10);
		fread(InputData , contentlength, 1, stdin);
	}
	else
	{
		// If GET
		strcpy(InputData, getenv("QUERY_STRING"));
	}
}

#define UNDEFINED "undefined"

int getParam(const char *Name, char *Value)
{
	char *pos1 = strstr(InputData, Name);

	if (pos1)
	{
		pos1 += strlen(Name);

		if (*pos1 == '=')
		{
			// Make sure there is an '=' where we expect it
			pos1++;

			while (*pos1 && *pos1 != '&')
			{
				if (*pos1 == '%')
				{ // Convert it to a single ASCII character and store at our Valueination
					*Value++ = (char)ToHex(pos1[1]) * 16 + ToHex(pos1[2]);
					pos1 += 3;
				}
				else if( *pos1=='+' )
				{ // If it's a '+', store a space at our Valueination
					*Value++ = ' ';
					pos1++;
				}
				else
				{
					*Value++ = *pos1++; // Otherwise, just store the character at our Valueination
				}
			}

			*Value++ = '\0';
			return 0;
		}
	}

	strcpy(Value, UNDEFINED); // If param not found, then use default parameter
	return -1;
}


std::string GetExeFileName()
{
  char buffer[MAX_PATH];
  GetModuleFileName( NULL, buffer, MAX_PATH );
  return std::string(buffer);
}

std::string GetExePath()
{
  std::string f = GetExeFileName();
  return f.substr(0, f.find_last_of( "\\/" ));
}


int main()
{

//    char *qs = getenv("QUERY_STRING");
//    fprintf(stderr,"QUERY_STRING (yup) - %s\n", qs);


	getAllParams();

	std::string pathName = GetExePath();
//	fprintf(stderr, "%s", pathName.c_str());



#define CHIME_TYPE_CMD "chimeType"
#define CHIME1_VALUE "CHIME1"
#define CHIME2_VALUE "CHIME2"

#define SPEECH_CMD "speech"

	char value[100];

	// Check for chime type command
	if (getParam(CHIME_TYPE_CMD, value) == 0)
	{
		if (!strcmp(value, CHIME1_VALUE))
		{
			std::string filePathName = pathName + "\\Star_Tours_Chime.wav";
			PlaySound(filePathName.c_str(), NULL, SND_FILENAME);
		}
		else if (!strcmp(value, CHIME2_VALUE))
		{
			std::string filePathName = pathName + "\\tos-redalert.wav";
			PlaySound(filePathName.c_str(), NULL, SND_FILENAME);
		}
	}
	else if (getParam(SPEECH_CMD, value) == 0)
	{
		// Must insert ',,,' for each space in the speech text
		std::string origSpeechText = std::string(value);
		std::string::iterator itor = origSpeechText.begin();

		std::string modifiedSpeechText;
		for (; itor != origSpeechText.end(); itor++)
		{
			if (*itor != ' ')
			{
				modifiedSpeechText.push_back(*itor);
			}
			else
			{
				modifiedSpeechText += ",,,,";
			}
		}

		std::string exePathName = pathName + "\\espeak.exe";
		ShellExecute(NULL, "open", exePathName.c_str(), modifiedSpeechText.c_str(), NULL, SW_SHOWNORMAL );
	}

	// A response is required... so tell the browser to return
	// to the previous page so that the controls are still shown...
	printf("Content-type: text/html\n\n");
	printf("<html>\n");
    printf("<body>\n");
    printf("<script>\n");
    printf("window.history.back()\n");
    printf("</script>\n");
	printf("</body>\n");
	printf("</html>\n");

	return 0;
}


