/*
 * Generated by asn1c-0.9.28 (http://lionet.info/asn1c)
 * From ASN.1 module "RRLP-Components"
 * 	found in "../rrlp-components.asn"
 * 	`asn1c -gen-PER`
 */

#ifndef	_GLONASSclockModel_H_
#define	_GLONASSclockModel_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeInteger.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* GLONASSclockModel */
typedef struct GLONASSclockModel {
	long	 gloTau;
	long	 gloGamma;
	long	*gloDeltaTau	/* OPTIONAL */;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} GLONASSclockModel_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_GLONASSclockModel;

#ifdef __cplusplus
}
#endif

#endif	/* _GLONASSclockModel_H_ */
#include <asn_internal.h>
