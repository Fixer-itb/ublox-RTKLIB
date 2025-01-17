#ifndef RTKLIB_H
#define RTKLIB_H
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include "usart.h"

extern unsigned char ublox_raw_flag;
extern unsigned char ublox_eph_flag;

#define ENAGLO 1 // 1.允许,0.不允许
#define ENACMP 1
#define ENAQZS 1
#define ENAGAL 1
#define ENAIRN 1

#define VER_RTKLIB "2.4.2" /* library version */

#define COPYRIGHT_RTKLIB \
    "Copyright (C) 2007-2013 by T.Takasu\nAll rights reserved."

#define PI 3.1415926535897932  /* pi */
#define D2R (PI / 180.0)       /* deg to rad??????*/
#define R2D (180.0 / PI)       /* rad to deg ??????*/
#define CLIGHT 299792458.0     /* speed of light (m/s)?? */
#define SC2RAD 3.1415926535898 /* semi-circle to radian (IS-GPS)??,?? */
#define AU 149597870691.0      /* 1 AU (m) */
#define AS2R (D2R / 3600.0)    /* arc sec to radian??,?? */

#define OMGE 7.2921151467E-5 /* earth angular velocity (IS-GPS) ?????(rad/s) */

#define RE_WGS84 6378137.0             /* earth semimajor axis (WGS84) (m)?????? */
#define FE_WGS84 (1.0 / 298.257223563) /* earth flattening (WGS84) ????*/

#define HION 350000.0 /* ionosphere height (m)????? */

#define MAXFREQ 7 /* max NFREQ */

#define FREQ1 1.57542E9      /* L1/E1  frequency (Hz) */
#define FREQ2 1.22760E9      /* L2     frequency (Hz) */
#define FREQ5 1.17645E9      /* L5/E5a frequency (Hz) */
#define FREQ6 1.27875E9      /* E6/LEX frequency (Hz) */
#define FREQ7 1.20714E9      /* E5b    frequency (Hz) */
#define FREQ8 1.191795E9     /* E5a+b  frequency (Hz) */
#define FREQ9 2.492028E9     /* S      frequency (Hz) */
#define FREQ1_GLO 1.60200E9  /* GLONASS G1 base frequency (Hz) */
#define DFRQ1_GLO 0.56250E6  /* GLONASS G1 bias frequency (Hz/n) */
#define FREQ2_GLO 1.24600E9  /* GLONASS G2 base frequency (Hz) */
#define DFRQ2_GLO 0.43750E6  /* GLONASS G2 bias frequency (Hz/n) */
#define FREQ3_GLO 1.202025E9 /* GLONASS G3 frequency (Hz) */
#define FREQ1_CMP 1.561098E9 /* BeiDou B1 frequency (Hz) */
#define FREQ2_CMP 1.20714E9  /* BeiDou B2 frequency (Hz) */
#define FREQ3_CMP 1.26852E9  /* BeiDou B3 frequency (Hz) */

#define EFACT_GPS 1.0 /* error factor(????): GPS */
#define EFACT_GLO 1.5 /* error factor: GLONASS */
#define EFACT_GAL 1.0 /* error factor: Galileo */
#define EFACT_QZS 1.0 /* error factor: QZSS */
#define EFACT_CMP 1.0 /* error factor: BeiDou */
#define EFACT_SBS 3.0 /* error factor: SBAS */

#define SYS_NONE 0x00 /* navigation system: none */
#define SYS_GPS 0x01  /* navigation system: GPS */
#define SYS_SBS 0x02  /* navigation system: SBAS */
#define SYS_GLO 0x04  /* navigation system: GLONASS */
#define SYS_GAL 0x08  /* navigation system: Galileo */
#define SYS_QZS 0x10  /* navigation system: QZSS */
#define SYS_CMP 0x20  /* navigation system: BeiDou */
#define SYS_IRN 0x40  /* navigation system: IRNS */
#define SYS_LEO 0x80  /* navigation system: LEO */
#define SYS_ALL 0xFF  /* navigation system: all */

#define TSYS_GPS 0 /* time system(???,????): GPS time */
#define TSYS_UTC 1 /* time system: UTC */
#define TSYS_GLO 2 /* time system: GLONASS time */
#define TSYS_GAL 3 /* time system: Galileo time */
#define TSYS_QZS 4 /* time system: QZSS time */
#define TSYS_CMP 5 /* time system: BeiDou time */

#ifndef NFREQ
#define NFREQ 3 /* number of carrier frequencies???? */
#endif
#define NFREQGLO 2 /* number of carrier frequencies of GLONASS ?????*/

#ifndef NEXOBS
#define NEXOBS 0 /* number of extended obs codes ??????*/
#endif

#define MINPRNGPS 1                         /* min satellite PRN number of GPS?????????? */
#define MAXPRNGPS 32                        /* max satellite PRN number of GPS ??????????*/
#define NSATGPS (MAXPRNGPS - MINPRNGPS + 1) /* number of GPS satellites */
#define NSYSGPS 1

#if ENAGLO
#define MINPRNGLO 1                         /* min satellite slot number of GLONASS */
#define MAXPRNGLO 24                        /* max satellite slot number of GLONASS */
#define NSATGLO (MAXPRNGLO - MINPRNGLO + 1) /* number of GLONASS satellites */
#define NSYSGLO 1
#else
#define MINPRNGLO 0
#define MAXPRNGLO 0
#define NSATGLO 0
#define NSYSGLO 0
#endif
#if ENAGAL
#define MINPRNGAL 1                         /* min satellite PRN number of Galileo */
#define MAXPRNGAL 27                        /* max satellite PRN number of Galileo */
#define NSATGAL (MAXPRNGAL - MINPRNGAL + 1) /* number of Galileo satellites */
#define NSYSGAL 1
#else
#define MINPRNGAL 0
#define MAXPRNGAL 0
#define NSATGAL 0
#define NSYSGAL 0
#endif
#if ENAQZS
#define MINPRNQZS 193                       /* min satellite PRN number of QZSS */
#define MAXPRNQZS 195                       /* max satellite PRN number of QZSS */
#define MINPRNQZS_S 183                     /* min satellite PRN number of QZSS SAIF */
#define MAXPRNQZS_S 185                     /* max satellite PRN number of QZSS SAIF */
#define NSATQZS (MAXPRNQZS - MINPRNQZS + 1) /* number of QZSS satellites */
#define NSYSQZS 1
#else
#define MINPRNQZS 0
#define MAXPRNQZS 0
#define NSATQZS 0
#define NSYSQZS 0
#endif
#if ENACMP
#define MINPRNCMP 1                         /* min satellite sat number of BeiDou */
#define MAXPRNCMP 35                        /* max satellite sat number of BeiDou */
#define NSATCMP (MAXPRNCMP - MINPRNCMP + 1) /* number of BeiDou satellites */
#define NSYSCMP 1
#else
#define MINPRNCMP 0
#define MAXPRNCMP 0
#define NSATCMP 0
#define NSYSCMP 0
#endif
#ifdef ENAIRN
#define MINPRNIRN 1                         /* min satellite sat number of IRNSS */
#define MAXPRNIRN 7                         /* max satellite sat number of IRNSS */
#define NSATIRN (MAXPRNIRN - MINPRNIRN + 1) /* number of IRNSS satellites */
#define NSYSIRN 1
#endif
#define NSYS (NSYSGPS + NSYSGLO + NSYSGAL + NSYSQZS + NSYSCMP) /* number of systems */

#define MINPRNSBS 120                       /* min satellite PRN number of SBAS */
#define MAXPRNSBS 142                       /* max satellite PRN number of SBAS */
#define NSATSBS (MAXPRNSBS - MINPRNSBS + 1) /* number of SBAS satellites */

#define MAXSAT (NSATGPS + NSATGLO + NSATGAL + NSATQZS + NSATCMP + NSATSBS)
/* max satellite number (1 to MAXSAT),??????? */
#ifndef MAXOBS
#define MAXOBS 64 /* max number of obs in an epoch????????? */
#endif
#define MAXRCV 64     /* max receiver number (1 to MAXRCV)?????? */
#define MAXOBSTYPE 64 /* max number of obs type in RINEX ??????????*/
#define DTTOL 0.005   /* tolerance of time difference (s)???? */
#if 0
#define MAXDTOE 10800.0 /* max time difference to ephem Toe (s) for GPS */
#else
#define MAXDTOE 7200.0 /* max time difference to ephem Toe (s) for GPS???????? */
#endif
#define MAXDTOE 7200.0      /* max time difference to GPS Toe (s) */
#define MAXDTOE_QZS 7200.0  /* max time difference to QZSS Toe (s) */
#define MAXDTOE_GAL 10800.0 /* max time difference to Galileo Toe (s) */
#define MAXDTOE_CMP 21600.0 /* max time difference to BeiDou Toe (s) */
#define MAXDTOE_GLO 1800.0  /* max time difference to GLONASS Toe (s) */
#define MAXDTOE_SBS 360.0   /* max time difference to SBAS Toe (s) */
#define MAXDTOE_S 86400.0   /* max time difference to ephem toe (s) for other */
#define MAXGDOP 300.0       /* max GDOP */

#define MAXEXFILE 100     /* max number of expanded files???????? */
#define MAXSBSAGEF 30.0   /* max age of SBAS fast correction (s)SBS?????????? */
#define MAXSBSAGEL 1800.0 /* max age of SBAS long term corr (s) SBAS?????????????*/
#define MAXSBSURA 8       /* max URA of SBAS satelliteSBAS???????(URA) */
#define MAXBAND 10        /* max SBAS band of IGP  SBAR????????????*/
#define MAXNIGP 201       /* max number of IGP in SBAS band */
#define MAXNGEO 4         /* max number of GEO satellites */
#define MAXCOMMENT 10     /* max number of RINEX comments */
#define MAXSTRPATH 1024   /* max length of stream path */
#define MAXSTRMSG 1024    /* max length of stream message */
#define MAXSTRRTK 8       /* max number of stream in RTK server */
#define MAXSBSMSG 32      /* max number of SBAS msg in RTK server */
#define MAXSOLMSG 4096    /* max length of solution message */
#define MAXRAWLEN 4096    /* max length of receiver raw message */
#define MAXERRMSG 4096    /* max length of error/warning message ??/?????????*/
#define MAXANT 64         /* max length of station name/antenna type???/????????? */
#define MAXSOLBUF 256     /* max number of solution buffer?????????? */
#define MAXOBSBUF 128     /* max number of observation data buffer?????????? */
#define MAXNRPOS 16       /* max number of reference positions???????? */
#define MAXLEAPS 64       /* max number of leap seconds table */

#define RNX2VER 2.10 /* RINEX ver.2 default output version */
#define RNX3VER 3.00 /* RINEX ver.3 default output version */

#define OBSTYPE_PR 0x01  /* observation type: pseudorange */
#define OBSTYPE_CP 0x02  /* observation type: carrier-phase */
#define OBSTYPE_DOP 0x04 /* observation type: doppler-freq */
#define OBSTYPE_SNR 0x08 /* observation type: SNR */
#define OBSTYPE_ALL 0xFF /* observation type: all */

#define FREQTYPE_L1 0x01  /* frequency type: L1/E1 */
#define FREQTYPE_L2 0x02  /* frequency type: L2/B1 */
#define FREQTYPE_L5 0x04  /* frequency type: L5/E5a/L3 */
#define FREQTYPE_L6 0x08  /* frequency type: E6/LEX/B3 */
#define FREQTYPE_L7 0x10  /* frequency type: E5b/B2 */
#define FREQTYPE_L8 0x20  /* frequency type: E5(a+b) */
#define FREQTYPE_ALL 0xFF /* frequency type: all */

#define CODE_NONE 0 /* obs code: none or unknown */
#define CODE_L1C 1  /* obs code: L1C/A,G1C/A,E1C (GPS,GLO,GAL,QZS,SBS) */
#define CODE_L1P 2  /* obs code: L1P,G1P    (GPS,GLO) */
#define CODE_L1W 3  /* obs code: L1 Z-track (GPS) */
#define CODE_L1Y 4  /* obs code: L1Y        (GPS) */
#define CODE_L1M 5  /* obs code: L1M        (GPS) */
#define CODE_L1N 6  /* obs code: L1codeless (GPS) */
#define CODE_L1S 7  /* obs code: L1C(D)     (GPS,QZS) */
#define CODE_L1L 8  /* obs code: L1C(P)     (GPS,QZS) */
#define CODE_L1E 9  /* (not used) */
#define CODE_L1A 10 /* obs code: E1A        (GAL) */
#define CODE_L1B 11 /* obs code: E1B        (GAL) */
#define CODE_L1X 12 /* obs code: E1B+C,L1C(D+P) (GAL,QZS) */
#define CODE_L1Z 13 /* obs code: E1A+B+C,L1SAIF (GAL,QZS) */
#define CODE_L2C 14 /* obs code: L2C/A,G1C/A (GPS,GLO) */
#define CODE_L2D 15 /* obs code: L2 L1C/A-(P2-P1) (GPS) */
#define CODE_L2S 16 /* obs code: L2C(M)     (GPS,QZS) */
#define CODE_L2L 17 /* obs code: L2C(L)     (GPS,QZS) */
#define CODE_L2X 18 /* obs code: L2C(M+L),B1I+Q (GPS,QZS,CMP) */
#define CODE_L2P 19 /* obs code: L2P,G2P    (GPS,GLO) */
#define CODE_L2W 20 /* obs code: L2 Z-track (GPS) */
#define CODE_L2Y 21 /* obs code: L2Y        (GPS) */
#define CODE_L2M 22 /* obs code: L2M        (GPS) */
#define CODE_L2N 23 /* obs code: L2codeless (GPS) */
#define CODE_L5I 24 /* obs code: L5/E5aI    (GPS,GAL,QZS,SBS) */
#define CODE_L5Q 25 /* obs code: L5/E5aQ    (GPS,GAL,QZS,SBS) */
#define CODE_L5X 26 /* obs code: L5/E5aI+Q/L5B+C (GPS,GAL,QZS,IRN,SBS) */
#define CODE_L7I 27 /* obs code: E5bI,B2I   (GAL,CMP) */
#define CODE_L7Q 28 /* obs code: E5bQ,B2Q   (GAL,CMP) */
#define CODE_L7X 29 /* obs code: E5bI+Q,B2I+Q (GAL,CMP) */
#define CODE_L6A 30 /* obs code: E6A        (GAL) */
#define CODE_L6B 31 /* obs code: E6B        (GAL) */
#define CODE_L6C 32 /* obs code: E6C        (GAL) */
#define CODE_L6X 33 /* obs code: E6B+C,LEXS+L,B3I+Q (GAL,QZS,CMP) */
#define CODE_L6Z 34 /* obs code: E6A+B+C    (GAL) */
#define CODE_L6S 35 /* obs code: LEXS       (QZS) */
#define CODE_L6L 36 /* obs code: LEXL       (QZS) */
#define CODE_L8I 37 /* obs code: E5(a+b)I   (GAL) */
#define CODE_L8Q 38 /* obs code: E5(a+b)Q   (GAL) */
#define CODE_L8X 39 /* obs code: E5(a+b)I+Q (GAL) */
#define CODE_L2I 40 /* obs code: B1I        (BDS) */
#define CODE_L2Q 41 /* obs code: B1Q        (BDS) */
#define CODE_L6I 42 /* obs code: B3I        (BDS) */
#define CODE_L6Q 43 /* obs code: B3Q        (BDS) */
#define CODE_L3I 44 /* obs code: G3I        (GLO) */
#define CODE_L3Q 45 /* obs code: G3Q        (GLO) */
#define CODE_L3X 46 /* obs code: G3I+Q      (GLO) */
#define CODE_L1I 47 /* obs code: B1I        (BDS) */
#define CODE_L1Q 48 /* obs code: B1Q        (BDS) */
#define CODE_L5A 49 /* obs code: L5A SPS    (IRN) */
#define CODE_L5B 50 /* obs code: L5B RS(D)  (IRN) */
#define CODE_L5C 51 /* obs code: L5C RS(P)  (IRN) */
#define CODE_L9A 52 /* obs code: SA SPS     (IRN) */
#define CODE_L9B 53 /* obs code: SB RS(D)   (IRN) */
#define CODE_L9C 54 /* obs code: SC RS(P)   (IRN) */
#define CODE_L9X 55 /* obs code: SB+C       (IRN) */
#define MAXCODE 55  /* max number of obs code */

#define PMODE_SINGLE 0     /* positioning mode: single */
#define PMODE_DGPS 1       /* positioning mode: DGPS/DGNSS */
#define PMODE_KINEMA 2     /* positioning mode: kinematic */
#define PMODE_STATIC 3     /* positioning mode: static */
#define PMODE_MOVEB 4      /* positioning mode: moving-base */
#define PMODE_FIXED 5      /* positioning mode: fixed */
#define PMODE_PPP_KINEMA 6 /* positioning mode: PPP-kinemaric */
#define PMODE_PPP_STATIC 7 /* positioning mode: PPP-static */
#define PMODE_PPP_FIXED 8  /* positioning mode: PPP-fixed */

#define SOLF_LLH 0  /* solution format: lat/lon/height */
#define SOLF_XYZ 1  /* solution format: x/y/z-ecef */
#define SOLF_ENU 2  /* solution format: e/n/u-baseline */
#define SOLF_NMEA 3 /* solution format: NMEA-183 */
#define SOLF_GSIF 4 /* solution format: GSI-F1/2/3 */

#define SOLQ_NONE 0   /* solution status: no solution */
#define SOLQ_FIX 1    /* solution status: fix */
#define SOLQ_FLOAT 2  /* solution status: float */
#define SOLQ_SBAS 3   /* solution status: SBAS */
#define SOLQ_DGPS 4   /* solution status: DGPS/DGNSS */
#define SOLQ_SINGLE 5 /* solution status: single */
#define SOLQ_PPP 6    /* solution status: PPP */
#define SOLQ_DR 7     /* solution status: dead reconing???? */
#define MAXSOLQ 7     /* max number of solution status */

#define TIMES_GPST 0 /* time system: gps time */
#define TIMES_UTC 1  /* time system: utc */
#define TIMES_JST 2  /* time system: jst */

#define IONOOPT_OFF 0  /* ionosphere option?????: correction off??? */
#define IONOOPT_BRDC 1 /* ionosphere option: broadcast model ???? */
#define IONOOPT_SBAS 2 /* ionosphere option: SBAS model */
#define IONOOPT_IFLC 3 /* ionosphere option: L1/L2 or L1/L5 iono-free???? LC */
#define IONOOPT_EST 4  /* ionosphere option: estimation ??*/
#define IONOOPT_TEC 5  /* ionosphere option: IONEX TEC model */
#define IONOOPT_QZS 6  /* ionosphere option: QZSS broadcast model ??*/
#define IONOOPT_LEX 7  /* ionosphere option: QZSS LEX ionospehre ???*/
#define IONOOPT_STEC 8 /* ionosphere option: SLANT TEC model */

#define TROPOPT_OFF 0  /* troposphere option???: correction off??? */
#define TROPOPT_SAAS 1 /* troposphere option: Saastamoinen model?? */
#define TROPOPT_SBAS 2 /* troposphere option: SBAS model */
#define TROPOPT_EST 3  /* troposphere option: ZTD estimation?? */
#define TROPOPT_ESTG 4 /* troposphere option: ZTD+grad estimation ??*/
#define TROPOPT_COR 5  /* troposphere option: ZTD correction?? */
#define TROPOPT_CORG 6 /* troposphere option: ZTD+grad correction?? */

#define EPHOPT_BRDC 0   /* ephemeris option: broadcast ephemeris????? */
#define EPHOPT_PREC 1   /* ephemeris option: precise ephemeris????? */
#define EPHOPT_SBAS 2   /* ephemeris option: broadcast + SBAS */
#define EPHOPT_SSRAPC 3 /* ephemeris option: broadcast + SSR_APC */
#define EPHOPT_SSRCOM 4 /* ephemeris option: broadcast + SSR_COM */
#define EPHOPT_LEX 5    /* ephemeris option: QZSS LEX ephemeris */

#define ARMODE_OFF 0       /* AR mode: off */
#define ARMODE_CONT 1      /* AR mode: continuous?? */
#define ARMODE_INST 2      /* AR mode: instantaneous ??*/
#define ARMODE_FIXHOLD 3   /* AR mode: fix and hold?? */
#define ARMODE_PPPAR 4     /* AR mode: PPP-AR */
#define ARMODE_PPPAR_ILS 5 /* AR mode: PPP-AR ILS */
#define ARMODE_WLNL 6      /* AR mode: wide lane/narrow lane??/?? */
#define ARMODE_TCAR 7      /* AR mode: triple carrier ar????AR */

#define SBSOPT_LCORR 1 /* SBAS option: long term correction????? */
#define SBSOPT_FCORR 2 /* SBAS option: fast correction????? */
#define SBSOPT_ICORR 4 /* SBAS option: ionosphere correction?????? */
#define SBSOPT_RANGE 8 /* SBAS option: ranging???? */

#define POSOPT_POS 0    /* pos option: LLH/XYZ */
#define POSOPT_SINGLE 1 /* pos option: average of single pos */
#define POSOPT_FILE 2   /* pos option: read from pos file */
#define POSOPT_RINEX 3  /* pos option: rinex header pos */
#define POSOPT_RTCM 4   /* pos option: rtcm station pos */
#define POSOPT_RAW 5    /* pos option: raw station pos */

#define STR_NONE 0     /* stream type: none */
#define STR_SERIAL 1   /* stream type: serial */
#define STR_FILE 2     /* stream type: file */
#define STR_TCPSVR 3   /* stream type: TCP server */
#define STR_TCPCLI 4   /* stream type: TCP client */
#define STR_UDP 5      /* stream type: UDP stream */
#define STR_NTRIPSVR 6 /* stream type: NTRIP server */
#define STR_NTRIPCLI 7 /* stream type: NTRIP client */
#define STR_FTP 8      /* stream type: ftp */
#define STR_HTTP 9     /* stream type: http */

#define STRFMT_RTCM2 0   /* stream format: RTCM 2 */
#define STRFMT_RTCM3 1   /* stream format: RTCM 3 */
#define STRFMT_OEM4 2    /* stream format: NovAtel OEMV/4 */
#define STRFMT_OEM3 3    /* stream format: NovAtel OEM3 */
#define STRFMT_UBX 4     /* stream format: u-blox LEA-*T */
#define STRFMT_SS2 5     /* stream format: NovAtel Superstar II */
#define STRFMT_CRES 6    /* stream format: Hemisphere */
#define STRFMT_STQ 7     /* stream format: SkyTraq S1315F */
#define STRFMT_GW10 8    /* stream format: Furuno GW10 */
#define STRFMT_JAVAD 9   /* stream format: JAVAD GRIL/GREIS */
#define STRFMT_NVS 10    /* stream format: NVS NVC08C */
#define STRFMT_BINEX 11  /* stream format: BINEX */
#define STRFMT_LEXR 12   /* stream format: Furuno LPY-10000 */
#define STRFMT_SIRF 13   /* stream format: SiRF    (reserved) */
#define STRFMT_RINEX 14  /* stream format: RINEX */
#define STRFMT_SP3 15    /* stream format: SP3 */
#define STRFMT_RNXCLK 16 /* stream format: RINEX CLK */
#define STRFMT_SBAS 17   /* stream format: SBAS messages */
#define STRFMT_NMEA 18   /* stream format: NMEA 0183 */
#ifndef EXTLEX
#define MAXRCVFMT 11 /* max number of receiver format????????? */
#else
#define MAXRCVFMT 12
#endif

#define STR_MODE_R 0x1  /* stream mode: read */
#define STR_MODE_W 0x2  /* stream mode: write */
#define STR_MODE_RW 0x3 /* stream mode: read/write */

#define GEOID_EMBEDDED 0    /* geoid model: embedded geoid */
#define GEOID_EGM96_M150 1  /* geoid model: EGM96 15x15" */
#define GEOID_EGM2008_M25 2 /* geoid model: EGM2008 2.5x2.5" */
#define GEOID_EGM2008_M10 3 /* geoid model: EGM2008 1.0x1.0" */
#define GEOID_GSI2000_M15 4 /* geoid model: GSI geoid 2000 1.0x1.5" */
#define GEOID_RAF09 5       /* geoid model: IGN RAF09 for France 1.5"x2" */

#define COMMENTH "%"                   /* comment line indicator for solution????????? */
#define MSG_DISCONN "$_DISCONNECT\r\n" /* disconnect message??????? */

#define DLOPT_FORCE 0x01   /* download option????: force download existing?????? */
#define DLOPT_KEEPCMP 0x02 /* download option: keep compressed file?????? */
#define DLOPT_HOLDERR 0x04 /* download option: hold on error file ???????*/
#define DLOPT_HOLDLST 0x08 /* download option: hold on listing file ??????*/

#define LLI_SLIP 0x01   /* LLI: cycle-slip */
#define LLI_HALFC 0x02  /* LLI: half-cycle not resovled */
#define LLI_BOCTRK 0x04 /* LLI: boc tracking of mboc signal */
#define LLI_HALFA 0x40  /* LLI: half-cycle added */
#define LLI_HALFS 0x80  /* LLI: half-cycle subtracted */

#define P2_5 0.03125                /* 2^-5 */
#define P2_6 0.015625               /* 2^-6 */
#define P2_11 4.882812500000000E-04 /* 2^-11 */
#define P2_15 3.051757812500000E-05 /* 2^-15 */
#define P2_17 7.629394531250000E-06 /* 2^-17 */
#define P2_19 1.907348632812500E-06 /* 2^-19 */
#define P2_20 9.536743164062500E-07 /* 2^-20 */
#define P2_21 4.768371582031250E-07 /* 2^-21 */
#define P2_23 1.192092895507810E-07 /* 2^-23 */
#define P2_24 5.960464477539063E-08 /* 2^-24 */
#define P2_27 7.450580596923828E-09 /* 2^-27 */
#define P2_29 1.862645149230957E-09 /* 2^-29 */
#define P2_30 9.313225746154785E-10 /* 2^-30 */
#define P2_31 4.656612873077393E-10 /* 2^-31 */
#define P2_32 2.328306436538696E-10 /* 2^-32 */
#define P2_33 1.164153218269348E-10 /* 2^-33 */
#define P2_35 2.910383045673370E-11 /* 2^-35 */
#define P2_38 3.637978807091710E-12 /* 2^-38 */
#define P2_39 1.818989403545856E-12 /* 2^-39 */
#define P2_40 9.094947017729280E-13 /* 2^-40 */
#define P2_43 1.136868377216160E-13 /* 2^-43 */
#define P2_48 3.552713678800501E-15 /* 2^-48 */
#define P2_50 8.881784197001252E-16 /* 2^-50 */
#define P2_55 2.775557561562891E-17 /* 2^-55 */

#define LEXFRMLEN 2000           /* lex frame length (bits) */
#define LEXRSLEN 256             /* lex reed solomon length (bits) */
#define LEXFRMPREAMB 0x1ACFFC1Du /* lex frame preamble */
#define LEXEPHMAXAGE 360.0       /* max age of lex ephemeris (s) */
#define LEXIONMAXAGE 3600.0      /* max age of lex ionos correction (s) */
#define RTCM2PREAMB 0x66         /* rtcm ver.2 frame preamble */
#define RTCM3PREAMB 0xD3         /* rtcm ver.3 frame preamble */

#define LEXHEADLEN 24           /* lex binary header length (bytes) */
extern const double lam_carr[]; /* carrier wave length (m) {L1,L2,...} */

/*RTKLIB内以GPST进行计算处理，其它时间系统的数据需转换为GPST数据才能进行计算。
gtime_t只是一种时间表示的形式，来表示GPST、UCT、BDT、GST*/
typedef struct
{               /* time struct */
    long time;  /* time (s) expressed by standard time_t */
    double sec; /* fraction of second under 1 s*/
} gtime_t;

typedef struct
{                          /* SNR mask type */
    int ena[2];            /* enable flag {rover,base} */
    double mask[NFREQ][9]; /* mask (dBHz) at 5,10,...85 deg */
} snrmask_t;
typedef struct
{                                       /* observation data record?????? */
    gtime_t time;                       /* receiver sampling time (GPST)??????? */
    unsigned char sat, rcv;             /* satellite/receiver number??/????? */
    unsigned char SNR[NFREQ + NEXOBS];  /* signal strength (0.25 dBHz)???? */
    unsigned char LLI[NFREQ + NEXOBS];  /* loss of lock indicator???????? */
    unsigned char code[NFREQ + NEXOBS]; /* code indicator (CODE_???)????? */
    double L[NFREQ + NEXOBS];           /* observation data carrier-phase (cycle)???????? */
    double P[NFREQ + NEXOBS];           /* observation data pseudorange (m)?????? */
    float D[NFREQ + NEXOBS];            /* observation data doppler frequency (Hz)????????? */
} obsd_t;

typedef struct
{                            /* observation data ????*/
    int n, nmax;             /* number of obervation data/allocated????/?? */
    obsd_t data[MAXOBS * 2]; /* observation data records?????? */
} obs_t;

typedef struct
{                       /* processing options type */
    int mode;           /* positioning mode (PMODE_???) */
    int soltype;        /* solution type (0:forward,1:backward,2:combined) */
    int nf;             /* number of frequencies (1:L1,2:L1+L2,3:L1+L2+L5) */
    int navsys;         /* navigation system */
    double elmin;       /* elevation mask angle (rad) */
    snrmask_t snrmask;  /* SNR mask */
    int sateph;         /* satellite ephemeris/clock (EPHOPT_???) */
    int modear;         /* AR mode (0:off,1:continuous,2:instantaneous,3:fix and hold,4:ppp-ar) */
    int glomodear;      /* GLONASS AR mode (0:off,1:on,2:auto cal,3:ext cal) */
    int maxout;         /* obs outage count to reset bias */
    int minlock;        /* min lock count to fix ambiguity */
    int minfix;         /* min fix count to hold ambiguity */
    int ionoopt;        /* ionosphere option (IONOOPT_???) */
    int tropopt;        /* troposphere option (TROPOPT_???) */
    int dynamics;       /* dynamics model (0:none,1:velociy,2:accel) */
    int tidecorr;       /* earth tide correction (0:off,1:solid,2:solid+otl+pole) */
    int niter;          /* number of filter iteration */
    int codesmooth;     /* code smoothing window size (0:none) */
    int intpref;        /* interpolate reference obs (for post mission) */
    int sbascorr;       /* SBAS correction options */
    int sbassatsel;     /* SBAS satellite selection (0:all) */
    int rovpos;         /* rover position for fixed mode */
    int refpos;         /* base position for relative mode */
                        /* (0:pos in prcopt,  1:average of single pos, */
                        /*  2:read from file, 3:rinex header, 4:rtcm pos) */
    double eratio;      /* code/phase error ratio */
    double err[5];      /* measurement error factor */
                        /* [0]:reserved */
                        /* [1-3]:error factor a/b/c of phase (m) */
                        /* [4]:doppler frequency (hz) */
    double std[3];      /* initial-state std [0]bias,[1]iono [2]trop */
    double prn[5];      /* process-noise std [0]bias,[1]iono [2]trop [3]acch [4]accv */
    double sclkstab;    /* satellite clock stability (sec/sec) */
    double thresar[4];  /* AR validation threshold */
    double elmaskar;    /* elevation mask of AR for rising satellite (deg) */
    double elmaskhold;  /* elevation mask to hold ambiguity (deg) */
    double thresslip;   /* slip threshold of geometry-free phase (m) */
    double maxtdiff;    /* max difference of time (sec) */
    double maxinno;     /* reject threshold of innovation (m) */
    double maxgdop;     /* reject threshold of gdop */
    double baseline[2]; /* baseline length constraint {const,sigma} (m) */
    double ru[3];       /* rover position for fixed mode {x,y,z} (ecef) (m) */
    double rb[3];       /* base position for relative mode {x,y,z} (ecef) (m) */
    //    char anttype[2][MAXANT]; /* antenna types {rover,base} */
    //    double antdel[2][3]; /* antenna delta {{rov_e,rov_n,rov_u},{ref_e,ref_n,ref_u}} */
    //    pcv_t pcvr[2];      /* receiver antenna parameters {rov,base} */
    //    unsigned char exsats[MAXSAT]; /* excluded satellites (1:excluded,2:included) */
    //    char rnxopt[2][256]; /* rinex options {rover,base} */
    //    int  posopt[6];     /* positioning options */
    //    int  syncsol;       /* solution sync mode (0:off,1:on) */
    //    double odisp[2][6*11]; /* ocean tide loading parameters {rov,base} */
    //    exterr_t exterr;    /* extended receiver error model */
} prcopt_t;

typedef struct
{                /* almanac type?????? */
    int sat;     /* satellite number???? */
    int svh;     /* sv health (????)????(0:ok) */
    int svconf;  /* as and sv config???? */
    int week;    /* GPS/QZS: gps week, GAL: galileo week */
    gtime_t toa; /* Toa */
                 /* SV orbit parameters(??????)???? */
    double A, e, i0, OMG0, omg, M0, OMGd;
    double toas;   /* Toa (s) in week */
    double f0, f1; /* SV clock parameters (af0,af1)???? */
} alm_t;
typedef struct
{                          /* SBAS message type */
    int week, tow;         /* receiption time */
    int prn;               /* SBAS satellite PRN number */
    unsigned char msg[29]; /* SBAS message (226bit) padded by 0 */
} sbsmsg_t;

typedef struct
{                      /* GLONASS broadcast ephemeris type ??????*/
    int sat;           /* satellite number ????*/
    int iode;          /* IODE (0-6 bit of tb field) */
    int frq;           /* satellite frequency number????? */
    int svh, sva, age; /* satellite health, accuracy, age of operation???????????? */
    gtime_t toe;       /* epoch of epherides (gpst) ????*/
    gtime_t tof;       /* message frame time (gpst)????? */
    double pos[3];     /* satellite position (ecef) (m)???? */
    double vel[3];     /* satellite velocity (ecef) (m/s)?????? */
    double acc[3];     /* satellite acceleration (ecef) (m/s^2)????? */
    double taun, gamn; /* SV clock bias (s)/relative freq bias ??????(s)/??????*/
    double dtaun;      /* delay between L1 and L2 (s)?? */
} geph_t;

typedef struct
{                           /* QZSS LEX message type */
    int prn;                /* satellite PRN number */
    int type;               /* message type */
    int alert;              /* alert flag */
    unsigned char stat;     /* signal tracking status */
    unsigned char snr;      /* signal C/N0 (0.25 dBHz) */
    unsigned int ttt;       /* tracking time (ms) */
    unsigned char msg[212]; /* LEX message data part 1695 bits */
} lexmsg_t;

typedef struct
{                          /* GPS/QZS/GAL broadcast ephemeris type ??????*/
    int sat;               /* satellite number */
    int iode, iodc;        /* IODE,IODC */
    int sva;               /* SV accuracy (URA index)SV??? */
    int svh;               /* SV health (0:ok) */
    int week;              /* GPS/QZS: gps week, GAL: galileo week */
    int code;              /* GPS/QZS: code on L2, GAL/CMP: data sources ???*/
    int flag;              /* GPS/QZS: L2 P data flag????, CMP: nav type???? */
    gtime_t toe, toc, ttr; /* Toe,Toc,T_trans */
                           /* SV orbit parameters???? */
    double A, e, i0, OMG0, omg, M0, deln, OMGd, idot;
    double crc, crs, cuc, cus, cic, cis;
    double toes;       /* Toe (s) in week */
    double fit;        /* fit interval (h)??????? */
    double f0, f1, f2; /* SV clock parameters (af0,af1,af2)???? */
    double tgd[4];     /* group delay parameters */
                       /* GPS/QZS:tgd[0]=TGD */
                       /* GAL    :tgd[0]=BGD E5a/E1,tgd[1]=BGD E5b/E1 */
                       /* CMP    :tgd[0]=BGD1,tgd[1]=BGD2 */
} eph_t;
typedef struct
{                    /* SBAS ephemeris type ?????*/
    int sat;         /* satellite number */
    gtime_t t0;      /* reference epoch time (GPST)?????? */
    gtime_t tof;     /* time of message frame (GPST)????? */
    int sva;         /* SV accuracy (URA index)???? */
    int svh;         /* SV health (0:ok)????? */
    double pos[3];   /* satellite position (m) (ecef)???? */
    double vel[3];   /* satellite velocity (m/s) (ecef)?????? */
    double acc[3];   /* satellite acceleration (m/s^2) (ecef)????? */
    double af0, af1; /* satellite clock-offset/drift (s,s/s)??????/?? */
} seph_t;

typedef struct
{                                /* navigation data type */
    int n, nmax;                 /* number of broadcast ephemeris */
    int ng, ngmax;               /* number of glonass ephemeris */
    int ns, nsmax;               /* number of sbas ephemeris */
    int ne, nemax;               /* number of precise ephemeris */
    int nc, ncmax;               /* number of precise clock */
    int na, namax;               /* number of almanac data */
    int nt, ntmax;               /* number of tec grid data */
    int nn, nnmax;               /* number of stec grid data */
    eph_t eph[MAXSAT];           /* GPS/QZS/GAL ephemeris */
    geph_t geph[NSATGLO];        /* GLONASS ephemeris */
    seph_t seph[NSATSBS * 2];    /* SBAS ephemeris */
                                 //    peph_t *peph;       /* precise ephemeris */
                                 //    pclk_t *pclk;       /* precise clock */
    alm_t alm[MAXSAT];           /* almanac data */
                                 //    tec_t *tec;         /* tec grid data */
                                 //    stec_t *stec;       /* stec grid data */
                                 //    erp_t  erp;         /* earth rotation parameters */
    double utc_gps[4];           /* GPS delta-UTC parameters {A0,A1,T,W} */
    double utc_glo[4];           /* GLONASS UTC GPS time parameters */
    double utc_gal[4];           /* Galileo UTC GPS time parameters */
    double utc_qzs[4];           /* QZS UTC GPS time parameters */
    double utc_cmp[4];           /* BeiDou UTC parameters */
    double utc_sbs[4];           /* SBAS UTC parameters */
    double ion_gps[8];           /* GPS iono model parameters {a0,a1,a2,a3,b0,b1,b2,b3} */
    double ion_gal[4];           /* Galileo iono model parameters {ai0,ai1,ai2,0} */
    double ion_qzs[8];           /* QZSS iono model parameters {a0,a1,a2,a3,b0,b1,b2,b3} */
    double ion_cmp[8];           /* BeiDou iono model parameters {a0,a1,a2,a3,b0,b1,b2,b3} */
    int leaps;                   /* leap seconds (s) */
    double lam[MAXSAT][NFREQ];   /* carrier wave lengths (m) */
    double cbias[MAXSAT][3];     /* code bias (0:p1-p2,1:p1-c1,2:p2-c2) (m) */
    double wlbias[MAXSAT];       /* wide-lane bias (cycle) */
    double glo_cpbias[4];        /* glonass code-phase bias {1C,1P,2C,2P} (m) */
    char glo_fcn[MAXPRNGLO + 1]; /* glonass frequency channel number + 8 */
    //    pcv_t pcvs[MAXSAT]; /* satellite antenna pcv */
    //    sbssat_t sbssat;    /* SBAS satellite corrections */
    //    sbsion_t sbsion[MAXBAND+1]; /* SBAS ionosphere corrections */
    //    dgps_t dgps[MAXSAT]; /* DGPS corrections */
    //    ssr_t ssr[MAXSAT];  /* SSR corrections */
    //    lexeph_t lexeph[MAXSAT]; /* LEX ephemeris */
    //    lexion_t lexion;    /* LEX ionosphere correction */
} nav_t;

typedef struct
{                         /* station parameter type */
    char name[MAXANT];    /* marker name */
    char marker[MAXANT];  /* marker number */
    char antdes[MAXANT];  /* antenna descriptor */
    char antsno[MAXANT];  /* antenna serial number */
    char rectype[MAXANT]; /* receiver type descriptor */
    char recver[MAXANT];  /* receiver firmware version */
    char recsno[MAXANT];  /* receiver serial number */
    int antsetup;         /* antenna setup id */
    int itrf;             /* ITRF realization year */
    int deltype;          /* antenna delta type (0:enu,1:xyz) */
    double pos[3];        /* station position (ecef) (m) */
    double del[3];        /* antenna position delta (e/n/u or x/y/z) (m) */
    double hgt;           /* antenna height (m) */
} sta_t;

typedef struct
{                                               /* RTCM control struct type */
    int staid;                                  /* station id */
    int stah;                                   /* station health */
    int seqno;                                  /* sequence number for rtcm 2 or iods msm */
    int outtype;                                /* output message type */
    gtime_t time;                               /* message time */
    gtime_t time_s;                             /* message start time */
    obs_t obs;                                  /* observation data (uncorrected) */
    nav_t nav;                                  /* satellite ephemerides */
    sta_t sta;                                  /* station parameters */
                                                //    dgps_t *dgps;       /* output of dgps corrections */
                                                //    ssr_t ssr[MAXSAT];  /* output of ssr corrections */
    char msg[128];                              /* special message */
    char msgtype[256];                          /* last message type */
    char msmtype[6][128];                       /* msm signal types */
    int obsflag;                                /* obs data complete flag (1:ok,0:not complete) */
    int ephsat;                                 /* update satellite of ephemeris */
    double cp[MAXSAT][NFREQ + NEXOBS];          /* carrier-phase measurement */
    unsigned char lock[MAXSAT][NFREQ + NEXOBS]; /* lock time */
    unsigned char loss[MAXSAT][NFREQ + NEXOBS]; /* loss of lock count */
    gtime_t lltime[MAXSAT][NFREQ + NEXOBS];     /* last lock time */
    int nbyte;                                  /* number of bytes in message buffer */
    int nbit;                                   /* number of bits in word buffer */
    int len;                                    /* message length (bytes) */
    unsigned char buff[1200];                   /* message buffer */
    unsigned int word;                          /* word buffer for rtcm 2 */
    unsigned int nmsg2[100];                    /* message count of RTCM 2 (1-99:1-99,0:other) */
    unsigned int nmsg3[300];                    /* message count of RTCM 3 (1-299:1001-1299,0:ohter) */
    char opt[256];                              /* RTCM dependent options */
} rtcm_t;

typedef struct
{                                                /* receiver raw data control type */
    gtime_t time;                                /* message time???? */
    gtime_t tobs;                                /* observation data time??????? */
    obs_t obs;                                   /* observation data???? */
    obs_t obuf;                                  /* observation data buffer?????? */
    nav_t nav;                                   /* satellite ephemerides????? */
    sta_t sta;                                   /* station parameters??? */
    int ephsat;                                  /* sat number of update ephemeris (0:no satellite)??????? */
    sbsmsg_t sbsmsg;                             /* SBAS message */
    char msgtype[256];                           /* last message type ????????*/
    unsigned char subfrm[MAXSAT][150];           /* subframe buffer (1-5)???? */
    lexmsg_t lexmsg;                             /* LEX message */
    double lockt[MAXSAT][NFREQ + NEXOBS];        /* lock time (s)???? */
    double icpp[MAXSAT], off[MAXSAT], icpc;      /* carrier params for ss2ss2???? */
    double prCA[MAXSAT], dpCA[MAXSAT];           /* L1/CA pseudrange/doppler for javad JAVAD?L1/CA??/????*/
    unsigned char halfc[MAXSAT][NFREQ + NEXOBS]; /* half-cycle add flag */
    char freqn[MAXOBS];                          /* frequency number for javad */
    int nbyte;                                   /* number of bytes in message buffer */
    int len;                                     /* message length (bytes) */
    int iod;                                     /* issue of data */
    int tod;                                     /* time of day (ms) */
    int tbase;                                   /* time base (0:gpst,1:utc(usno),2:glonass,3:utc(su) */
    int flag;                                    /* general purpose flag */
    int outtype;                                 /* output message type */
    unsigned char buff[MAXRAWLEN];               /* message buffer */
                                                 //    int prn[MAXOBS];      //新添加的卫星PRN号
    char opt[256];                               /* receiver dependent options ???????,????????*/
} raw_t;
typedef struct
{
    gtime_t time;
    double rr[6]; // pos/vel (m,m/s)
    float qr[6];  // pos variance/covariance (m^2)
    /* {c_xx,c_yy,c_zz,c_xy,c_yz,c_zx} or */
    /* {c_ee,c_nn,c_uu,c_en,c_nu,c_ue} */
    double dtr[6]; // receiver clock bias
    uint8_t type;  // 0: xyz-ecef, 1:enu-baseline
    uint8_t stat;  // solution status
    uint8_t ns;    // number of valid satellites
    float age;     // age of differential (s)
    float ratio;   // for validation
    int processTime;
    int encoder;
} sol_t;
typedef struct
{
    uint8_t vs;         // valid sat flag
    double azel[2];     // azimuth, elevation angle (rad)
    double resp[NFREQ]; // residuals of pseudorange (m)
    double resc[NFREQ]; // residulas of carrier-phase (m)
    uint8_t vsat;       // valid sat flag
    uint8_t snr[NFREQ]; // signal strength (0.25dBHz)
    uint8_t fix;        // 1:fix,2:float,3:hold
    uint8_t slip;       // cycle-slip flag
    unsigned int lock;  // lock counter of phase
    unsigned int outc;  // obs outage counter of phase
    unsigned int slipc; // cycle-slip counter
    unsigned int rejc;  // reject counter
    double phw;         // phase windup
    gtime_t pt[2];      // previous carrier-phase time
    double ph[2];       // previous carrier-phase observable (cycle)
} ssat_t;
typedef struct
{
    gtime_t epoch[4];   // last epoch
    int fixcnt;         // fix counter
    char flags[MAXSAT]; // fix flags
    double n[4];        // number of epoch
    double LC[4];       // linear combination average
    double LCv[4];      // linear combination variance
} ambc_t;
typedef struct
{                          /* stream converter type */
    int itype, otype;      /* input and output stream type */
    int nmsg;              /* number of output messages */
    int msgs[32];          /* output message types */
    double tint[32];       /* output message intervals (s) */
    unsigned int tick[32]; /* cycle tick of output message */
    int ephsat[32];        /* satellites of output ephemeris */
    int stasel;            /* station info selection (0:remote,1:local) */
    rtcm_t rtcm;           /* rtcm input data buffer */
    raw_t raw;             /* raw  input data buffer */
    rtcm_t out;            /* rtcm output data buffer */
} strconv_t;

typedef struct
{
    sol_t sol;
    double rb[6];             // base position/velocity (ecef) (m,m/s)
    int nx, na;               // number of float states/fixed states
    double tt;                // time difference between current and previous
    double x[41], P[41 * 41]; // float states and their cov
    // float state:pos(3),vel(3),acc(3),sat(32)
    double xa[9], Pa[81]; // fixed stated and their cov
    // fix state:pos(3),vel(3),acc(3)
    int nfix; // number of continuous fixes of ambiguity
    ambc_t ambc[MAXSAT];
    ssat_t ssat[MAXSAT];
    int neb;                // bytes in error msg buffer
    char errbuf[MAXERRMSG]; // error msg buffer
    int errLen;
    prcopt_t opt;
} rtk_t;
typedef struct
{                             /* stream type */
    int type;                 /* type (STR_???) */
    int mode;                 /* mode (STR_MODE_?) */
    int state;                /* state (-1:error,0:close,1:open) */
    unsigned int inb, inr;    /* input bytes/rate ????/??*/
    unsigned int outb, outr;  /* output bytes/rate */
    unsigned int tick, tact;  /* tick/active tick??/???? */
    unsigned int inbt, outbt; /* input/output bytes at tick?????/???? */
                              //    lock_t lock;        /* lock flag??? */
    void *port;               /* type dependent port control struct??????????? */
                              //    char path[MAXSTRPATH]; /* stream path ?????*/
    char msg[MAXSTRMSG];      /* stream message ?????*/
} stream_t;

typedef struct
{ /* stream server type */
    rtk_t rtk;
    //    int state;          /* server state (0:stop,1:running) */
    //    int cycle;          /* server cycle (ms) */
    //    int buffsize;       /* input/monitor buffer size (bytes) */
    //    int nmeacycle;      /* NMEA request cycle (ms) (0:no) */
    //    int nstr;           /* number of streams (1 input + (nstr-1) outputs */
    //    int npb;            /* data length in peek buffer (bytes) */
    //    double nmeapos[3];  /* NMEA request position (ecef) (m) */
    //    unsigned char *buff; /* input buffers */
    //    unsigned char *pbuf; /* peek buffer */
    //    unsigned int tick;  /* start tick */
    stream_t stream[16]; /* input/output streams */
    strconv_t *conv[16]; /* stream converter */
    raw_t raw[2];
    //    thread_t thread;    /* server thread */
    //    lock_t lock;        /* lock flag */
} strsvr_t;

// raw_t raw;

/* receiver raw data functions */
extern int input_raw(raw_t *raw, int format, unsigned char data);
extern int input_rawf(raw_t *raw, int format, FILE *fp);
extern int init_raw(raw_t *raw);
extern int input_ubx(raw_t *raw, unsigned char data);
extern int decode_frame(const unsigned char *buff, eph_t *eph, alm_t *alm,
                        double *ion, double *utc, int *leaps);
extern int decode_gal_inav(const unsigned char *buff, eph_t *eph);
extern int decode_bds_d1(const unsigned char *buff, eph_t *eph);
extern int decode_bds_d2(const unsigned char *buff, eph_t *eph);
extern int test_glostr(const unsigned char *buff);
extern int decode_glostr(const unsigned char *buff, geph_t *geph);
// extern void free_raw  (raw_t *raw);
extern int input_ubxf(raw_t *raw, FILE *fp);
/* sbas functions*/
extern int sbsdecodemsg(gtime_t time, int prn, const unsigned int *words,
                        sbsmsg_t *sbsmsg);
/* RTCMN */
extern void dops(int ns, const double *azel, double elmin, double *dop);
extern int satno(int sys, int prn);
extern void time2str(gtime_t t, char *s, int n);
extern void time2epoch(gtime_t t, double *ep);
extern gtime_t gpst2time(int week, double sec);
extern gtime_t epoch2time(const double *ep);
extern double time2gpst(gtime_t t, int *week);
extern gtime_t gst2time(int week, double sec);
extern gtime_t gpst2utc(gtime_t t);
extern gtime_t bdt2gpst(gtime_t t);
extern gtime_t bdt2time(int week, double sec);
extern gtime_t utc2gpst(gtime_t t);
extern gtime_t timeadd(gtime_t t, double sec);
extern double timediff(gtime_t t1, gtime_t t2);
extern int satsys(int sat, int *prn);
extern void matcpy(double *A, const double *B, int n, int m);
extern void ecef2pos(const double *r, double *pos);
extern void pos2ecef(const double *pos, double *r);
extern double dot(const double *a, const double *b, int n);
extern void ecef2enu(const double *pos, const double *r, double *e);
extern void xyz2enu(const double *pos, double *E);
extern void matmul(const char *tr, int n, int k, int m, double alpha,
                   const double *A, const double *B, double beta, double *C);
extern unsigned int getbitu(const unsigned char *buff, int pos, int len);
extern int getbits(const unsigned char *buff, int pos, int len);
extern void setbitu(unsigned char *buff, int pos, int len, unsigned int data);
extern unsigned int crc24q(const unsigned char *buff, int len);
extern unsigned int rtk_crc24q(const unsigned char *buff, int len);
extern int decode_word(unsigned int word, unsigned char *data);
// extern int satexclude(int sat, int svh, const prcopt_t *opt);
extern int testsnr(int base, int freq, double el, double snr,
                   const snrmask_t *mask);
extern double satazel(const double *pos, const double *e, double *azel);
extern double norm(const double *a, int n);
extern double geodist(const double *rs, const double *rr, double *e);
extern double ionmodel(gtime_t t, const double *ion, const double *pos,
                       const double *azel);
extern double tropmodel(gtime_t time, const double *pos, const double *azel,
                        double humi);
extern int lsq(const double *A, const double *y, int n, int m, double *x,
               double *Q);
extern int matinv(double *A, int n);
extern void covenu(const double *pos, const double *P, double *Q);
extern void satno2id(int sat, char *id);
extern void deg2dms(double deg, double *dms, int ndec);
/* STREAMSVR */
extern void strconv(stream_t *str, strconv_t *conv, unsigned char *buff, int n);
static void rtcm2rtcm(rtcm_t *out, const rtcm_t *rtcm, int ret, int stasel);
static void raw2rtcm(rtcm_t *out, const raw_t *raw, int ret);
/* RTCM */
extern int input_rtcm2(rtcm_t *rtcm, unsigned char data);
extern int input_rtcm3(rtcm_t *rtcm, unsigned char data);
/* RTCM2 */
extern int decode_rtcm2(rtcm_t *rtcm);
static void adjhour(rtcm_t *rtcm, double zcnt);
// rtkpos
extern void rtkinit(rtk_t *rtk, const prcopt_t *opt);
extern int rtkpos(rtk_t *rtk, const obsd_t *obs, int n, nav_t *nav);
// ppp
extern int pppnx(const prcopt_t *opt);
// pntpos
extern int pntpos(const obsd_t *obs, int n, nav_t *nav,
                  const prcopt_t *opt, sol_t *sol, double *azel, ssat_t *ssat,
                  char *msg);
// ephemeris
extern void satposs(gtime_t teph, const obsd_t *obs, int n, nav_t *nav,
                    int ephopt, double *rs, double *dts, double *var, int *svh);
// solution
extern void outsol(char *res, const sol_t *sol, const double *rb);
extern int outnmea_rmc(unsigned char *buff, const sol_t *sol);
extern int outnmea_gga(unsigned char *buff, const sol_t *sol);
extern int outnmea_gsa(unsigned char *buff, const sol_t *sol,
                       const ssat_t *ssat);
extern int outnmea_gsv(unsigned char *buff, const sol_t *sol,
                       const ssat_t *ssat);
// geoid
extern double geoidh(const double *pos);
#endif
