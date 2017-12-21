/*
 * Generated by asn1c-0.9.28 (http://lionet.info/asn1c)
 * From ASN.1 module "RRLP-Components"
 * 	found in "../rrlp-components.asn"
 * 	`asn1c -gen-PER`
 */

#include "OTD-MsrElementFirst.h"

static int
memb_refFrameNumber_constraint_1(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	long value;
	
	if(!sptr) {
		ASN__CTFAIL(app_key, td, sptr,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
	
	value = *(const long *)sptr;
	
	if((value >= 0 && value <= 42431)) {
		/* Constraint check succeeded */
		return 0;
	} else {
		ASN__CTFAIL(app_key, td, sptr,
			"%s: constraint failed (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
}

static int
memb_taCorrection_constraint_1(asn_TYPE_descriptor_t *td, const void *sptr,
			asn_app_constraint_failed_f *ctfailcb, void *app_key) {
	long value;
	
	if(!sptr) {
		ASN__CTFAIL(app_key, td, sptr,
			"%s: value not given (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
	
	value = *(const long *)sptr;
	
	if((value >= 0 && value <= 960)) {
		/* Constraint check succeeded */
		return 0;
	} else {
		ASN__CTFAIL(app_key, td, sptr,
			"%s: constraint failed (%s:%d)",
			td->name, __FILE__, __LINE__);
		return -1;
	}
}

static asn_per_constraints_t asn_PER_memb_refFrameNumber_constr_2 GCC_NOTUSED = {
	{ APC_CONSTRAINED,	 16,  16,  0,  42431 }	/* (0..42431) */,
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 },
	0, 0	/* No PER value map */
};
static asn_per_constraints_t asn_PER_memb_taCorrection_constr_6 GCC_NOTUSED = {
	{ APC_CONSTRAINED,	 10,  10,  0,  960 }	/* (0..960) */,
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 },
	0, 0	/* No PER value map */
};
static asn_TYPE_member_t asn_MBR_OTD_MsrElementFirst_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct OTD_MsrElementFirst, refFrameNumber),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		memb_refFrameNumber_constraint_1,
		&asn_PER_memb_refFrameNumber_constr_2,
		0,
		"refFrameNumber"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct OTD_MsrElementFirst, referenceTimeSlot),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_ModuloTimeSlot,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"referenceTimeSlot"
		},
	{ ATF_POINTER, 1, offsetof(struct OTD_MsrElementFirst, toaMeasurementsOfRef),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_TOA_MeasurementsOfRef,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"toaMeasurementsOfRef"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct OTD_MsrElementFirst, stdResolution),
		(ASN_TAG_CLASS_CONTEXT | (3 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_StdResolution,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"stdResolution"
		},
	{ ATF_POINTER, 2, offsetof(struct OTD_MsrElementFirst, taCorrection),
		(ASN_TAG_CLASS_CONTEXT | (4 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NativeInteger,
		memb_taCorrection_constraint_1,
		&asn_PER_memb_taCorrection_constr_6,
		0,
		"taCorrection"
		},
	{ ATF_POINTER, 1, offsetof(struct OTD_MsrElementFirst, otd_FirstSetMsrs),
		(ASN_TAG_CLASS_CONTEXT | (5 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_SeqOfOTD_FirstSetMsrs,
		0,	/* Defer constraints checking to the member type */
		0,	/* No PER visible constraints */
		0,
		"otd-FirstSetMsrs"
		},
};
static const int asn_MAP_OTD_MsrElementFirst_oms_1[] = { 2, 4, 5 };
static const ber_tlv_tag_t asn_DEF_OTD_MsrElementFirst_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static const asn_TYPE_tag2member_t asn_MAP_OTD_MsrElementFirst_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* refFrameNumber */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 }, /* referenceTimeSlot */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 2, 0, 0 }, /* toaMeasurementsOfRef */
    { (ASN_TAG_CLASS_CONTEXT | (3 << 2)), 3, 0, 0 }, /* stdResolution */
    { (ASN_TAG_CLASS_CONTEXT | (4 << 2)), 4, 0, 0 }, /* taCorrection */
    { (ASN_TAG_CLASS_CONTEXT | (5 << 2)), 5, 0, 0 } /* otd-FirstSetMsrs */
};
static asn_SEQUENCE_specifics_t asn_SPC_OTD_MsrElementFirst_specs_1 = {
	sizeof(struct OTD_MsrElementFirst),
	offsetof(struct OTD_MsrElementFirst, _asn_ctx),
	asn_MAP_OTD_MsrElementFirst_tag2el_1,
	6,	/* Count of tags in the map */
	asn_MAP_OTD_MsrElementFirst_oms_1,	/* Optional members */
	3, 0,	/* Root/Additions */
	-1,	/* Start extensions */
	-1	/* Stop extensions */
};
asn_TYPE_descriptor_t asn_DEF_OTD_MsrElementFirst = {
	"OTD-MsrElementFirst",
	"OTD-MsrElementFirst",
	SEQUENCE_free,
	SEQUENCE_print,
	SEQUENCE_constraint,
	SEQUENCE_decode_ber,
	SEQUENCE_encode_der,
	SEQUENCE_decode_xer,
	SEQUENCE_encode_xer,
	SEQUENCE_decode_uper,
	SEQUENCE_encode_uper,
	0,	/* Use generic outmost tag fetcher */
	asn_DEF_OTD_MsrElementFirst_tags_1,
	sizeof(asn_DEF_OTD_MsrElementFirst_tags_1)
		/sizeof(asn_DEF_OTD_MsrElementFirst_tags_1[0]), /* 1 */
	asn_DEF_OTD_MsrElementFirst_tags_1,	/* Same as above */
	sizeof(asn_DEF_OTD_MsrElementFirst_tags_1)
		/sizeof(asn_DEF_OTD_MsrElementFirst_tags_1[0]), /* 1 */
	0,	/* No PER visible constraints */
	asn_MBR_OTD_MsrElementFirst_1,
	6,	/* Elements count */
	&asn_SPC_OTD_MsrElementFirst_specs_1	/* Additional specs */
};

