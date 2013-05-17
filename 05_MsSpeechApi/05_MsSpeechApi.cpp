// 05_MsSpeechApi.cpp : Defines the entry point for the console application.
//

#include <sphelper.h>
#include <sapi.h>
#include <iostream>
#include <string>

class StreamObject : ISpStreamFormat 
{
};

int speakTest(const std::wstring& text);
int start_listening(const std::string& word);
ISpRecoGrammar* init_grammar(ISpRecoContext* recoContext, const std::string& command);
void get_text(ISpRecoContext* reco_context);
void check_result(const HRESULT& result);

int main(int argc, char** argv)
{
	speakTest(L"Waiting for you to say Hello");
	start_listening("Hello");
	speakTest(L"Hello user");

	system("PAUSE");
	return EXIT_SUCCESS;
}

int speakTest(const std::wstring& text)
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
		hr = ispVoice->Speak(text.c_str(), SPF_DEFAULT, nullptr);
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
	return EXIT_SUCCESS;
}

const ULONGLONG grammarId = 0;
const wchar_t* ruleName1 = L"ruleName1";

int start_listening(const std::string& word)
{
	// Initialize COM library
    if (FAILED(::CoInitialize(nullptr))) {
		return EXIT_FAILURE;
	}

	std::cout << "You should start Windows Recognition" << std::endl;
	std::cout << "Just say \""<< word << "\"" << std::endl;

	HRESULT hr;

	ISpRecognizer* recognizer;
	hr = CoCreateInstance(CLSID_SpSharedRecognizer,
		nullptr, CLSCTX_ALL, IID_ISpRecognizer,
		reinterpret_cast<void**>(&recognizer));
	check_result(hr);

	ISpRecoContext* recoContext;
	hr = recognizer->CreateRecoContext(&recoContext);
	check_result(hr);

	// Disable context
	hr = recoContext->Pause(0);
	check_result(hr);

	// TODO: Change static text to a parameter
	ISpRecoGrammar* recoGrammar = init_grammar(recoContext, word);

	/////////////////////////////////////////////////
	// TODO: Review this
	hr = recoContext->SetNotifyWin32Event();
	check_result(hr);

	HANDLE handleEvent;
	handleEvent = recoContext->GetNotifyEventHandle();
	if(handleEvent == INVALID_HANDLE_VALUE) {
		check_result(E_FAIL);
	}

	ULONGLONG interest;
	interest = SPFEI(SPEI_RECOGNITION);
	hr = recoContext->SetInterest(interest, interest);
	check_result(hr);

	// Activate Grammar
	hr = recoGrammar->SetRuleState(ruleName1, 0, SPRS_ACTIVE);
	check_result(hr);

	// Enable context
	hr = recoContext->Resume(0);
	check_result(hr);

	// Wait for reco
	HANDLE handles[1];
	handles[0] = handleEvent;
	WaitForMultipleObjects(1, handles, FALSE, INFINITE);
	get_text(recoContext);

	std::cout << "Hello user" << std::endl;

	recoGrammar->Release();
    ::CoUninitialize();
	
	return EXIT_SUCCESS;
}

/**
* Create and initialize the Grammar.
* Create a rule for the grammar.
* Add word to the grammar.
*/
ISpRecoGrammar* init_grammar(ISpRecoContext* recoContext, const std::string& command)
{
	HRESULT hr;
	SPSTATEHANDLE sate;

	ISpRecoGrammar* recoGrammar;
	hr = recoContext->CreateGrammar(grammarId, &recoGrammar);
	check_result(hr);

	WORD langId = MAKELANGID(LANG_FRENCH, SUBLANG_FRENCH);
	hr = recoGrammar->ResetGrammar(langId);
	check_result(hr);
	// TODO: Catch error and use default langId => GetUserDefaultUILanguage()

	// Create rules
	hr = recoGrammar->GetRule(ruleName1, 0, SPRAF_TopLevel | SPRAF_Active, true, &sate);
	check_result(hr);

	// Add a word
	const std::wstring commandWstr = std::wstring(command.begin(), command.end());
	hr = recoGrammar->AddWordTransition(sate, NULL, commandWstr.c_str(), L" ", SPWT_LEXICAL, 1, nullptr);
	check_result(hr);

	// Commit changes
	hr = recoGrammar->Commit(0);
	check_result(hr);

	return recoGrammar;
}

void get_text(ISpRecoContext* reco_context)
{
	const ULONG maxEvents = 10;
	SPEVENT events[maxEvents];

	ULONG eventCount;
	HRESULT hr;
	hr = reco_context->GetEvents(maxEvents, events, &eventCount);

	// Warning hr equal S_FALSE if everything is OK 
	// but eventCount < requestedEventCount
	if(!(hr == S_OK || hr == S_FALSE)) {
		check_result(hr);
	}

	ISpRecoResult* recoResult;
	recoResult = reinterpret_cast<ISpRecoResult*>(events[0].lParam);

	wchar_t* text;
	hr = recoResult->GetText(SP_GETWHOLEPHRASE, SP_GETWHOLEPHRASE, FALSE, &text, NULL);
	check_result(hr);

	CoTaskMemFree(text);
}

void check_result(const HRESULT& result)
{
	if (result == S_OK) {
		return;
	}

	std::string message;

	switch(result) {

	case E_INVALIDARG:
		message = "One or more arguments are invalids.";

	case E_ACCESSDENIED:
		message = "Acces Denied.";

	case E_NOINTERFACE:
		message = "Interface does not exist.";

	case E_NOTIMPL:
		message = "Not implemented method.";

	case E_OUTOFMEMORY:
		message = "Out of memory.";

	case E_POINTER:
		message = "Invalid pointer.";

	case E_UNEXPECTED:
		message = "Unexpecter error.";

	case E_FAIL:
		message = "Failure";

	default:
		message = "Unknown : " + std::to_string(result);
	}

	throw std::exception(message.c_str());
}