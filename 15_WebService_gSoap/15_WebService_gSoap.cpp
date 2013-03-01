// 15_WebService_gSoap.cpp : Defines the entry point for the console application.
//
#include "gsoap\stdsoap2.h"
#include "gsoap\soapStub.h" // obtain the generated stub  
#include "gsoap\GeoIPServiceSoap.nsmap" // obtain the generated XML namespace mapping table for the GeoIPServiceSoap service  

struct Namespace *v_namespaces;

int main(int argc, char** argv)
{
	struct soap soap;
	soap_init(&soap);
	/*
	_ns1__GetGeoIP geoIp;
	geoIp.IPAddress->assign("217.154.243.205");
	_ns1__GetGeoIPResponse response;

	//if (soap_call___ns1__GetGeoIP(&soap, nullptr, nullptr, )
	if (soap_call___ns1__GetGeoIP(&soap, "http://www.webservicex.net/geoipservice.asmx?WSDL", "", &geoIp, &response) == SOAP_OK) {
		std::cout << "Country : " << response.GetGeoIPResult->CountryName << std::endl; 

	} else {
		soap_print_fault(&soap, stderr); // display the SOAP fault message on the stderr stream 
	}

	soap_destroy(&soap); // delete deserialized class instances (for C++) 
	soap_end(&soap); // remove deserialized data and clean up 
	soap_done(&soap); // detach the gSOAP context
	*/

	system("PAUSE");
	return 0;
}

