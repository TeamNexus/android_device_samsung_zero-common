/*
 * Generated by asn1c-0.9.28 (http://lionet.info/asn1c)
 * From ASN.1 module "Ver2-ULP-Components"
 * 	found in "../ver2-ulp-components.asn"
 * 	`asn1c -fcompound-names -gen-PER`
 */

#ifndef	_LocationData_H_
#define	_LocationData_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeInteger.h>
#include <OCTET_STRING.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* LocationData */
typedef struct LocationData {
	unsigned long	*locationAccuracy	/* OPTIONAL */;
	OCTET_STRING_t	 locationValue;
	/*
	 * This type is extensible,
	 * possible extensions are below.
	 */
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} LocationData_t;

/* Implementation */
/* extern asn_TYPE_descriptor_t asn_DEF_locationAccuracy_2;	// (Use -fall-defs-global to expose) */
extern asn_TYPE_descriptor_t asn_DEF_LocationData;

#ifdef __cplusplus
}
#endif

#endif	/* _LocationData_H_ */
#include <asn_internal.h>