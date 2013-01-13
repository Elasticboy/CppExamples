// 05_MsSpeechApi.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <sphelper.h>
#include <sapi.h>
#include <iostream>

class StreamObject : ISpStreamFormat 
{

};

int speakTest();

int main(int argc, char** argv)
{
	speakTest();
	return 0;
}

int speakTest()
{
	ISpVoice * ispVoice = nullptr;

    if (FAILED(::CoInitialize(nullptr))) {
		return EXIT_FAILURE;
	}

	HRESULT hr = CoCreateInstance(CLSID_SpVoice, nullptr, CLSCTX_ALL, IID_ISpVoice, (void **)&ispVoice);
	/*
	HRESULT hr = SpCreateBestObject(SPCAT_VOICES, L"Language=409", L"Gender=Female", &ispVoice);
	if (FAILED(hr)) {
		std::cout << "Failed creating ispVoice" << std::endl;
	}
	*/

	//hr = ::CoCreateInstance(CLSID_SpVoice,nullptr, CLSCTX_ALL, IID_ISpVoice, reinterpret_cast<void**>(&ispvoice));
    if(SUCCEEDED( hr ) ) {
	
		ISpObjectToken* cpTokenEng;
		hr = SpFindBestToken(SPCAT_VOICES, L"Language=409", L"Gender=Female", &cpTokenEng);
		ispVoice->SetVoice(cpTokenEng);

		std::cout << "Hello World synchrone" << std::endl;
		hr = ispVoice->Speak(L"Hello world", SPF_DEFAULT, nullptr);
		/*
		std::cout << "Bemol sur un Hello World synchrone " << std::endl;
		hr = ispVoice->Speak(L" <pitch middle = '-30'/> Hello world", SPF_IS_XML, nullptr );
		hr = ispVoice->Speak(L" <pitch middle = '+30'/> Hello world", SPF_IS_XML, nullptr );
		

		// Sans temporisation la terminaison du programme coupe le sifflet à la voix :)
		std::cout << "Hello World asynchrone" << std::endl;
		hr = ispVoice->Speak(L"Hello world", SPF_ASYNC|SPF_PURGEBEFORESPEAK , nullptr);
		Sleep(2000);

		std::cout << "Debut d'un long speech" << std::endl;
		hr = ispVoice->Speak(L"Hello world number one", SPF_ASYNC|SPF_PURGEBEFORESPEAK, nullptr);
		hr = ispVoice->Speak(L"Hello world number two", SPF_ASYNC, nullptr);
		std::cout << "Attente de la fin du speech" << std::endl;
		hr = ispVoice->WaitUntilDone(30000);
		std::cout << "Le speech est fini" << std::endl;
		*/
		
		/*
		std::cout << "Debut d'un long speech depuis un fichier" << std::endl;
		hr = ispVoice->Speak(nullptr, SPF_PURGEBEFORESPEAK, nullptr);
		const WCHAR* chemin = L"C:\\hello_world.txt";
		hr = ispVoice->Speak(chemin, SPF_ASYNC|SPF_IS_FILENAME|SPF_IS_XML, nullptr);
		std::cout << "Attente de la fin du speech depuis un fichier" << std::endl;
		hr = ispVoice->WaitUntilDone(40000);
		std::cout << "Le speech depuis un fichier est fini" << std::endl;
		*/
        ispVoice->Release();
        ispVoice = nullptr;
    }

    ::CoUninitialize();
	
	system("PAUSE");
	return EXIT_SUCCESS;
}
