/*
 * Generated by asn1c-0.9.28 (http://lionet.info/asn1c)
 * From ASN.1 module "SUPL-TRIGGERED-START"
 * 	found in "../supl-triggered-start.asn"
 * 	`asn1c -fcompound-names -gen-PER`
 */

#ifndef	_GeographicTargetArea_H_
#define	_GeographicTargetArea_H_


#include <asn_application.h>

/* Including external dependencies */
#include "CircularArea.h"
#include "EllipticalArea.h"
#include "PolygonArea.h"
#include <constr_CHOICE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Dependencies */
typedef enum GeographicTargetArea_PR {
	GeographicTargetArea_PR_NOTHING,	/* No components present */
	GeographicTargetArea_PR_circularArea,
	GeographicTargetArea_PR_ellipticalArea,
	GeographicTargetArea_PR_polygonArea,
	/* Extensions may appear below */
	
} GeographicTargetArea_PR;

/* GeographicTargetArea */
typedef struct GeographicTargetArea {
	GeographicTargetArea_PR present;
	union GeographicTargetArea_u {
		CircularArea_t	 circularArea;
		EllipticalArea_t	 ellipticalArea;
		PolygonArea_t	 polygonArea;
		/*
		 * This type is extensible,
		 * possible extensions are below.
		 */
	} choice;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} GeographicTargetArea_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_GeographicTargetArea;

#ifdef __cplusplus
}
#endif

#endif	/* _GeographicTargetArea_H_ */
#include <asn_internal.h>