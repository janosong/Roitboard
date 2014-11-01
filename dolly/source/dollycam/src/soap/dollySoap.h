/* dolly.h
   Generated by wsdl2h 1.2.9l from dolly.wsdl and typemap.dat
   2013-08-13 13:40:23 GMT
   Copyright (C) 2001-2007 Robert van Engelen, Genivia Inc. All Rights Reserved.
   This part of the software is released under one of the following licenses:
   GPL or Genivia's license for commercial use.
*/

/* NOTE:

 - Compile this file with soapcpp2 to complete the code generation process.
 - Use soapcpp2 option -I to specify paths for #import
   To build with STL, 'stlvector.h' is imported from 'import' dir in package.
 - Use wsdl2h options -c and -s to generate pure C code or C++ code without STL.
 - Use 'typemap.dat' to control schema namespace bindings and type mappings.
   It is strongly recommended to customize the names of the namespace prefixes
   generated by wsdl2h. To do so, modify the prefix bindings in the Namespaces
   section below and add the modified lines to 'typemap.dat' to rerun wsdl2h.
 - Use Doxygen (www.doxygen.org) to browse this file.
 - Use wsdl2h option -l to view the software license terms.

   DO NOT include this file directly into your project.
   Include only the soapcpp2-generated headers and source code files.
*/

//gsoapopt cw

/******************************************************************************\
 *                                                                            *
 * http://www.example.org/dolly/                                              *
 *                                                                            *
\******************************************************************************/


/******************************************************************************\
 *                                                                            *
 * Import                                                                     *
 *                                                                            *
\******************************************************************************/


/******************************************************************************\
 *                                                                            *
 * Schema Namespaces                                                          *
 *                                                                            *
\******************************************************************************/


/* NOTE:

It is strongly recommended to customize the names of the namespace prefixes
generated by wsdl2h. To do so, modify the prefix bindings below and add the
modified lines to typemap.dat to rerun wsdl2h:

ns1 = "http://www.example.org/dolly/"

*/

//gsoap ns1   schema namespace:	http://www.example.org/dolly/
//gsoap ns1   schema form:	unqualified

/******************************************************************************\
 *                                                                            *
 * Schema Types                                                               *
 *                                                                            *
\******************************************************************************/




/// Element "http://www.example.org/dolly/":sendCmd of complexType.

/// "http://www.example.org/dolly/":sendCmd is a complexType.
struct _ns1__sendCmd
{
/// Element cmd of type xs:int.
    int                                  cmd                            1;	///< Required element.
/// Element p1 of type xs:int.
    int                                  p1                             1;	///< Required element.
/// Element p2 of type xs:int.
    int                                  p2                             1;	///< Required element.
};


/// Element "http://www.example.org/dolly/":sendCmdResponse of complexType.

/// "http://www.example.org/dolly/":sendCmdResponse is a complexType.
struct _ns1__sendCmdResponse
{
/// Element retval of type xs:int.
    int                                  retval                         1;	///< Required element.
};


/// Element "http://www.example.org/dolly/":getConfig of complexType.

/// "http://www.example.org/dolly/":getConfig is a complexType.
struct _ns1__getConfig
{
/// Element p1 of type xs:int.
    int                                  p1                             1;	///< Required element.
};


/// Element "http://www.example.org/dolly/":getConfigResponse of complexType.

/// "http://www.example.org/dolly/":getConfigResponse is a complexType.
struct _ns1__getConfigResponse
{
/// Element out of type xs:string.
    char*                                out                            1;	///< Required element.
};

/******************************************************************************\
 *                                                                            *
 * Services                                                                   *
 *                                                                            *
\******************************************************************************/


//gsoap ns1  service name:	dollySOAP 
//gsoap ns1  service type:	dolly 
//gsoap ns1  service port:	http://www.example.org/ 
//gsoap ns1  service namespace:	http://www.example.org/dolly/ 
//gsoap ns1  service transport:	http://schemas.xmlsoap.org/soap/http 

/** @mainpage dolly Definitions

@section dolly_bindings Bindings
  - @ref dollySOAP

*/

/**

@page dollySOAP Binding "dollySOAP"

@section dollySOAP_operations Operations of Binding  "dollySOAP"
  - @ref __ns1__sendCmd
  - @ref __ns1__getConfig

@section dollySOAP_ports Endpoints of Binding  "dollySOAP"
  - http://www.example.org/

Note: use wsdl2h option -N to change the service binding prefix name

*/

/******************************************************************************\
 *                                                                            *
 * dollySOAP                                                                  *
 *                                                                            *
\******************************************************************************/


/******************************************************************************\
 *                                                                            *
 * __ns1__sendCmd                                                             *
 *                                                                            *
\******************************************************************************/


/// Operation "__ns1__sendCmd" of service binding "dollySOAP"

/**

Operation details:

  - SOAP document/literal style
  - SOAP action="http://www.example.org/dolly/sendCmd"

C stub function (defined in soapClient.c[pp] generated by soapcpp2):
@code
  int soap_call___ns1__sendCmd(
    struct soap *soap,
    NULL, // char *endpoint = NULL selects default endpoint for this operation
    NULL, // char *action = NULL selects default action for this operation
    // request parameters:
    struct _ns1__sendCmd*               ns1__sendCmd,
    // response parameters:
    struct _ns1__sendCmdResponse*       ns1__sendCmdResponse
  );
@endcode

C server function (called from the service dispatcher defined in soapServer.c[pp]):
@code
  int __ns1__sendCmd(
    struct soap *soap,
    // request parameters:
    struct _ns1__sendCmd*               ns1__sendCmd,
    // response parameters:
    struct _ns1__sendCmdResponse*       ns1__sendCmdResponse
  );
@endcode

*/

//gsoap ns1  service method-style:	sendCmd document
//gsoap ns1  service method-encoding:	sendCmd literal
//gsoap ns1  service method-action:	sendCmd http://www.example.org/dolly/sendCmd
int __ns1__sendCmd(
    struct _ns1__sendCmd*               ns1__sendCmd,	///< Request parameter
    struct _ns1__sendCmdResponse*       ns1__sendCmdResponse	///< Response parameter
);

/******************************************************************************\
 *                                                                            *
 * __ns1__getConfig                                                           *
 *                                                                            *
\******************************************************************************/


/// Operation "__ns1__getConfig" of service binding "dollySOAP"

/**

Operation details:

  - SOAP document/literal style
  - SOAP action="http://www.example.org/dolly/getConfig"

C stub function (defined in soapClient.c[pp] generated by soapcpp2):
@code
  int soap_call___ns1__getConfig(
    struct soap *soap,
    NULL, // char *endpoint = NULL selects default endpoint for this operation
    NULL, // char *action = NULL selects default action for this operation
    // request parameters:
    struct _ns1__getConfig*             ns1__getConfig,
    // response parameters:
    struct _ns1__getConfigResponse*     ns1__getConfigResponse
  );
@endcode

C server function (called from the service dispatcher defined in soapServer.c[pp]):
@code
  int __ns1__getConfig(
    struct soap *soap,
    // request parameters:
    struct _ns1__getConfig*             ns1__getConfig,
    // response parameters:
    struct _ns1__getConfigResponse*     ns1__getConfigResponse
  );
@endcode

*/

//gsoap ns1  service method-style:	getConfig document
//gsoap ns1  service method-encoding:	getConfig literal
//gsoap ns1  service method-action:	getConfig http://www.example.org/dolly/getConfig
int __ns1__getConfig(
    struct _ns1__getConfig*             ns1__getConfig,	///< Request parameter
    struct _ns1__getConfigResponse*     ns1__getConfigResponse	///< Response parameter
);

/* End of dolly.h */
