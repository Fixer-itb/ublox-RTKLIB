/*------------------------------------------------------------------------------
 * ublox.c : ublox receiver dependent functions
 *
 *          Copyright (C) 2007-2013 by T.TAKASU, All rights reserved.
 *
 * reference :
 *     [1] ublox-AG, GPS.G3-X-03002-D, ANTARIS Positioning Engine NMEA and UBX
 *         Protocol Specification, Version 5.00, 2003
 *
 * version : $Revision: 1.2 $ $Date: 2008/07/14 00:05:05 $
 * history : 2007/10/08 1.0 new
 *           2008/06/16 1.1 separate common functions to rcvcmn.c
 *           2009/04/01 1.2 add range check of prn number
 *           2009/04/10 1.3 refactored
 *           2009/09/25 1.4 add function gen_ubx()
 *           2010/01/17 1.5 add time tag adjustment option -tadj sec
 *           2010/10/31 1.6 fix bug on playback disabled for raw data (2.4.0_p9)
 *           2011/05/27 1.7 add almanac decoding
 *                          add -EPHALL option
 *                          fix problem with ARM compiler
 *           2013/02/23 1.8 fix memory access violation problem on arm
 *                          change options -tadj to -TADJ, -invcp to -INVCP
 *-----------------------------------------------------------------------------*/
#include "rtklib.h"

#define UBXSYNC1 0xB5 /* ubx message sync code 1 */
#define UBXSYNC2 0x62 /* ubx message sync code 2 */
#define UBXCFG 0x06   /* ubx message cfg-??? */

#define ID_RXMRAW 0x0210   /* ubx message id: raw measurement data */
#define ID_RXMSFRB 0x0211  /* ubx message id: subframe buffer */
#define ID_RXMSFRBX 0x0213 /* ubx message id: raw subframe data */
#define ID_RXMRAWX 0x0215  /* ubx message id: multi-gnss raw meas data */

#define FU1 1 /* ubx message field types */
#define FU2 2
#define FU4 3
#define FI1 4
#define FI2 5
#define FI4 6
#define FR4 7
#define FR8 8
#define FS32 9

#define CPSTD_VALID 5 /* std-dev threshold of carrier-phase valid */
// static const char rcsid[]="$Id: ublox.c,v 1.2 2008/07/14 00:05:05 TTAKA Exp $";
unsigned char ublox_raw_flag = 0;
unsigned char ublox_eph_flag = 0;
/* get fields (little-endian) ------------------------------------------------*/
#define U1(p) (*((unsigned char *)(p)))
#define I1(p) (*((char *)(p)))
static unsigned short U2(unsigned char *p)
{
    unsigned short u;
    memcpy(&u, p, 2);
    return u;
}
static unsigned int U4(unsigned char *p)
{
    unsigned int u;
    memcpy(&u, p, 4);
    return u;
}
static float R4(unsigned char *p)
{
    float r;
    memcpy(&r, p, 4);
    return r;
}
static double R8(unsigned char *p)
{
    double r;
    memcpy(&r, p, 8);
    return r;
}

/* set fields (little-endian) ------------------------------------------------*/
static void setU1(unsigned char *p, unsigned char u) { *p = u; }
static void setU2(unsigned char *p, unsigned short u) { memcpy(p, &u, 2); }
static void setU4(unsigned char *p, unsigned int u) { memcpy(p, &u, 4); }
static void setI1(unsigned char *p, char i) { *p = (unsigned char)i; }
static void setI2(unsigned char *p, short i) { memcpy(p, &i, 2); }
static void setI4(unsigned char *p, int i) { memcpy(p, &i, 4); }
static void setR4(unsigned char *p, float r) { memcpy(p, &r, 4); }
static void setR8(unsigned char *p, double r) { memcpy(p, &r, 8); }

/* checksum ------------------------------------------------------------------*/
static int checksum(unsigned char *buff, int len)
{
    unsigned char cka = 0, ckb = 0;
    int i;

    for (i = 2; i < len - 2; i++)
    {
        cka += buff[i];
        ckb += cka;
    }
    return cka == buff[len - 2] && ckb == buff[len - 1];
}
/* ubx gnss indicator (ref [2] 25) -------------------------------------------*/
static int ubx_sys(int ind)
{
    switch (ind)
    {
    case 0:
        return SYS_GPS;
    case 1:
        return SYS_SBS;
    case 2:
        return SYS_GAL;
    case 3:
        return SYS_CMP;
    case 5:
        return SYS_QZS;
    case 6:
        return SYS_GLO;
    }
    return 0;
}
static void setcs(unsigned char *buff, int len)
{
    unsigned char cka = 0, ckb = 0;
    int i;

    for (i = 2; i < len - 2; i++)
    {
        cka += buff[i];
        ckb += cka;
    }
    buff[len - 2] = cka;
    buff[len - 1] = ckb;
}
/* decode ublox rxm-raw: raw measurement data --------------------------------*/
static int decode_rxmraw(raw_t *raw)
{
    gtime_t time;
    double tow, tt, tadj = 0.0, toff = 0.0, tn;
    int i, j, prn, sat, n = 0, nsat, week;
    unsigned char *p = raw->buff + 6;
    char *q, tstr[32];

    //    printf("decode_rxmraw: len=%d\n",raw->len);

    /* time tag adjustment option (-TADJ) */
    //    if ((q=strstr(raw->opt,"-TADJ"))) {  //TAD
    //        sscanf(q,"-TADJ=%lf",&tadj);
    //    }
    tow = U4(p);
    week = U2(p + 4);
    nsat = U1(p + 6);
    if (raw->len < 12 + 24 * nsat)
    {
        //        printf("ubx rxmraw length error: len=%d nsat=%d\n",raw->len,nsat);
        return -1;
    }
    time = gpst2time(week, tow * 0.001);

    /* time tag adjustment */
    if (tadj > 0.0)
    {
        tn = time2gpst(time, &week) / tadj;
        toff = (tn - floor(tn + 0.5)) * tadj;
        time = timeadd(time, -toff);
    }
    if (fabs(tt = timediff(time, raw->time)) <= 1e-3)
    {
        time2str(time, tstr, 3);
        //       printf("ubx rxmraw time tag duplicated: time=%s\n",tstr);
        return 0;
    }
    for (i = 0, p += 8; i < nsat && i < MAXOBS; i++, p += 24)
    {
        raw->obs.data[n].time = time;
        raw->obs.data[n].L[0] = R8(p) - toff * FREQ1;
        raw->obs.data[n].P[0] = R8(p + 8) - toff * CLIGHT;
        raw->obs.data[n].D[0] = R4(p + 16);
        prn = U1(p + 20);
        //			  raw->prn[n]                    =U1(p+20);
        raw->obs.data[n].SNR[0] = (unsigned char)(I1(p + 22) * 4.0 + 0.5);
        raw->obs.data[n].LLI[0] = U1(p + 23);
        raw->obs.data[n].code[0] = CODE_L1C;

        /* phase polarity flip option (-INVCP) */
        //        if (strstr(raw->opt,"-INVCP")) {
        //            raw->obs.data[n].L[0]=-raw->obs.data[n].L[0];
        //        }
        if (!(sat = satno(MINPRNSBS <= prn ? SYS_SBS : SYS_GPS, prn)))
        {
            //            printf("ubx rxmraw sat number error: prn=%d\n",prn);
            continue;
        }
        raw->obs.data[n].sat = sat;

        if (raw->obs.data[n].LLI[0] & 1)
            raw->lockt[sat - 1][0] = 0.0;
        else if (tt < 0.0 || 10.0 < tt)
            raw->lockt[sat - 1][0] = 0.0;
        else
            raw->lockt[sat - 1][0] += tt;

        for (j = 1; j < NFREQ; j++)
        {
            raw->obs.data[n].L[j] = raw->obs.data[n].P[j] = 0.0;
            raw->obs.data[n].D[j] = 0.0;
            raw->obs.data[n].SNR[j] = raw->obs.data[n].LLI[j] = 0;
            raw->obs.data[n].code[j] = CODE_NONE;
        }
        n++;
    }
    raw->time = time;
    raw->obs.n = n;

    ublox_raw_flag = 1;
    return 1;
}
/* decode ubx-rxm-rawx: multi-gnss raw measurement data (ref [3]) ------------*/
static int decode_rxmrawx(raw_t *raw)
{
    gtime_t time;
    double tow, cp1, pr1, tadj = 0.0, toff = 0.0, freq, tn;
    int i, j, sys, prn, sat, n = 0, nsat, week, tstat, lockt, slip, halfv, halfc, fcn, cpstd;
    int std_slip = 0;
    // char *q;
    unsigned char *p = raw->buff + 6;

    // trace(4,"decode_rxmrawx: len=%d\n",raw->len);

    nsat = U1(p + 11);
    if (raw->len < 24 + 32 * nsat)
    {
        // trace(2,"ubx rxmrawx length error: len=%d nsat=%d\n",raw->len,nsat);
        return -1;
    }
    tow = R8(p);
    week = U2(p + 8);
    time = gpst2time(week, tow);

    if (week == 0)
    {
        // trace(3,"ubx rxmrawx week=0 error: len=%d nsat=%d\n",raw->len,nsat);
        return 0;
    }
    //    if (raw->outtype) {
    //        sprintf(raw->msgtype,"UBX RXM-RAWX  (%4d): time=%s nsat=%d",raw->len,
    //                time_str(time,2),U1(p+11));
    //    }
    /* time tag adjustment option (-TADJ) */
    //    if ((q=strstr(raw->opt,"-TADJ="))) {
    //        sscanf(q,"-TADJ=%lf",&tadj);
    //    }
    /* slip theshold of std-dev of carreir-phase (-STD_SLIP) */
    //    if ((q=strstr(raw->opt,"-STD_SLIP="))) {
    //        sscanf(q,"-STD_SLIP=%d",&std_slip);
    //    }
    /* time tag adjustment */
    if (tadj > 0.0)
    {
        tn = time2gpst(time, &week) / tadj;
        toff = (tn - floor(tn + 0.5)) * tadj;
        time = timeadd(time, -toff);
    }
    for (i = 0, p += 16; i < nsat && i < MAXOBS; i++, p += 32)
    {

        if (!(sys = ubx_sys(U1(p + 20))))
        {
            // trace(2,"ubx rxmrawx: system error\n");
            continue;
        }
        prn = U1(p + 21) + (sys == SYS_QZS ? 192 : 0);
        if (!(sat = satno(sys, prn)))
        {
            // trace(2,"ubx rxmrawx sat number error: sys=%2d prn=%2d\n",sys,prn);
            continue;
        }
        cpstd = U1(p + 28) & 15; /* carrier-phase std-dev */
        tstat = U1(p + 30);      /* tracking status */
        pr1 = tstat & 1 ? R8(p) : 0.0;
        cp1 = tstat & 2 ? R8(p + 8) : 0.0;
        if (cp1 == -0.5 || cpstd > CPSTD_VALID)
            cp1 = 0.0; /* invalid phase */
        raw->obs.data[n].sat = sat;
        raw->obs.data[n].time = time;
        raw->obs.data[n].P[0] = pr1;
        raw->obs.data[n].L[0] = cp1;

        /* offset by time tag adjustment */
        if (toff != 0.0)
        {
            fcn = (int)U1(p + 23) - 7;
            freq = sys == SYS_CMP ? FREQ1_CMP : (sys == SYS_GLO ? FREQ1_GLO + DFRQ1_GLO * fcn : FREQ1);
            raw->obs.data[n].P[0] -= toff * CLIGHT;
            raw->obs.data[n].L[0] -= toff * freq;
        }
        raw->obs.data[n].D[0] = R4(p + 16);
        raw->obs.data[n].SNR[0] = U1(p + 26) * 4;
        raw->obs.data[n].LLI[0] = 0;
        raw->obs.data[n].code[0] =
            sys == SYS_CMP ? CODE_L1I : (sys == SYS_GAL ? CODE_L1X : CODE_L1C);

        lockt = U2(p + 24); /* lock time count (ms) */
        slip = lockt == 0 || lockt < raw->lockt[sat - 1][0] ? 1 : 0;
#if 0
        if (std_slip>0) {
            slip|=(cpstd>=std_slip)?1:0; /* slip by std-dev of cp */
        }
#else
        if (std_slip > 0)
        {
            cp1 = 0.0;
        }
#endif
        halfv = tstat & 4 ? 1 : 0; /* half cycle valid */
        halfc = tstat & 8 ? 1 : 0; /* half cycle subtracted from phase */

        if (cp1 != 0.0)
        { /* carrier-phase valid */

            /* LLI: bit1=loss-of-lock,bit2=half-cycle-invalid */
            raw->obs.data[n].LLI[0] |= slip ? LLI_SLIP : 0;
#if 0
            raw->obs.data[n].LLI[0]|=halfc!=raw->halfc[sat-1][0]?1:0;
#elif 1
            raw->obs.data[n].LLI[0] |= halfc ? LLI_HALFA : 0; /* half-cycle subtraced */
#else
            raw->obs.data[n].LLI[0] |= halfc ? LLI_HALFS : 0; /* half-cycle subtraced */
#endif
            raw->obs.data[n].LLI[0] |= halfv ? 0 : LLI_HALFC;
            raw->lockt[sat - 1][0] = lockt;
            raw->halfc[sat - 1][0] = halfc;
        }
        for (j = 1; j < NFREQ + NEXOBS; j++)
        {
            raw->obs.data[n].L[j] = raw->obs.data[n].P[j] = 0.0;
            raw->obs.data[n].D[j] = 0.0;
            raw->obs.data[n].SNR[j] = raw->obs.data[n].LLI[j] = 0;
            raw->obs.data[n].code[j] = CODE_NONE;
        }
        n++;
    }
    raw->time = time;
    raw->obs.n = n;
    ublox_raw_flag = 1;
    return 1;
}
/* save subframe -------------------------------------------------------------*/
static int save_subfrm(int sat, raw_t *raw)
{
    unsigned char *p = raw->buff + 6, *q;
    int i, j, n, id = (U4(p + 6) >> 2) & 0x7;

    //    printf("save_subfrm: sat=%2d id=%d\n",sat,id);

    if (id < 1 || 5 < id)
        return 0;

    q = raw->subfrm[sat - 1] + (id - 1) * 30;

    for (i = n = 0, p += 2; i < 10; i++, p += 4)
    {
        for (j = 23; j >= 0; j--)
        {
            *q = (*q << 1) + ((U4(p) >> j) & 1);
            if (++n % 8 == 0)
                q++;
        }
    }
    return id;
}
/* decode ephemeris ----------------------------------------------------------*/
static int decode_ephem(int sat, raw_t *raw)
{
    eph_t eph = {0};

    //    printf("decode_ephem: sat=%2d\n",sat);

    if (decode_frame(raw->subfrm[sat - 1], &eph, NULL, NULL, NULL, NULL) != 1 ||
        decode_frame(raw->subfrm[sat - 1] + 30, &eph, NULL, NULL, NULL, NULL) != 2 ||
        decode_frame(raw->subfrm[sat - 1] + 60, &eph, NULL, NULL, NULL, NULL) != 3)
        return 0;

    //    if (!strstr(raw->opt,"-EPHALL")) {
    if (eph.iode == raw->nav.eph[sat - 1].iode)
        return 0; /* unchanged */
                  //    }
    eph.sat = sat;
    raw->nav.eph[sat - 1] = eph;
    raw->ephsat = sat;
    ublox_eph_flag = 1;
    return 2;
}
/* decode almanac and ion/utc ------------------------------------------------*/
static int decode_alm1(int sat, raw_t *raw)
{
    //    printf("decode_alm1 : sat=%2d\n",sat);
    decode_frame(raw->subfrm[sat - 1] + 90, NULL, raw->nav.alm, raw->nav.ion_gps,
                 raw->nav.utc_gps, &raw->nav.leaps);
    ublox_eph_flag = 1;
    return 0;
}
/* decode almanac ------------------------------------------------------------*/
static int decode_alm2(int sat, raw_t *raw)
{
    //    printf("decode_alm2 : sat=%2d\n",sat);
    decode_frame(raw->subfrm[sat - 1] + 120, NULL, raw->nav.alm, NULL, NULL, NULL);
    ublox_eph_flag = 1;
    return 0;
}
/* decode ublox rxm-sfrb: subframe buffer ------------------------------------*/
static int decode_rxmsfrb(raw_t *raw)
{
    unsigned int words[10];
    int i, prn, sat, sys, id;
    unsigned char *p = raw->buff + 6;

    //    printf("decode_rxmsfrb: len=%d\n",raw->len);

    if (raw->len < 42)
    {
        //        printf("ubx rxmsfrb length error: len=%d\n",raw->len);
        return -1;
    }
    prn = U1(p + 1);
    if (!(sat = satno(MINPRNSBS <= prn ? SYS_SBS : SYS_GPS, prn)))
    {
        //        printf("ubx rxmsfrb satellite number error: prn=%d\n",prn);
        return -1;
    }
    sys = satsys(sat, &prn);

    if (sys == SYS_GPS)
    {
        id = save_subfrm(sat, raw);
        if (id == 3)
            return decode_ephem(sat, raw);
        if (id == 4)
            return decode_alm1(sat, raw);
        if (id == 5)
            return decode_alm2(sat, raw);
        return 0;
    }
    else if (sys == SYS_SBS)
    {
        for (i = 0, p += 2; i < 10; i++, p += 4)
            words[i] = U4(p);
        return sbsdecodemsg(raw->time, prn, words, &raw->sbsmsg) ? 3 : 0;
    }
    ublox_eph_flag = 1;
    return 0;
}
/* decode gps and qzss navigation data ---------------------------------------*/
static int decode_nav(raw_t *raw, int sat, int off)
{
    unsigned int words[10];
    int i, id;
    unsigned char *p = raw->buff + 6 + off;

    if (raw->len < 48 + off)
    {
        // trace(2,"ubx rawsfrbx length error: sat=%d len=%d\n",sat,raw->len);
        return -1;
    }
    for (i = 0; i < 10; i++, p += 4)
        words[i] = U4(p) >> 6; /* 24 bits without parity */

    id = (words[1] >> 2) & 7;
    if (id < 1 || 5 < id)
    {
        // trace(2,"ubx rawsfrbx subfrm id error: sat=%2d\n",sat);
        return -1;
    }
    for (i = 0; i < 10; i++)
    {
        setbitu(raw->subfrm[sat - 1] + (id - 1) * 30, i * 24, 24, words[i]);
    }
    if (id == 3)
        return decode_ephem(sat, raw);
    if (id == 4)
        return decode_alm1(sat, raw);
    if (id == 5)
        return decode_alm2(sat, raw);
    return 0;
}
/* decode galileo navigation data --------------------------------------------*/
static int decode_enav(raw_t *raw, int sat, int off)
{
    eph_t eph = {0};
    unsigned char *p = raw->buff + 6 + off, buff[32], crc_buff[26] = {0};
    int i, j, k, part1, page1, part2, page2, type;

    if (raw->len < 44 + off)
    {
        // trace(2,"ubx rawsfrbx length error: sat=%d len=%d\n",sat,raw->len);
        return -1;
    }
    for (i = k = 0; i < 8; i++, p += 4)
        for (j = 0; j < 4; j++)
        {
            buff[k++] = p[3 - j];
        }
    part1 = getbitu(buff, 0, 1);
    page1 = getbitu(buff, 1, 1);
    part2 = getbitu(buff + 16, 0, 1);
    page2 = getbitu(buff + 16, 1, 1);

    /* skip alert page */
    if (page1 == 1 || page2 == 1)
        return 0;

    /* test even-odd parts */
    if (part1 != 0 || part2 != 1)
    {
        // trace(2,"ubx rawsfrbx gal page even/odd error: sat=%2d\n",sat);
        return -1;
    }
    /* test crc (4(pad) + 114 + 82 bits) */
    for (i = 0, j = 4; i < 15; i++, j += 8)
        setbitu(crc_buff, j, 8, getbitu(buff, i * 8, 8));
    for (i = 0, j = 118; i < 11; i++, j += 8)
        setbitu(crc_buff, j, 8, getbitu(buff + 16, i * 8, 8));
    if (rtk_crc24q(crc_buff, 25) != getbitu(buff + 16, 82, 24))
    {
        // trace(2,"ubx rawsfrbx gal page crc error: sat=%2d\n",sat);
        return -1;
    }
    type = getbitu(buff, 2, 6); /* word type */

    /* skip word except for ephemeris, iono, utc parameters */
    if (type > 6)
        return 0;

    /* clear word 0-6 flags */
    if (type == 2)
        raw->subfrm[sat - 1][112] = 0;

    /* save page data (112 + 16 bits) to frame buffer */
    k = type * 16;
    for (i = 0, j = 2; i < 14; i++, j += 8)
        raw->subfrm[sat - 1][k++] = getbitu(buff, j, 8);
    for (i = 0, j = 2; i < 2; i++, j += 8)
        raw->subfrm[sat - 1][k++] = getbitu(buff + 16, j, 8);

    /* test word 0-6 flags */
    raw->subfrm[sat - 1][112] |= (1 << type);
    if (raw->subfrm[sat - 1][112] != 0x7F)
        return 0;

    /* decode galileo inav ephemeris */
    if (!decode_gal_inav(raw->subfrm[sat - 1], &eph))
    {
        return 0;
    }
    /* test svid consistency */
    if (eph.sat != sat)
    {
        // trace(2,"ubx rawsfrbx gal svid error: sat=%2d %2d\n",sat,eph.sat);
        return -1;
    }
    //    if (!strstr(raw->opt,"-EPHALL")) {
    if (eph.iode == raw->nav.eph[sat - 1].iode && /* unchanged */
        timediff(eph.toe, raw->nav.eph[sat - 1].toe) == 0.0 &&
        timediff(eph.toc, raw->nav.eph[sat - 1].toc) == 0.0)
        return 0;
    //    }
    eph.sat = sat;
    raw->nav.eph[sat - 1] = eph;
    raw->ephsat = sat;
    return 2;
}
/* decode beidou navigation data ---------------------------------------------*/
static int decode_cnav(raw_t *raw, int sat, int off)
{
    eph_t eph = {0};
    unsigned int words[10];
    int i, id, pgn, prn;
    unsigned char *p = raw->buff + 6 + off;

    if (raw->len < 48 + off)
    {
        //        trace(2,"ubx rawsfrbx length error: sat=%d len=%d\n",sat,raw->len);
        return -1;
    }
    for (i = 0; i < 10; i++, p += 4)
        words[i] = U4(p) & 0x3FFFFFFF; /* 30 bits */

    satsys(sat, &prn);
    id = (words[0] >> 12) & 0x07; /* subframe id (3bit) */
    if (id < 1 || 5 < id)
    {
        //      trace(2,"ubx rawsfrbx subfrm id error: sat=%2d\n",sat);
        return -1;
    }
    if (prn > 5)
    { /* IGSO/MEO */

        for (i = 0; i < 10; i++)
        {
            setbitu(raw->subfrm[sat - 1] + (id - 1) * 38, i * 30, 30, words[i]);
        }
        if (id != 3)
            return 0;

        /* decode beidou D1 ephemeris */
        if (!decode_bds_d1(raw->subfrm[sat - 1], &eph))
            return 0;
    }
    else
    { /* GEO */
        if (id != 1)
            return 0;

        /* subframe 1 */
        pgn = (words[1] >> 14) & 0x0F; /* page number (4bit) */
        if (pgn < 1 || 10 < pgn)
        {
            // trace(2,"ubx rawsfrbx page number error: sat=%2d\n",sat);
            return -1;
        }
        for (i = 0; i < 10; i++)
        {
            setbitu(raw->subfrm[sat - 1] + (pgn - 1) * 38, i * 30, 30, words[i]);
        }
        if (pgn != 10)
            return 0;

        /* decode beidou D2 ephemeris */
        if (!decode_bds_d2(raw->subfrm[sat - 1], &eph))
            return 0;
    }
    //    if (!strstr(raw->opt,"-EPHALL")) {
    if (timediff(eph.toe, raw->nav.eph[sat - 1].toe) == 0.0 &&
        eph.iode == raw->nav.eph[sat - 1].iode &&
        eph.iodc == raw->nav.eph[sat - 1].iodc)
        return 0; /* unchanged */
                  //    }
    eph.sat = sat;
    raw->nav.eph[sat - 1] = eph;
    raw->ephsat = sat;
    return 2;
}
/* decode glonass navigation data --------------------------------------------*/
static int decode_gnav(raw_t *raw, int sat, int off, int frq)
{
    geph_t geph = {0};
    int i, j, k, m, prn;
    unsigned char *p = raw->buff + 6 + off, buff[64], *fid;

    satsys(sat, &prn);

    if (raw->len < 24 + off)
    {
        // trace(2,"ubx rawsfrbx gnav length error: len=%d\n",raw->len);
        return -1;
    }
    for (i = k = 0; i < 4; i++, p += 4)
        for (j = 0; j < 4; j++)
        {
            buff[k++] = p[3 - j];
        }
    /* test hamming of glonass string */
    if (!test_glostr(buff))
    {
        // trace(2,"ubx rawsfrbx glo string hamming error: sat=%2d\n",sat);
        return -1;
    }
    m = getbitu(buff, 1, 4);
    if (m < 1 || 15 < m)
    {
        // trace(2,"ubx rawsfrbx glo string no error: sat=%2d\n",sat);
        return -1;
    }
    /* flush frame buffer if frame-id changed */
    fid = raw->subfrm[sat - 1] + 150;
    if (fid[0] != buff[12] || fid[1] != buff[13])
    {
        for (i = 0; i < 4; i++)
            memset(raw->subfrm[sat - 1] + i * 10, 0, 10);
        memcpy(fid, buff + 12, 2); /* save frame-id */
    }
    memcpy(raw->subfrm[sat - 1] + (m - 1) * 10, buff, 10);

    if (m != 4)
        return 0;

    /* decode glonass ephemeris strings */
    geph.tof = raw->time;
    if (!decode_glostr(raw->subfrm[sat - 1], &geph) || geph.sat != sat)
        return 0;
    geph.frq = frq - 7;

    //    if (!strstr(raw->opt,"-EPHALL")) {
    if (geph.iode == raw->nav.geph[prn - 1].iode)
        return 0; /* unchanged */
                  //    }
    raw->nav.geph[prn - 1] = geph;
    raw->ephsat = sat;
    return 2;
}
/* decode sbas navigation data -----------------------------------------------*/
static int decode_snav(raw_t *raw, int sat, int off)
{
    int i, j, k, prn, tow, week;
    unsigned char *p = raw->buff + 6 + off, buff[64];

    if (raw->len < 40 + off)
    {
        // trace(2,"ubx rawsfrbx snav length error: len=%d\n",raw->len);
        return -1;
    }
    tow = (int)time2gpst(timeadd(raw->time, -1.0), &week);
    satsys(sat, &prn);
    raw->sbsmsg.prn = prn;
    raw->sbsmsg.tow = tow;
    raw->sbsmsg.week = week;
    for (i = k = 0; i < 8; i++, p += 4)
        for (j = 0; j < 4; j++)
        {
            buff[k++] = p[3 - j];
        }
    memcpy(raw->sbsmsg.msg, buff, 29);
    raw->sbsmsg.msg[28] &= 0xC0;
    return 3;
}
/* decode ubx-rxm-sfrbx: raw subframe data (ref [3]) -------------------------*/
static int decode_rxmsfrbx(raw_t *raw)
{
    int prn, sat, sys;
    unsigned char *p = raw->buff + 6;

    // trace(4,"decode_rxmsfrbx: len=%d\n",raw->len);

    //    if (raw->outtype) {
    //        sprintf(raw->msgtype,"UBX RXM-SFRBX (%4d): sys=%d prn=%3d",raw->len,
    //                U1(p),U1(p+1));
    //    }
    if (!(sys = ubx_sys(U1(p))))
    {
        // trace(2,"ubx rxmsfrbx sys id error: sys=%d\n",U1(p));
        return -1;
    }
    prn = U1(p + 1) + (sys == SYS_QZS ? 192 : 0);
    if (!(sat = satno(sys, prn)))
    {
        // trace(2,"ubx rxmsfrbx sat number error: sys=%d prn=%d\n",sys,prn);
        return -1;
    }
    switch (sys)
    {
    case SYS_GPS:
        return decode_nav(raw, sat, 8);
    case SYS_QZS:
        return decode_nav(raw, sat, 8);
    case SYS_GAL:
        return decode_enav(raw, sat, 8);
    case SYS_CMP:
        return decode_cnav(raw, sat, 8);
    case SYS_GLO:
        return decode_gnav(raw, sat, 8, U1(p + 3));
    case SYS_SBS:
        return decode_snav(raw, sat, 8);
    }
    return 0;
}
/* decode ublox raw message --------------------------------------------------*/
static int decode_ubx(raw_t *raw)
{
    int type = (U1(raw->buff + 2) << 8) + U1(raw->buff + 3);

    //   printf("decode_ubx: type=%04x len=%d\n",type,raw->len);

    /* checksum */
    if (!checksum(raw->buff, raw->len))
    {
        //        printf("ubx checksum error: type=%04x len=%d\n",type,raw->len);
        return -1;
    }
    if (raw->outtype)
    {
        sprintf(raw->msgtype, "UBX 0x%04X (%4d):", type, raw->len);
    }
    switch (type)
    {
    case ID_RXMRAW:
        return decode_rxmraw(raw);
    case ID_RXMRAWX:
        return decode_rxmrawx(raw);
    case ID_RXMSFRB:
        return decode_rxmsfrb(raw);
    case ID_RXMSFRBX:
        return decode_rxmsfrbx(raw);
    }
    return 0;
}
/* sync code -----------------------------------------------------------------*/
static int sync_ubx(unsigned char *buff, unsigned char data)
{
    buff[0] = buff[1];
    buff[1] = data;
    return buff[0] == UBXSYNC1 && buff[1] == UBXSYNC2;
}
/* input ublox raw message from stream -----------------------------------------
 * fetch next ublox raw data and input a mesasge from stream
 * args   : raw_t *raw   IO     receiver raw data control struct
 *          unsigned char data I stream data (1 byte)
 * return : status (-1: error message, 0: no message, 1: input observation data,
 *                  2: input ephemeris, 3: input sbas message,
 *                  9: input ion/utc parameter)
 *
 * notes  : to specify input options, set raw->opt to the following option
 *          strings separated by spaces.
 *          指定输入选项，集原始- >选择以下选项
 *          -EPHALL    : input all ephemerides
 *          -INVCP     : invert polarity of carrier-phase
 *          -TADJ=tint : adjust time tags to multiples of tint (sec)
 *
 *-----------------------------------------------------------------------------*/
extern int input_ubx(raw_t *raw, unsigned char data)
{
    //    u1_printf("input_ubx: data=%02x\n",data);

    /* synchronize frame */
    if (raw->nbyte == 0)
    {
        if (!sync_ubx(raw->buff, data))
            return 0;
        raw->nbyte = 2;
        return 0;
    }
    raw->buff[raw->nbyte++] = data;

    if (raw->nbyte == 6)
    {
        if ((raw->len = U2(raw->buff + 4) + 8) > MAXRAWLEN)
        {
            //            u1_printf("ubx length error: len=%d\n",raw->len);
            raw->nbyte = 0;
            return -1;
        }
    }
    if (raw->nbyte < 6 || raw->nbyte < raw->len)
        return 0;
    raw->nbyte = 0;

    /* decode ublox raw message */
    return decode_ubx(raw);
}
/* input ublox raw message from file -------------------------------------------
 * fetch next ublox raw data and input a message from file
 * args   : raw_t  *raw   IO     receiver raw data control struct
 *          FILE   *fp    I      file pointer
 * return : status(-2: end of file, -1...9: same as above)
 *-----------------------------------------------------------------------------*/
extern int input_ubxf(raw_t *raw, FILE *fp)
{
    int i, data;

    //    printf("input_ubxf:\n");

    /* synchronize frame */
    if (raw->nbyte == 0)
    {
        for (i = 0;; i++)
        {
            if ((data = fgetc(fp)) == EOF)
                return -2;
            if (sync_ubx(raw->buff, (unsigned char)data))
                break;
            if (i >= 4096)
                return 0;
        }
    }
    if (fread(raw->buff + 2, 1, 4, fp) < 4)
        return -2;
    raw->nbyte = 6;

    if ((raw->len = U2(raw->buff + 4) + 8) > MAXRAWLEN)
    {
        //        printf("ubx length error: len=%d\n",raw->len);
        raw->nbyte = 0;
        return -1;
    }
    if (fread(raw->buff + 6, 1, raw->len - 6, fp) < (size_t)(raw->len - 6))
        return -2;
    raw->nbyte = 0;

    /* decode ubx raw message */
    return decode_ubx(raw);
}
/* generate ublox binary message -----------------------------------------------
 * generate ublox binary message from message string
 * args   : char  *msg   IO     message string
 *            "CFG-PRT   portid res0 res1 mode baudrate inmask outmask flags"
 *            "CFG-USB   vendid prodid res1 res2 power flags vstr pstr serino"
 *            "CFG-MSG   msgid rate0 rate1 rate2 rate3"
 *            "CFG-NMEA  filter version numsv flags"
 *            "CFG-RATE  meas nav time"
 *            "CFG-CFG   clear_mask save_mask load_mask"
 *            "CFG-TP    interval length status time_ref res adelay rdelay udelay"
 *            "CFG-NAV2  ..."
 *            "CFG-DAT   maja flat dx dy dz rotx roty rotz scale"
 *            "CFG-INF   protocolid res0 res1 mask0 mask1 mask2 mask3"
 *            "CFG-RST   navbbr reset res"
 *            "CFG-RXM   gpsmode lpmode"
 *            "CFG-ANT   flags pins"
 *            "CFG-FXN   flags treacq tacq treacqoff tacqoff ton toff res basetow"
 *            "CFG-SBAS  mode usage maxsbas res scanmode"
 *            "CFG-LIC   key0 key1 key2 key3 key4 key5"
 *            "CFG-TM    intid rate flags"
 *            "CFG-TM2   ch res0 res1 rate flags"
 *            "CFG-TMODE tmode posx posy posz posvar svinmindur svinvarlimit"
 *            "CFG-EKF   ..."
 *          unsigned char *buff O binary message
 * return : length of binary message (0: error)
 * note   : see reference [1] for details.
 *-----------------------------------------------------------------------------*/
extern int gen_ubx(const char *msg, unsigned char *buff)
{
    const char *cmd[] = {
        "PRT", "USB", "MSG", "NMEA", "RATE", "CFG", "TP", "NAV2", "DAT", "INF",
        "RST", "RXM", "ANT", "FXN", "SBAS", "LIC", "TM", "TM2", "TMODE", "EKF", ""};
    const unsigned char id[] = {
        0x00, 0x1B, 0x01, 0x17, 0x08, 0x09, 0x07, 0x1A, 0x06, 0x02,
        0x04, 0x11, 0x13, 0x0E, 0x16, 0x80, 0x10, 0x19, 0x1D, 0x12};
    const int prm[][32] = {
        {FU1, FU1, FU2, FU4, FU4, FU2, FU2, FU2, FU2},    /* PRT */
        {FU2, FU2, FU2, FU2, FU2, FU2, FS32, FS32, FS32}, /* USB */
        {FU1, FU1, FU1, FU1, FU1, FU1},                   /* MSG */
        {FU1, FU1, FU1, FU1},                             /* NMEA */
        {FU2, FU2, FU2},                                  /* RATE */
        {FU4, FU4, FU4},                                  /* CFG */
        {FU4, FU4, FI1, FU1, FU2, FI2, FI2, FI4},         /* TP */
        {FU1, FU1, FU2, FU1, FU1, FU1, FU1, FI4, FU1, FU1, FU1, FU1, FU1, FU1, FU2, FU2, FU2, FU2,
         FU2, FU1, FU1, FU2, FU4, FU4},                    /* NAV2 */
        {FR8, FR8, FR4, FR4, FR4, FR4, FR4, FR4, FR4},     /* DAT */
        {FU1, FU1, FU2, FU1, FU1, FU1, FU1},               /* INF */
        {FU2, FU1, FU1},                                   /* RST */
        {FU1, FU1},                                        /* RXM */
        {FU2, FU2},                                        /* ANT */
        {FU4, FU4, FU4, FU4, FU4, FU4, FU4, FU4},          /* FXN */
        {FU1, FU1, FU1, FU1, FU4},                         /* SBAS */
        {FU2, FU2, FU2, FU2, FU2, FU2},                    /* LIC */
        {FU4, FU4, FU4},                                   /* TM */
        {FU1, FU1, FU2, FU4, FU4},                         /* TM2 */
        {FU4, FI4, FI4, FI4, FU4, FU4, FU4},               /* TMODE */
        {FU1, FU1, FU1, FU1, FU4, FU2, FU2, FU1, FU1, FU2} /* EKF */
    };
    unsigned char *q = buff;
    char mbuff[1024], *args[32], *p;
    int i, j, n, narg = 0;

    //    printf("gen_ubxf: msg=%s\n",msg);

    strcpy(mbuff, msg);
    for (p = strtok(mbuff, " "); p && narg < 32; p = strtok(NULL, " "))
    {
        args[narg++] = p;
    }
    if (narg < 1 || strncmp(args[0], "CFG-", 4))
        return 0;

    for (i = 0; *cmd[i]; i++)
    {
        if (!strcmp(args[0] + 4, cmd[i]))
            break;
    }
    if (!*cmd[i])
        return 0;

    *q++ = UBXSYNC1;
    *q++ = UBXSYNC2;
    *q++ = UBXCFG;
    *q++ = id[i];
    q += 2;
    for (j = 1; prm[i][j - 1] > 0; j++)
    {
        switch (prm[i][j - 1])
        {
        case FU1:
            setU1(q, j < narg ? (unsigned char)atoi(args[j]) : 0);
            q += 1;
            break;
        case FU2:
            setU2(q, j < narg ? (unsigned short)atoi(args[j]) : 0);
            q += 2;
            break;
        case FU4:
            setU4(q, j < narg ? (unsigned int)atoi(args[j]) : 0);
            q += 4;
            break;
        case FI1:
            setI1(q, j < narg ? (char)atoi(args[j]) : 0);
            q += 1;
            break;
        case FI2:
            setI2(q, j < narg ? (short)atoi(args[j]) : 0);
            q += 2;
            break;
        case FI4:
            setI4(q, j < narg ? (int)atoi(args[j]) : 0);
            q += 4;
            break;
        case FR4:
            setR4(q, j < narg ? (float)atof(args[j]) : 0);
            q += 4;
            break;
        case FR8:
            setR8(q, j < narg ? (double)atof(args[j]) : 0);
            q += 8;
            break;
        case FS32:
            sprintf((char *)q, "%-32.32s", j < narg ? args[j] : "");
            q += 32;
            break;
        }
    }
    n = (int)(q - buff) + 2;
    setU2(buff + 4, (unsigned short)(n - 8));
    setcs(buff, n);

    //    printf("gen_ubxf: buff=\n");
    //		traceb(5,buff,n);
    return n;
}
